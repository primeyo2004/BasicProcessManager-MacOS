//
//  ProcessTableViewController.m
//  BasicProcessManager
//
//  Created by Jeune Prime Origines on 3/3/17.
//  Copyright © 2017 Jeune Prime Origines. All rights reserved.
//

#import "ProcessTableViewController.h"

@interface ProcessTableViewController ()

@end

@implementation ProcessTableViewController

-(id)init{
    if(self = [super init]){
    
        
        // Load up our sample data
        _processModel = [[ProcessModel alloc] init];
        
        if (nil != _processModel){
            
            // assign the delegates
            [_processModel setDelegate:self];
            
            [_processModel start];
        }

    }
    
    return self;
}


- (void)dealloc {
    NSLog(@"ProcessTableViewController::dealloc");
}

- (IBAction)processKill:(id)sender{
    
    if (nil != _processModel){
        [_processModel killProcessAtIndex: [_tableView selectedRow]];
    }
}


- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    
    if (nil != _processModel){
        return [_processModel count];
    }
    
    return 0;
}


- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {

    NSTableCellView *cellView = nil;
    
    ProcessItem *pItem = nil;
    
    if (nil != _processModel){
        pItem = [_processModel getItemAtIndex:row];
    }


    
    if (nil != pItem){
        
        // get the column id to get the right cell
        NSString *identifier = [tableColumn identifier];
        
        if ([identifier isEqualToString:@"idPID"]) {
            cellView = [tableView makeViewWithIdentifier:identifier owner:self];
            cellView.textField.stringValue = [pItem.processID stringValue];
        } else if ([identifier isEqualToString:@"idProcessName"]) {
            cellView = [tableView makeViewWithIdentifier:identifier owner:self];
            cellView.textField.stringValue = pItem.processName;
        } else if ([identifier isEqualToString:@"idUser"]) {
            cellView = [tableView makeViewWithIdentifier:identifier owner:self];
            cellView.textField.stringValue = pItem.userName;
        } else {
            NSAssert1(NO, @"Unhandled table column identifier %@", identifier);
        }
    }
    

    
    return cellView;
}

-(void)updateFromModel:(ProcessModel *)processModel{
    
    NSInteger row = [_tableView selectedRow];
    
    [_tableView reloadData];
    
    [_tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];
}

- (void)windowWillClose:(NSNotification *)notification{
    NSLog(@"windowWillClose");
   
    if (nil != _processModel){
    
        // clear the delegates
        [_processModel setDelegate:nil];
        
        [_processModel stop];
        
        _processModel = nil;
    }

}


@end
