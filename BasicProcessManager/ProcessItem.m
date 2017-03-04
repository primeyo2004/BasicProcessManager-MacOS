//
//  ProcessModel.m
//  ProcessManager
//
//  Created by Jeune Prime Origines on 1/3/17.
//  Copyright © 2017 Jeune Prime Origines. All rights reserved.
//

#import "ProcessItem.h"

@implementation ProcessItem

-(id)initWithProcID: (NSNumber*)processID andProcName:(NSString*)processName andUser:(NSString*)userName{
    if (self = [super init]){
        self.processID = processID;
        self.processName = processName;
        self.userName = userName;
    }
    
    return self;
}

@end
