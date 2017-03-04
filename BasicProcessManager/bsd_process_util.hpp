//
//  bsd_process_util.hpp
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


#ifndef bsd_process_util_hpp
#define bsd_process_util_hpp

#include <map>
#include <list>
#include <memory>
#include <unistd.h>
#include <cstdlib>
#include <string>

// forward ref
struct kinfo_proc;
struct kinfo_proc_block;


namespace primes{
    
    namespace bsd{
        
        enum e_delta_status : short {
            DELTA_STAT_ADDED    = 0x01,
            DELTA_STAT_EXISTING = 0x02,
            DELTA_STAT_REMOVED  = 0x04,
        };
        
        class bsd_process_item {
        public:
            pid_t       m_pid;
            std::string m_processName;
            std::string m_user;
        };
        
        class bsd_process_item_state {
        public:
            bsd_process_item  m_process_item;
            e_delta_status      m_delta_stat;
        };
        
        
        typedef std::map<pid_t,bsd_process_item_state>            pid_to_proc_item_state_map;
        typedef std::unique_ptr<kinfo_proc_block,void(*)(void*)>  kinfo_proc_block_ptr;
        typedef std::list<bsd_process_item_state>                 process_item_state_list;
        
        class bsd_process_util{
        public:
            bsd_process_util();
            virtual ~bsd_process_util();
            
            bsd_process_util(const bsd_process_util&) = delete;
            bsd_process_util& operator=(const bsd_process_util&) = delete;

        public:
            virtual void reload(unsigned short filter, process_item_state_list& items);
            virtual void clear();
            
            virtual void killprocess(pid_t procId);
            
            
        protected:
            kinfo_proc_block_ptr _fetch();
            void _sync(kinfo_proc_block_ptr ptr_block);
            
            
        private:
            pid_to_proc_item_state_map      m_map_pid_to_proc_item_state;
        };
        
    }


}


#endif /* bsd_process_util_hpp */
