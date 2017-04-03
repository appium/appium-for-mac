//
//  NotesWindowController.h
//  AppiumForMac
//
//  Created by Russell, Stuart on 2/11/17.
//  Copyright © 2017 Appium. All rights reserved.
//

#import <Cocoa/Cocoa.h>

extern NSString * const kNotesWindowTextKey;
extern NSString * const kShowNotesWindowOnLaunchKey;

@interface NotesWindowController : NSWindowController <NSWindowDelegate>

@property IBOutlet NSTextField *notesTextField;

@end
