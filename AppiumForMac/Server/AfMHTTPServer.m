//
//  AppiumMacGCDServer.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMHTTPServer.h"
#import "AppiumForMacAppDelegate.h"

@implementation AfMHTTPServer

-(id) init
{
    self = [super init];
    if (self) {
        self.handler = [AfMHandlers new];
    }
    return self;
}

@end
