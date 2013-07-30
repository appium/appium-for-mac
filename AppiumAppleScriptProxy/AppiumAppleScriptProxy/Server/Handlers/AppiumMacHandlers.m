//
//  AppiumMacController.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacHandlers.h"
#import "AppiumMacHTTP303JSONResponse.h"
#import "NSData+Base64.h"
#import "Utility.h"

@implementation AppiumMacHandlers
- (id)init
{
    self = [super init];
    if (self) {
        [self setSessions:[NSMutableDictionary new]];
        [self setApplescript:[AppiumMacAppleScriptExecutor new]];
    }
    return self;
}

-(NSDictionary*) dictionaryFromPostData:(NSData*)postData
{
    if (!postData)
    {
        return [NSDictionary new];
    }
    
    NSError *error = nil;
    NSDictionary *postDict = [NSJSONSerialization JSONObjectWithData:postData options:NSJSONReadingMutableContainers error:&error];
    
    // TODO: error handling
    return postDict;
}

-(AppiumMacHTTPJSONResponse*) respondWithJson:(id)json status:(int)status session:(NSString*)session
{
    return [self respondWithJson:json status:status session:session statusCode:200];
}

-(AppiumMacHTTPJSONResponse*) respondWithJson:(id)json status:(int)status session:(NSString*)session statusCode:(NSInteger)statusCode
{
    NSMutableDictionary *responseJson = [NSMutableDictionary new];
    [responseJson setValue:[NSNumber numberWithInt:status] forKey:@"status"];
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

// GET /status
-(AppiumMacHTTPJSONResponse*) getStatus:(NSString*)path
{
    NSDictionary *buildJson = [NSDictionary dictionaryWithObjectsAndKeys:[Utility bundleVersion], @"version", [Utility bundleRevision], @"revision", [NSString stringWithFormat:@"%d", [Utility unixTimestamp]], @"time", nil];
    NSDictionary *osJson = [NSDictionary dictionaryWithObjectsAndKeys:[Utility arch], @"arch", @"Mac OS X", @"name", [Utility version], @"version", nil];
    NSDictionary *json = [NSDictionary dictionaryWithObjectsAndKeys:buildJson, @"build", osJson, @"os", nil];
    return [self respondWithJson:json status:0 session:nil];
}

// POST /session
-(AppiumMacHTTPJSONResponse*) postSession:(NSString*)path data:(NSData*)postData
{
    // generate new session key
    NSString *newSession = [Utility randomStringOfLength:8];
    while ([self.sessions objectForKey:newSession] != nil)
    {
        newSession = [Utility randomStringOfLength:8];
    }
    
    // TODO: Add capabilities support
    // set empty capabilities for now
    [self.sessions setValue:@"" forKey:newSession];
    
    // respond with the session
    return [self respondWithJson:[self.sessions objectForKey:newSession] status:0 session: newSession];
}

// GET /sessions
-(AppiumMacHTTPJSONResponse*) getSessions:(NSString*)path
{
    // respond with the session
    NSMutableArray *json = [NSMutableArray new];
    for(id key in self.sessions)
    {
        [json addObject:[NSDictionary dictionaryWithObjectsAndKeys:key, @"id", [self.sessions objectForKey:key], @"capabilities", nil]];
    }
    
    return [self respondWithJson:json status:0 session: nil];
}

// GET /session/:sessionId
-(AppiumMacHTTPJSONResponse*) getSession:(NSString*)path
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    // TODO: show error if session does not exist
    return [self respondWithJson:[self.sessions objectForKey:sessionId] status:0 session:sessionId];
}

// DELETE /session/:sessionId
-(AppiumMacHTTPJSONResponse*) deleteSession:(NSString*)path
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    if ([self.sessions objectForKey:sessionId] != nil)
    {
        [self.sessions removeObjectForKey:sessionId];
    }
    return [self respondWithJson:nil status:0 session: sessionId];
}

// /session/:sessionId/timeouts
// /session/:sessionId/timeouts/async_script
// /session/:sessionId/timeouts/implicit_wait

// GET /session/:sessionId/window_handle
-(AppiumMacHTTPJSONResponse*) getWindowHandle:(NSString*)path
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    // TODO: add error handling
    return [self respondWithJson:[self.applescript processForApplication:[self.applescript frontmostApplication]] status:0 session: sessionId];
}

// GET /session/:sessionId/window_handles
-(AppiumMacHTTPJSONResponse*) getWindowHandles:(NSString*)path
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    // TODO: add error handling
    return [self respondWithJson:[self.applescript allProcesses] status:0 session: sessionId];
}

// GET /session/:sessionId/url
-(AppiumMacHTTPJSONResponse*) getUrl:(NSString*)path
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    return [self respondWithJson:[self.applescript currentApplication] status:0 session: sessionId];
}

// POST /session/:sessionId/url
-(AppiumMacHTTPJSONResponse*) postUrl:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];
    
    // activate supplied application

    NSString *url = (NSString*)[postParams objectForKey:@"url"];
    [self.applescript activateApplication:url];
    [self.applescript setCurrentApplication:url];
    [self.applescript setCurrentProcess:[self.applescript processForApplication:url]];
    // TODO: error handling
    return [self respondWithJson:nil status:0 session: sessionId];
}

