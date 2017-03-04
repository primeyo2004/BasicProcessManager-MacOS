//
//  ProcessModel.h
//  ProcessManager
//
//  Created by Jeune Prime Origines on 1/3/17.
//  Copyright © 2017 Jeune Prime Origines. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ProcessItem : NSObject

@property (nonatomic,copy) NSNumber*  processID;
@property (nonatomic,copy) NSString*  processName;
@property (nonatomic,copy) NSString*  userName;

-(id)initWithProcID: (NSNumber*)processID andProcName:(NSString*)processName andUser:(NSString*)userName;

@end
