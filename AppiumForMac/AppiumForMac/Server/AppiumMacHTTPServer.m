//
//  AppiumMacGCDServer.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacHTTPServer.h"
#import "AppiumForMacAppDelegate.h"

@implementation AppiumMacHTTPServer

-(id) init
{
    self = [super init];
    if (self) {
        self.handler = [AppiumMacHandlers new];
    }
    return self;
}

@end
