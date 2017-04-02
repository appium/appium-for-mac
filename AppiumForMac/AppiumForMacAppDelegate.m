//
//  AppiumAppDelegate.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/27/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumForMacAppDelegate.h"

#import "AfMHTTPServer.h"
#import "AfMHTTPConnection.h"
#import "AXPathUtility.h"
#import "HTTPServer.h"
#import "DDLog.h"
#import "DDTTYLogger.h"
#import "NotesWindowController.h"
#import "Utility.h"

static const int ddLogLevel = LOG_LEVEL_VERBOSE;

@interface AppiumForMacAppDelegate ()
@property id fnKeyGlobalMonitor;
@property id fnKeyLocalMonitor;
@property id fnKeyTimer;
@property AXPathUtility *axPathUtility;
@end

@implementation AppiumForMacAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [DDLog addLogger:[DDTTYLogger sharedInstance]];
    self.afmHTTPServer = [[AfMHTTPServer alloc] init];
    [self.afmHTTPServer setType:@"_http._tcp."];
    [self.afmHTTPServer setPort:4622];
    [self.afmHTTPServer setName:[NSString stringWithFormat:@"Appium for Mac (%@)", [Utility bundleVersion]]];
    [self.afmHTTPServer setConnectionClass:[AfMHTTPConnection class]];
	NSError *error = nil;
	if(![self.afmHTTPServer start:&error])
	{
		DDLogError(@"Error starting HTTP Server: %@", error);
	}
    
    self.notesWC = [[NotesWindowController alloc] initWithWindowNibName:@"NotesWindow"];
    BOOL shouldShowNotesWindow = [[NSUserDefaults standardUserDefaults] boolForKey:kShowNotesWindowOnLaunchKey];
    if (shouldShowNotesWindow) {
        [self showNotesWindow:nil];
    }
    self.axPathUtility = [[AXPathUtility alloc] init];
    [self startMonitoring];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
{
    BOOL notesWindowShowing = self.notesWC.window.visible;
    [[NSUserDefaults standardUserDefaults] setBool:notesWindowShowing forKey:kShowNotesWindowOnLaunchKey];
    
    return NSTerminateNow;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    [self stopMonitoring];
}

- (void)startMonitoring
{
    [self addGlobalMonitorForFnKey];
    [self addLocalMonitorForFnKey];
}

- (void)stopMonitoring
{
    [self stopFnKeyTimer];
    [self removeMonitors];
}

// Press the fn key for a second to take a snapshot of the AXPath under the mouse.
// It will also cancel any existing WebDriver sessions.
- (void)addGlobalMonitorForFnKey
{
    self.fnKeyGlobalMonitor = [NSEvent addGlobalMonitorForEventsMatchingMask:NSFlagsChangedMask handler:^(NSEvent *event) {
        NSEventModifierFlags newFlags = [NSEvent modifierFlags];
        
        if (newFlags & NSFunctionKeyMask) {
            // Start a timer to detect how long the function key is held down.
            [self startFnKeyTimer];
        } else {
            // The function key is no longer held down. Disable the timer.
            [self stopFnKeyTimer];
        }
    }];
}

// Press the fn key for a few seconds to take a snapshot of the AXPath under the mouse.
// It will also cancel any existing WebDriver sessions.
- (void)addLocalMonitorForFnKey
{
    self.fnKeyLocalMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSFlagsChangedMask handler:^NSEvent *(NSEvent *event) {
        NSEventModifierFlags newFlags = [NSEvent modifierFlags];
        
        if (newFlags & NSFunctionKeyMask) {
            // Start a timer to detect how long the function key is held down.
            [self startFnKeyTimer];
        } else {
            // The function key is no longer held down. Disable the timer.
            [self stopFnKeyTimer];
        }
        return event;
    }];
}

- (void)removeMonitors
{
    if (self.fnKeyGlobalMonitor) {
        [NSEvent removeMonitor:self.fnKeyGlobalMonitor];
        self.fnKeyGlobalMonitor = nil;
    }
    if (self.fnKeyLocalMonitor) {
        [NSEvent removeMonitor:self.fnKeyLocalMonitor];
        self.fnKeyLocalMonitor = nil;
    }
}

- (void)startFnKeyTimer
{
    [self stopFnKeyTimer];
    NSTimeInterval const kTriggerDelay = 1.0;
    //self.fnKeyTimer = [NSTimer scheduledTimerWithTimeInterval:kTriggerDelay target:self selector:@selector(fnKeyTimer:) userInfo:nil repeats:NO];
    
    dispatch_time_t fnKeyDelay = dispatch_time(DISPATCH_TIME_NOW, kTriggerDelay * NSEC_PER_SEC);
    dispatch_after(fnKeyDelay, dispatch_get_main_queue(), ^{ @autoreleasepool {
        [self fnKeyTimer:nil];
    }});
}

- (void)stopFnKeyTimer
{
    if ([self.fnKeyTimer isValid]) {
        [self.fnKeyTimer invalidate];
    }
    self.fnKeyTimer = nil;
}

- (void)fnKeyTimer:(NSTimer *)timer
{
    [[NSNotificationCenter defaultCenter] postNotificationName:AFMFnKeyTriggerNotification object:nil];
}

- (IBAction)showNotesWindow:(id)sender
{
    [[self notesWC] showWindow:nil];
}

@end
