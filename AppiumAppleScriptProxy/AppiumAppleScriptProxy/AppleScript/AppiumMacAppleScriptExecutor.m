//
//  AppiumMacAppleScriptLibrary.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacAppleScriptExecutor.h"

@implementation AppiumMacAppleScriptExecutor

- (id)init
{
    self = [super init];
    if (self) {
        [self setCurrentApplication:nil];
        [self setCurrentProcess:nil];
        [self setFinder:[SBApplication applicationWithBundleIdentifier:@"com.apple.finder"]];
        [self setSystemEvents:[SBApplication applicationWithBundleIdentifier:@"com.apple.systemevents"]];
    }
    return self;
}

-(NSArray*) allProcesses
{
    NSMutableArray *processes = [NSMutableArray new];
    for(SystemEventsProcess *process in [self.systemEvents processes])
    {
        [processes addObject:[process name]];
    }
    return processes;
}

-(void) activateApplication:(NSString*)applicationName
{
    NSDictionary *errorDict;
    NSAppleScript *activateScript = [[NSAppleScript alloc] initWithSource:
        [NSString stringWithFormat:@"tell application \"%@\" to activate", applicationName]];
    [activateScript executeAndReturnError:&errorDict];
    // TODO: Add error handling
}

-(NSString*) applicationForProcessName:(NSString*)processName
{
    NSDictionary *errorDict;
    NSAppleScript *appForProcNameScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset process_bid to get the bundle identifier of process \"%@\"\nset application_name to file of (application processes where bundle identifier is process_bid)\nend tell\nreturn application_name", processName]];
    NSString *statusString = [[appForProcNameScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return statusString;
}

-(NSString*) frontmostApplication
{
    NSDictionary *errorDict;
    NSAppleScript *frontMostApplicationScript = [[NSAppleScript alloc] initWithSource:
    @"tell application \"Finder\"\nset appPath to the path to the frontmost application\nset appName to the name of file appPath\nset appName to text 1 thru ((offset of \".\" in appName) - 1) of appName\nend tell"];
    NSString *statusString = [[frontMostApplicationScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return statusString;
}

-(NSString*) frontmostProcess
{
    return [self processForApplication:[self frontmostApplication]];
}

-(NSDictionary*) pageSource
{
    NSMutableArray *children = [NSMutableArray new];
    NSDictionary *source = [NSDictionary dictionaryWithObject:children forKey:@"source"];
    for (SystemEventsProcess *process in [self.systemEvents processes])
    {
        if ([process.name isEqualToString:self.currentProcess])
        {
            for(SystemEventsUIElement *element in process.entireContents)
            {
                [children addObject:[NSString stringWithFormat:@"%@ %@",                 element.classDescription, element.name]];
            }
            break;
        }
    }
    return source;
}

-(int) pidForProcess:(NSString*)processName
{
    NSDictionary *errorDict;
    NSAppleScript *pidScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\" to return unix id of process \"%@\"", processName]];
    int pid = [[pidScript executeAndReturnError:&errorDict] int32Value];
    // TODO: Add error handling
    return pid;
    
}

-(NSString*) processForApplication:(NSString*) applicationName
{
    NSDictionary *errorDict;
    NSAppleScript *fronstMostProcessScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset application_id to (get the id of application \"%@\" as string)\nset process_name to name of (application processes where bundle identifier is application_id)\nend tell\nreturn item 1 of process_name as text", applicationName]];
    NSString *statusString = [[fronstMostProcessScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return statusString;
}

@end
