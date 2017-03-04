//
//  ProcessModel.h
//  ProcessManager
//
//  Created by Jeune Prime Origines on 1/3/17.
//  Copyright © 2017 Jeune Prime Origines. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "ProcessItem.h"



@class ProcessModel;
@protocol ProcessModelDelegate <NSObject>
@optional
-(void)updateFromModel:(ProcessModel *)processModel;
@end

@interface ProcessModel : NSObject
{
    @private
    NSTimer*          _refreshTimer;
    NSMutableArray*   _processItems;
    dispatch_group_t  _task_disp_group;
    NSError*          _error;
    
}


@property (weak) id <ProcessModelDelegate> delegate;



-(void)start;
-(void)stop;



-(void)killProcessAtIndex:(NSInteger) index;

-(ProcessItem*) getItemAtIndex:(NSInteger) index;

-(NSUInteger) count;


@end