// /session/:sessionId/forward
// /session/:sessionId/back
// /session/:sessionId/refresh
// /session/:sessionId/execute
// /session/:sessionId/execute_async

// GET /session/:sessionId/screenshot
-(HTTPDataResponse*) getScreenshot:(NSString*)path
{
    system([@"/usr/sbin/screencapture -c" UTF8String]);
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSArray *classArray = [NSArray arrayWithObject:[NSImage class]];
    NSDictionary *options = [NSDictionary dictionary];
    
    BOOL foundImage = [pasteboard canReadObjectForClasses:classArray options:options];
    if (foundImage)
    {
        NSArray *objectsToPaste = [pasteboard readObjectsForClasses:classArray options:options];
        NSImage *image = [objectsToPaste objectAtIndex:0];
        NSString *base64Image = [[image TIFFRepresentation] base64EncodedString];
        return [self respondWithJson:base64Image status:0 session:[Utility getSessionFromPath:path]];
    }
    else
    {
        return [self respondWithJson:nil status:0 session: [Utility getSessionFromPath:path]];
    }
}

// /session/:sessionId/ime/available_engines
// /session/:sessionId/ime/active_engine
// /session/:sessionId/ime/activated
// /session/:sessionId/ime/deactivate
// /session/:sessionId/ime/activate
// /session/:sessionId/frame

// POST /session/:sessionId/window
-(AppiumMacHTTPJSONResponse*) postWindow:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    // activate application for supplied process
    NSString *name = (NSString*)[postParams objectForKey:@"name"];
    NSString *applicationName = [self.applescript applicationForProcessName:name];
    [self.applescript activateApplication:applicationName];
    [self.applescript setCurrentApplication:applicationName];
    [self.applescript setCurrentProcess:name];
    // TODO: error handling
    return [self respondWithJson:nil status:0 session: sessionId];
}

// DELETE /session/:sessionId/window
-(AppiumMacHTTPJSONResponse*) deleteWindow:(NSString*)path
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    
    // kill supplied process
    int pid = [self.applescript pidForProcess:[self.applescript frontmostProcess]];
    system([[NSString stringWithFormat:@"killall -9 %d", pid] UTF8String]);
    [self.applescript setCurrentApplication:nil];
    [self.applescript setCurrentProcess:nil];
    
    // TODO: error handling
    return [self respondWithJson:nil status:0 session: sessionId];
}

// /session/:sessionId/window/:windowHandle/size
// /session/:sessionId/window/:windowHandle/position
// /session/:sessionId/window/:windowHandle/maximize
// /session/:sessionId/cookie
// /session/:sessionId/cookie/:name

// GET /session/:sessionId/source
-(AppiumMacHTTPJSONResponse*) getSource:(NSString*)path
{
    NSString *sessionId = [Utility getSessionFromPath:path];
    return [self respondWithJson:[self.applescript pageSource] status:0 session: sessionId];
}

// /session/:sessionId/title
// /session/:sessionId/element
// /session/:sessionId/elements
// /session/:sessionId/element/active
// /session/:sessionId/element/:id
// /session/:sessionId/element/:id/element
// /session/:sessionId/element/:id/elements
// /session/:sessionId/element/:id/click
// /session/:sessionId/element/:id/submit
// /session/:sessionId/element/:id/text
// /session/:sessionId/element/:id/value
// /session/:sessionId/keys
// /session/:sessionId/element/:id/name
// /session/:sessionId/element/:id/clear
// /session/:sessionId/element/:id/selected
// /session/:sessionId/element/:id/enabled
// /session/:sessionId/element/:id/attribute/:name
// /session/:sessionId/element/:id/equals/:other
// /session/:sessionId/element/:id/displayed
// /session/:sessionId/element/:id/location
// /session/:sessionId/element/:id/location_in_view
// /session/:sessionId/element/:id/size
// /session/:sessionId/element/:id/css/:propertyName
// /session/:sessionId/orientation
// /session/:sessionId/alert_text
// /session/:sessionId/accept_alert
// /session/:sessionId/dismiss_alert
// /session/:sessionId/moveto
// /session/:sessionId/click
// /session/:sessionId/buttondown
// /session/:sessionId/buttonup
// /session/:sessionId/doubleclick
// /session/:sessionId/touch/click
// /session/:sessionId/touch/down
// /session/:sessionId/touch/up
// /session/:sessionId/touch/move
// /session/:sessionId/touch/scroll
// /session/:sessionId/touch/scroll
// /session/:sessionId/touch/doubleclick
// /session/:sessionId/touch/longclick
// /session/:sessionId/touch/flick
// /session/:sessionId/touch/flick
// /session/:sessionId/location
// /session/:sessionId/local_storage
// /session/:sessionId/local_storage/key/:key
// /session/:sessionId/local_storage/size
// /session/:sessionId/session_storage
// /session/:sessionId/session_storage/key/:key
// /session/:sessionId/session_storage/size
// /session/:sessionId/log
// /session/:sessionId/log/types
// /session/:sessionId/application_cache/status

@end