//
//  AppiumMacGCDServer.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "GCDWebServer.h"
#import "AppiumMacHandler.h"

@interface AppiumMacGCDServer : GCDWebServer

@property AppiumMacHandler *handler;

@end
