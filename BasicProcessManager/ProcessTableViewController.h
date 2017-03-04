//
//  ProcessTableViewController.h
//  BasicProcessManager
//
//  Created by Jeune Prime Origines on 3/3/17.
//  Copyright © 2017 Jeune Prime Origines. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "ProcessModel.h"

@interface ProcessTableViewController : NSObject<NSTableViewDelegate, NSTableViewDataSource,ProcessModelDelegate> {
@private
    // An array of dictionaries that contain the contents to display
    ProcessModel* _processModel;
    IBOutlet NSTableView *_tableView;
}

- (IBAction)processKill:(id)sender;

@end
