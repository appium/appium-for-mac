//
//  AppiumMacHTTPJSONResponse.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacHTTPJSONResponse.h"
#import "HTTPLogging.h"

static const int httpLogLevel = HTTP_LOG_LEVEL_WARN;

@implementation AppiumMacHTTPJSONResponse

-(NSDictionary*) httpHeaders
{
    return [NSDictionary dictionaryWithObjectsAndKeys:
            @"*", @"Access-Control-Allow-Origin",
            @"GET,PUT,POST,OPTIONS,DELETE", @"Access-Control-Allow-Methods",
            @"origin, content-type, accept", @"Access-Control-Allow-Headers",
            @"application/json; charset=utf-8", @"Content-Type",
            nil];
}

@end
