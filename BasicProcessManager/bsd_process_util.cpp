//
//  bsd_process_util.cpp
//  BasicProcessManager
//
//  Created by Jeune Prime Origines on 28/2/17.
//  Copyright © 2017 Jeune Prime Origines. All rights reserved.
//

// Code based from GetBSDProcessList: http://www.cocoabuilder.com/archive/cocoa/92971-bsd-processes-with-code.html

// Note:
// Apprently [[NSWorkspace sharedWorkspace] runningApplications] seems so convenient but after taking a closer look
// it actually couldn't give what is needed in this challenge so have to do it in the BSD way
// ended up writing my version of GetBSDProcessList

#include <errno.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <pwd.h>
#include <system_error>
#include <signal.h>
#include "bsd_process_util.hpp"

typedef struct kinfo_proc kinfo_proc;

struct kinfo_proc_block{
    size_t       elements;
    kinfo_proc*  pblock;
};

namespace primes{
namespace bsd{


typedef std::unique_ptr< kinfo_proc_block,void(*)(void*) >   kinfo_proc_block_ptr;
    
    
bsd_process_util::bsd_process_util(){
    
}

bsd_process_util::~bsd_process_util(){

}
    


void bsd_process_util::reload(unsigned short filter,process_item_state_list& items){
    
    items.clear();
    
    kinfo_proc_block_ptr ptr_block = _fetch();
    _sync(std::move(ptr_block));
    
    // read the matching flag
    std::for_each(m_map_pid_to_proc_item_state.begin(),
                  m_map_pid_to_proc_item_state.end(),
                  [&](std::pair<const pid_t,bsd_process_item_state>& item){
                      
                      
                      if (item.second.m_delta_stat & filter){
                
                          items.push_back(item.second);
 
                      }
                  });

    
}
    

void bsd_process_util::clear(){
    m_map_pid_to_proc_item_state.clear();
}
    
void bsd_process_util::killprocess(pid_t procId){
    
    int err = kill(procId,SIGKILL);
    
    //std::cout << "killprocess: " <<procId << "ret = " << err << " errno= " << errno << std::endl;
    
    if (err == -1){
        // throw err = errno;
        throw std::system_error(errno, std::system_category());
    }
    
}

kinfo_proc_block_ptr bsd_process_util::_fetch(){
    
    kinfo_proc_block_ptr ptr_block{nullptr,std::free};

    while (!ptr_block.get()){

       size_t block_size = 0;
       static int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
       int    err = sysctl (name, (sizeof(name)/sizeof(name[0])) - 1, NULL, &block_size, NULL, 0 );

       if (-1 == err){
          // throw err = errno;
          throw std::system_error(errno, std::system_category());
       }

        kinfo_proc_block_ptr ptr_block_temp {
            reinterpret_cast<kinfo_proc_block*>(std::malloc(sizeof(kinfo_proc_block) + block_size)),
            std::free };
   
        if (!ptr_block_temp.get()){
            // throw exception ENOMEM
            throw std::system_error(ENOMEM, std::system_category());
        }

        ptr_block_temp->elements = block_size / sizeof(kinfo_proc);
        ptr_block_temp->pblock   = reinterpret_cast<kinfo_proc*>(ptr_block_temp.get() + sizeof(kinfo_proc_block));

        err = sysctl (name, (sizeof(name)/sizeof(name[0])) - 1, ptr_block_temp->pblock , &block_size, NULL, 0 );


       if (-1 == err){
          err = errno;
       }

       switch(err){
           case 0:
               // we are done, move the pointer
               // recalc the actual size fetched
               ptr_block_temp->elements = block_size / sizeof(kinfo_proc);
               ptr_block = std::move(ptr_block_temp);
               
               break;
           case ENOMEM:
               // continue with the loop and repopulate
               break;
           default:
               // throw the error, it something else
               throw std::system_error(errno, std::system_category());
               break;
       }
    }
   
    return ptr_block;
}

void bsd_process_util::_sync(kinfo_proc_block_ptr ptr_block){
    
    pid_to_proc_item_state_map::iterator itr;
    
    // removed all the previously marked as removed
    do{
        itr  = std::find_if(m_map_pid_to_proc_item_state.begin(),
                    m_map_pid_to_proc_item_state.end(),
                    [&](std::pair<const pid_t,bsd_process_item_state>& item){
                        
                        return ( item.second.m_delta_stat == e_delta_status::DELTA_STAT_REMOVED);
                        
                            });
        
        if (itr != m_map_pid_to_proc_item_state.end()){
            m_map_pid_to_proc_item_state.erase(itr++);
        }
        
        
    }while(itr != m_map_pid_to_proc_item_state.end());
 
    
           
    // mark everthing as removed
    std::for_each(m_map_pid_to_proc_item_state.begin(),
                  m_map_pid_to_proc_item_state.end(),
                  [&](std::pair<const pid_t,bsd_process_item_state>& item){
                      item.second.m_delta_stat = e_delta_status::DELTA_STAT_REMOVED;
                  });

    // check if the PID is in the map
    for (size_t i = 0; i < ptr_block->elements; i++){
        pid_to_proc_item_state_map::iterator itr = m_map_pid_to_proc_item_state.find(ptr_block->pblock[i].kp_proc.p_pid);
        
        if (m_map_pid_to_proc_item_state.end() != itr){
            // update the status to existing
            itr->second.m_delta_stat = e_delta_status::DELTA_STAT_EXISTING;
        }
        else{
            
            bsd_process_item_state& proc_item_state  = m_map_pid_to_proc_item_state[ptr_block->pblock[i].kp_proc.p_pid];
            
            struct passwd *user = getpwuid(ptr_block->pblock[i].kp_eproc.e_ucred.cr_uid);
            
            proc_item_state.m_process_item.m_pid         = ptr_block->pblock[i].kp_proc.p_pid;
            proc_item_state.m_process_item.m_processName = std::string(ptr_block->pblock[i].kp_proc.p_comm);
            
            if (user){
                proc_item_state.m_process_item.m_user    = std::string(user->pw_name);
            }
    
            proc_item_state.m_delta_stat = e_delta_status::DELTA_STAT_ADDED;
        }
        
    }
}
    



}
}
