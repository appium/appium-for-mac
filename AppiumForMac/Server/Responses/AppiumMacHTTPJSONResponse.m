//
//  AppiumMacHTTPJSONResponse.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacHTTPJSONResponse.h"
#import "AppiumMacHTTP303JSONResponse.h"
#import "AfMStatusCodes.h"
#import "HTTPLogging.h"

// Commented out unused variable to avoid compiler warning.
//static const int httpLogLevel = HTTP_LOG_LEVEL_WARN;

@implementation AppiumMacHTTPJSONResponse

+(AppiumMacHTTPJSONResponse *) responseWithJson:(id)json status:(int)status session:(NSString*)session
{
    return [self responseWithJson:json status:status session:session statusCode:200];
}

+ (AppiumMacHTTPJSONResponse *)responseWithJson:(id)json status:(int)status session:(NSString*)session statusCode:(NSInteger)statusCode
{
    NSMutableDictionary *responseJson = [NSMutableDictionary new];
    [responseJson setValue:[NSNumber numberWithInteger:status] forKey:@"status"];
    if (session != nil)
    {
        [responseJson setValue:session forKey:@"sessionId"];
    }
    [responseJson setValue:json forKey:@"value"];
    
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:responseJson
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:&error];
    if (!jsonData)
    {
        NSLog(@"Got an error: %@", error);
        jsonData = [NSJSONSerialization dataWithJSONObject:
                    [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:-1], @"status", session, @"session", [NSString stringWithFormat:@"%@", error], @"value" , nil]
                                                   options:NSJSONWritingPrettyPrinted
                                                     error:&error];
    }
    switch (statusCode)
    {
        case 303:
            return [[AppiumMacHTTP303JSONResponse alloc] initWithData:jsonData];
        default:
            return [[AppiumMacHTTPJSONResponse alloc] initWithData:jsonData];
    }
}

+ (AppiumMacHTTPJSONResponse *)responseWithJsonError:(int)statusCode session:(NSString*)sessionId
{
    NSDictionary *errorJson = [NSDictionary dictionaryWithObjectsAndKeys:kAfMStatusCodeMessages[statusCode],@"message" , nil];
    return [self responseWithJson:errorJson status:statusCode session:sessionId];
}


- (NSDictionary *)httpHeaders
{
    return [NSDictionary dictionaryWithObjectsAndKeys:
            @"*", @"Access-Control-Allow-Origin",
            @"GET,PUT,POST,OPTIONS,DELETE", @"Access-Control-Allow-Methods",
            @"origin, content-type, accept", @"Access-Control-Allow-Headers",
            @"application/json; charset=utf-8", @"Content-Type",
            nil];
}

// Returns the AfM status code, e.g. kAfMStatusCodeSuccess for success
- (NSInteger)afmStatusCode
{
    id jsonResponseObject = [NSJSONSerialization JSONObjectWithData:data options:0 error:NULL];
    if ([jsonResponseObject isKindOfClass:[NSDictionary class]]) {
        id statusValue = [jsonResponseObject valueForKey:@"status"];
        if ([statusValue isKindOfClass:[NSNumber class]]) {
            return [statusValue integerValue];
        }
    }
    // Default is success
    return kAfMStatusCodeSuccess;
}

- (NSString *)description
{
    NSInteger statusCode = 0;
    id jsonResponseObject = [NSJSONSerialization JSONObjectWithData:data options:0 error:NULL];
    if ([jsonResponseObject isKindOfClass:[NSDictionary class]]) {
        id statusValue = [jsonResponseObject valueForKey:@"status"];
        if ([statusValue isKindOfClass:[NSNumber class]]) {
            statusCode = [statusValue integerValue];
        }
    }
    NSString *descStr = [NSString stringWithFormat:@"<AppiumMacHTTPJSONResponse %p afmStatusCode:%ld jsonObject: \n%@", self, statusCode, jsonResponseObject];
    return descStr;
}


@end
