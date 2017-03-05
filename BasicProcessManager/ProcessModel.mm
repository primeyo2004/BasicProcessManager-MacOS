//
//  ProcessModel.m
//  ProcessManager
//
//  Created by Jeune Prime Origines on 1/3/17.
//  Copyright © 2017 Jeune Prime Origines. All rights reserved.
//

#import "ProcessModel.h"
#include "bsd_process_util.hpp"
#include <system_error>
#include <map>

using namespace primes::bsd;

typedef std::shared_ptr<bsd_process_util>     bsd_process_util_ptr;

// Now we can support multiple instance of bsd_process_util
static std::map<void*,bsd_process_util_ptr>  l_process_mon_map;


@implementation ProcessModel

-(id)init{
    
    if (self = [super init]){
        
        _processItems = [[NSMutableArray alloc] init];
        _refreshTimer = [NSTimer scheduledTimerWithTimeInterval:2.0 target:self selector:@selector(poll) userInfo:nil repeats:YES];
        
        // thread synchronization
        _task_disp_group = dispatch_group_create();
        
        // Creating our own  associated instance of bsd_process_util
        l_process_mon_map[(__bridge void*)self] = std::make_shared<bsd_process_util>();
        
    }
    
    return self;
}


-(void)poll{
    
   

    
    dispatch_queue_t global_queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    
    dispatch_group_enter(_task_disp_group);
    
    // Let the process reload be done on different thread
    dispatch_async(global_queue, ^{
     
    
        std::shared_ptr<process_item_state_list> plstItems(new process_item_state_list());
        
        try{
            
            // no need for GCD since this is just onetime call
            l_process_mon_map[(__bridge void*)self]->reload(static_cast<unsigned>(e_delta_status::DELTA_STAT_ADDED) |
                                 static_cast<unsigned>(e_delta_status::DELTA_STAT_REMOVED),*(plstItems.get()));

        }
        catch (std::system_error& error)
        {
            _error = [[NSError alloc] initWithDomain: NSPOSIXErrorDomain
                                                code: error.code().value()
                                            userInfo: nil ];
            
            
            dispatch_async(dispatch_get_main_queue(), ^{
            
                // display on the main thread
                NSAlert *alert = [NSAlert alertWithError:_error];
                [alert runModal];
            
            });
        }
        
            
       
        
        // push back to the main thread to update the GUI
        dispatch_async(dispatch_get_main_queue(), ^{
        
            process_item_state_list::iterator itr;
            for (itr = plstItems->begin();itr != plstItems->end(); itr++){
                
                bsd_process_item_state& item = (*itr);
                
                if (item.m_delta_stat == e_delta_status::DELTA_STAT_ADDED){
                    
                    [self addItem: [[ProcessItem alloc] initWithProcID:
                                    [NSNumber numberWithInt:item.m_process_item.m_pid]
                                                           andProcName:[NSString
                                                                        stringWithFormat: @"%s",item.m_process_item.m_processName.c_str()]
                                                               andUser:[NSString
                                                                        stringWithFormat: @"%s",item.m_process_item.m_user.c_str()]
                                    
                                    ]];
                    
                    
                }
                else if (item.m_delta_stat == e_delta_status::DELTA_STAT_REMOVED){
                    
                    [self deleteItem: [[ProcessItem alloc] initWithProcID:
                                       [NSNumber numberWithInt:item.m_process_item.m_pid]
                                                              andProcName:[NSString
                                                                           stringWithFormat: @"%s",item.m_process_item.m_processName.c_str()]
                                                                  andUser:[NSString
                                                                           stringWithFormat: @"%s",item.m_process_item.m_user.c_str()]
                                       ]];
                }
                
                
            }
            
            
            if (plstItems->size()){
                // only refresh if there are changes
                [_delegate updateFromModel:self];
            }
            
            
        });
        
         dispatch_group_leave(_task_disp_group);
    });
    

}

-(void)start{
    [_refreshTimer fire];
}

-(void)stop{
    [_refreshTimer invalidate];
    dispatch_group_wait(_task_disp_group, DISPATCH_TIME_FOREVER );
}



-(void)killProcessAtIndex:(NSInteger) index{
    
    NSLog(@"Killing Process at %ld",index);
    
    ProcessItem* pItem = [self getItemAtIndex: index];
    
    if (nil != pItem){
        
        int pidToKill = (int) [pItem.processID integerValue];
        
        dispatch_queue_t global_queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        
        dispatch_group_enter(_task_disp_group);
        
        // Let the process reload be done on different thread
        dispatch_async(global_queue, ^{
        
            try{
                
                // run from GCD
                bsd_process_util::killprocess(pidToKill);
            }
            catch (std::system_error& error)
            {
                _error = [[NSError alloc] initWithDomain: NSPOSIXErrorDomain
                                                    code: error.code().value()
                                                userInfo: nil ];
                
                // push back to the main thread to pop the error
                dispatch_async(dispatch_get_main_queue(), ^{
                    
                    // display on the main thread
                    NSAlert *alert = [NSAlert alertWithError:_error];
                    [alert runModal];
                    
                });
            }
            
            dispatch_group_leave(_task_disp_group);
        });
        
    }
    
    
}

-(ProcessItem*) getItemAtIndex:(NSInteger) index{
    
    if (nil != _processItems){
        
       return [_processItems objectAtIndex:index];
        
    }
    return nil;
}

-(NSUInteger) count{
    
    if (nil != _processItems){
        
        return [_processItems count];
    }
    
    return 0;
}



-(void)addItem: (ProcessItem*) item{
    
    if (nil != _processItems){
        
        [_processItems addObject:item];
    }
}

-(void)deleteItem: (ProcessItem*) item{
    
    if (nil != _processItems) {
         
        for(NSUInteger i = 0; i < [_processItems count]; i++){
            
            ProcessItem *o = [_processItems objectAtIndex:i];
            
            if (nil != o && o.processID == item.processID){
                
                [_processItems removeObjectAtIndex:i];
            }
        }
     }
}

-(void)dealloc{
    NSLog(@"ProcessModel::dealloc");
    
    // release the owned object
    l_process_mon_map[(__bridge void*)self]  =  bsd_process_util_ptr();
    
}



@end
