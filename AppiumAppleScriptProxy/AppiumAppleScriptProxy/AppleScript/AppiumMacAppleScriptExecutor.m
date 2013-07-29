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
        [self setCurrentProcess: @"Finder"];
        [self setFinder:[SBApplication applicationWithBundleIdentifier:@"com.apple.finder"]];
        [self setSystemEvents:[SBApplication applicationWithBundleIdentifier:@"com.apple.systemevents"]];
    }
    return self;
}

-(NSString*) frontmostApplication
{
    NSDictionary *errorDict;
    NSAppleScript *frontMostApplicationScript = [[NSAppleScript alloc] initWithSource:
    @"tell application \"Finder\"\nset appPath to the path to the frontmost application\nset appName to the name of file appPath\nset appName to text 1 thru ((offset of \".\" in appName) - 1) of appName\nend tell"];
    NSString *statusString =
    [[frontMostApplicationScript executeAndReturnError:&errorDict] stringValue];
    return statusString;
}

-(NSDictionary*) pageSource
{
    // TODO: Implement page-souce
    return nil;
}

@end
