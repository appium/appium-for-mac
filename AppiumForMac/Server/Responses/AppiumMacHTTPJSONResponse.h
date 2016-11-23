//
//  AppiumMacHTTPJSONResponse.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "HTTPDataResponse.h"

@interface AppiumMacHTTPJSONResponse : HTTPDataResponse

- (NSInteger)afmStatusCode;

+ (AppiumMacHTTPJSONResponse *)responseWithJson:(id)json status:(int)status session:(NSString*)session;

+ (AppiumMacHTTPJSONResponse *)responseWithJson:(id)json status:(int)status session:(NSString*)session statusCode:(NSInteger)statusCode;

+ (AppiumMacHTTPJSONResponse *)responseWithJsonError:(int)statusCode session:(NSString*)sessionId;

@end
