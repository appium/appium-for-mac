//
//  AppiumAppDelegate.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/27/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppiumMacHTTPServer.h"
#define SERVER [(AppiumAppleScriptProxyAppDelegate*)[[NSApplication sharedApplication] delegate] server]

@interface AppiumAppleScriptProxyAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property AppiumMacHTTPServer *server;

@end
