//
//  AppiumMacGCDServer.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacHandlers.h"
#import "HTTPServer.h"

@interface AppiumMacHTTPServer : HTTPServer

@property AppiumMacHandlers *handler;

@end
