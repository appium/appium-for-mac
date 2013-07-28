//
//  AppiumAppDelegate.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/27/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumAppleScriptProxyAppDelegate.h"

#import "AppiumMacGCDServer.h"

@implementation AppiumAppleScriptProxyAppDelegate

-(void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    self.server = [[AppiumMacGCDServer alloc] init];
    [self.server runWithPort:8080];
}

@end
