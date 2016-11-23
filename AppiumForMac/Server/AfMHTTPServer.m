//
//  AfMHTTPServer.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMHTTPServer.h"

@implementation AfMHTTPServer

-(id) init
{
    self = [super init];
    if (self) {
        self.handlers = [AfMHandlers new];
    }
    return self;
}

@end
