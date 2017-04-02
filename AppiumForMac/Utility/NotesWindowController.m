//
//  NotesWindowController.m
//  AppiumForMac
//
//  Created by Russell, Stuart on 2/11/17.
//  Copyright © 2017 Appium. All rights reserved.
//

#import "NotesWindowController.h"
#import "AXPathUtility.h"

NSString * const kNotesWindowTextKey = @"notesWindowText";
NSString * const kShowNotesWindowOnLaunchKey = @"showNotesWindowOnLaunch";

@interface NotesWindowController ()

@end

@implementation NotesWindowController

- (instancetype)initWithWindowNibName:(NSString *)windowNibName
{
    self = [super initWithWindowNibName:windowNibName];
    if (self) {
    }
    return self;
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    NSString *savedString = [[NSUserDefaults standardUserDefaults] stringForKey:kNotesWindowTextKey];
    if (savedString) {
        [self.notesTextField setStringValue:savedString];
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        NSText* textEditor = [self.window fieldEditor:YES forObject:self.notesTextField];
        NSRange range = {savedString.length, 0};
        [textEditor setSelectedRange:range];
    });
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(newTextPosted:) name:kNewTextPostedNotification object:nil];
}

- (void)newTextPosted:(NSNotification *)note
{
    NSString *newString = note.object;
    newString = [newString stringByAppendingString:@"\n\n"];
    NSString *totalString = [[self.notesTextField stringValue] stringByAppendingString:newString];
    [self.notesTextField setStringValue:totalString];
    NSText* textEditor = [self.window fieldEditor:YES forObject:self.notesTextField];
    NSRange range = {totalString.length, 0};
    [textEditor setSelectedRange:range];
    [textEditor scrollRangeToVisible:range];
    
    [[NSUserDefaults standardUserDefaults] setObject:totalString forKey:kNotesWindowTextKey];
}

- (void)windowWillClose:(NSNotification *)notification
{
    NSString *notesString = [self.notesTextField stringValue];
    if (notesString == nil) {
        notesString = @"";
    }
    [[NSUserDefaults standardUserDefaults] setObject:notesString forKey:kNotesWindowTextKey];
}


- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end
