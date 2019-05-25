//
//  AfMHandlers.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Improvements by Stuart Russell at Intuit.
//  Copyright (c) 2013-2016 Appium. All rights reserved.
//

#import "AfMHandlers.h"
#import <PFAssistive/PFAssistive.h>

#import "AfMElementLocator.h"
#import "AfMSessionController.h"
#import "AfMStatusCodes.h"
#import "AppiumMacHTTP303JSONResponse.h"
#import "AppiumMacHTTPJSONResponse.h"
#import "HTTPMessage.h"
#import "NSData+Base64.h"
#import "Utility.h"
#import "DDLog.h"
static const int ddLogLevel = LOG_LEVEL_VERBOSE;

@implementation AfMHandlers
- (id)init
{
    self = [super init];
    if (self) {
        [self setSessions:[NSMutableDictionary new]];
    }
    return self;
}

-(AfMSessionController*) controllerForSession:(NSString*)sessionId
{
    return [self.sessions objectForKey:sessionId];
}

// This convenience method uses the path to select a session, then sends this message to the session instance.
- (AppiumMacHTTPJSONResponse *)executeWebDriverCommandWithPath:(NSString *)path data:(NSData *)postData onMainThread:(BOOL)onMainThread 
                                                  commandBlock:(AppiumMacHTTPJSONResponse *(^)(AfMSessionController *session, NSDictionary *commandParams, int *statusCode))commandBlock
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    
    if (!session) {
        return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchDriver session:session.sessionId];
    }
    
    return [session executeWebDriverCommandWithPath:path data:postData onMainThread:onMainThread commandBlock:commandBlock];
}

// GET /status
// Query the server's current status.
- (AppiumMacHTTPJSONResponse *)get_status:(NSString*)path
{
    // This can not use executeWebDriverCommandWithPath because that method requires a sessionId in the path.
    
    NSDictionary *buildJson = [NSDictionary dictionaryWithObjectsAndKeys:[Utility bundleVersion], @"version", [Utility bundleRevision], @"revision", [NSString stringWithFormat:@"%d", [Utility unixTimestamp]], @"time", nil];
    NSDictionary *osJson = [NSDictionary dictionaryWithObjectsAndKeys:[Utility arch], @"arch", @"Mac OS X", @"name", [Utility version], @"version", nil];
    NSDictionary *json = [NSDictionary dictionaryWithObjectsAndKeys:buildJson, @"build", osJson, @"os", nil];
    DDLogCInfo(@"Request:GET /status");
    DDLogCInfo(@"Response:%@,status:%lu", osJson,kAfMStatusCodeSuccess);
    return [AppiumMacHTTPJSONResponse responseWithJson:json status:kAfMStatusCodeSuccess session:nil];
}

// POST /session
// Create a new session.
// https://w3c.github.io/webdriver/webdriver-spec.html#new-session
- (AppiumMacHTTPJSONResponse *)post_session:(NSString*)path data:(NSData*)postData
{
    // This can not use executeWebDriverCommandWithPath because that method requires a sessionId in the path.
    DDLogCInfo(@"Request:POST /session");
    NSDictionary *postParams = [Utility dictionaryFromPostData:postData];
    NSDictionary *desiredCapabilities = [postParams objectForKey:@"desiredCapabilities"];
    DDLogCInfo(@"create Session with desiredCapabilities:%@", desiredCapabilities);
    // We can only have so many sessions, and no more.
    if ([self.sessions count] >= kMaximumActiveSessions) {
        DDLogCInfo(@"The limit for Maximum Active session is Reached now:%d", kMaximumActiveSessions);
        return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeSessionNotCreatedException session:nil];
    }
    
    // Create a new session. It will generate its own sessionId. Use that as a key for our session dictionary.
    AfMSessionController *newSession = [[AfMSessionController alloc] init];
    [newSession setDesiredCapabilities:desiredCapabilities];
    [self.sessions setValue:newSession forKey:newSession.sessionId];
    DDLogCInfo(@"Response:");
    DDLogCInfo(@"Capabilities:%@",newSession.capabilities);
    DDLogCInfo(@"session:%@",newSession.sessionId);
    DDLogCInfo(@"status:%lu",kAfMStatusCodeSuccess);
    return [AppiumMacHTTPJSONResponse responseWithJson:newSession.capabilities status:kAfMStatusCodeSuccess session:newSession.sessionId];
}

// GET /sessions
// Returns a list of the currently active sessions.
- (AppiumMacHTTPJSONResponse *)get_sessions:(NSString*)path
{
    // This can not use executeWebDriverCommandWithPath because that method requires a sessionId in the path.
    DDLogCInfo(@"Request:GET /sessions");
    NSMutableArray *json = [NSMutableArray new];
    for (id key in self.sessions) {
        AfMSessionController *session = [self.sessions objectForKey:key];
        [json addObject:@{@"id":key, @"capabilities":session.capabilities}];
    }
    DDLogCInfo(@"Response: %@",json);
    DDLogCInfo(@"status:%lu",kAfMStatusCodeSuccess);
    return [AppiumMacHTTPJSONResponse responseWithJson:json status:kAfMStatusCodeSuccess session: nil];
}

// GET /session/:sessionId
// Retrieve the capabilities of the specified session.
- (AppiumMacHTTPJSONResponse *)get_session:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:GET /session/: %@",session.sessionId);
        DDLogCInfo(@"Response: %@",session.capabilities);
        return [AppiumMacHTTPJSONResponse responseWithJson:session.capabilities status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// DELETE /session/:sessionId
// Delete the session.
- (AppiumMacHTTPJSONResponse *)delete_session:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        if (session != nil) {
            [self.sessions removeObjectForKey:session.sessionId];
        }
        DDLogCInfo(@"Request:DELETE /session/: %@",session.sessionId);
        DDLogCInfo(@"Response: %@",session.capabilities);
        DDLogCInfo(@"status: %lu",kAfMStatusCodeSuccess);
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/timeouts
// Configure the amount of time that a particular type of operation can execute for before they are aborted and a |Timeout| error is returned to the client.
- (AppiumMacHTTPJSONResponse *)post_timeouts:(NSString*)path data:(NSData*)postData;
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/timeouts",session.sessionId);
        NSTimeInterval seconds = 0;
        NSNumber *msValue = [commandParams objectForKey:@"ms"];
        if (msValue && [msValue isKindOfClass:[NSNumber class]]) {
            seconds = [msValue unsignedIntegerValue] / 1000;
        }
        // Valid values are: "script" for script timeouts, "implicit" for modifying the implicit wait timeout and "page load" for setting a page load timeout.
        NSString *type = [commandParams objectForKey:@"type"];
        if (type && [type isKindOfClass:[NSString class]]) {
            if ([type isEqualToString:@"implicit"]) {
                session.implicitTimeout = seconds;
                DDLogCInfo(@"implicitTimeout :%f",seconds);
            } else if ([type isEqualToString:@"page load"]) {
                session.pageLoadTimeout = seconds;
                DDLogCInfo(@"pageLoadTimeout :%f",seconds);
            } else if ([type isEqualToString:@"script"]) {
                session.scriptTimeout = seconds;
                DDLogCInfo(@"scriptTimeout :%f",seconds);
            } else {
                return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownCommand session:session.sessionId];
            }
        }
        
        return [AppiumMacHTTPJSONResponse responseWithJson:session.currentWindowHandle status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/timeouts/async_script
// Set the amount of time, in milliseconds, that asynchronous scripts executed by /session/:sessionId/execute_async are permitted to run before they are aborted and a |Timeout| error is returned to the client.
- (AppiumMacHTTPJSONResponse *)post_timeouts_async_script:(NSString*)path data:(NSData*)postData;
{
    
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/timeouts/async_script",session.sessionId);
        NSNumber *msValue = [commandParams objectForKey:@"ms"];
        if (msValue && [msValue isKindOfClass:[NSNumber class]]) {
            session.scriptTimeout = [msValue unsignedIntegerValue] / 1000;
        }
        return [AppiumMacHTTPJSONResponse responseWithJson:session.currentWindowHandle status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/timeouts/implicit_wait
// Set the amount of time the driver should wait when searching for elements. 
- (AppiumMacHTTPJSONResponse *)post_timeouts_implicit_wait:(NSString*)path data:(NSData*)postData;
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/timeouts/implicit_wait",session.sessionId);
        NSNumber *msValue = [commandParams objectForKey:@"ms"];
        if (msValue && [msValue isKindOfClass:[NSNumber class]]) {
            session.implicitTimeout = [msValue unsignedIntegerValue] / 1000;
        }
        return [AppiumMacHTTPJSONResponse responseWithJson:session.currentWindowHandle status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/window_handle
// Retrieve the current window handle.
- (AppiumMacHTTPJSONResponse *)get_window_handle:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/timeouts/async_script",session.sessionId);
        if ([session.currentWindowHandle intValue] >= session.allWindows.count)
        {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
        }
        
        return [AppiumMacHTTPJSONResponse responseWithJson:session.currentWindowHandle status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/window_handles
// Retrieve the list of all window handles available to the session.
- (AppiumMacHTTPJSONResponse *)get_window_handles:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        return [AppiumMacHTTPJSONResponse responseWithJson:session.allWindowHandles status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/url
// Retrieve the URL of the current page.
- (AppiumMacHTTPJSONResponse *)get_url:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        return [AppiumMacHTTPJSONResponse responseWithJson:session.currentApplication.AXTitle status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/url
// Navigate to a new URL.
- (AppiumMacHTTPJSONResponse *)post_url:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        // In AppiumForMac, a "url" is an application name or url string, not a hypertext link.
        // e.g. url == "Calculator" will launch Calculator.app.
        NSString *url = (NSString*)[commandParams objectForKey:@"url"];
        
        return [session postURL:url];
    }];
}

// POST /session/:sessionId/forward
// Navigate forwards in the browser history, if possible.

// POST /session/:sessionId/back
// Navigate backwards in the browser history, if possible.

// POST /session/:sessionId/refresh
// Refresh the current page.

// POST /session/:sessionId/execute
// Inject a snippet of JavaScript into the page for execution in the context of the currently selected frame. The executed script is assumed to be synchronous and the result of evaluating the script is returned to the client.

// POST /session/:sessionId/execute_async
// Inject a snippet of JavaScript into the page for execution in the context of the currently selected frame. The executed script is assumed to be asynchronous and must signal that is done by invoking the provided callback, which is always provided as the final argument to the function. 

// GET /session/:sessionId/screenshot
// Take a screenshot of the current page.
-(HTTPDataResponse*)get_screenshot:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        // Ask the session to put a screen shot into the clipboard.
        if ([session screenCaptureToClipboard]) {
            NSArray *objectsToPaste = [[NSPasteboard generalPasteboard] readObjectsForClasses:@[[NSImage class]] options:@{}];
            NSImage *image = [objectsToPaste objectAtIndex:0];
            NSString *base64Image = [[image TIFFRepresentation] base64EncodedString];
            return [AppiumMacHTTPJSONResponse responseWithJson:base64Image status:kAfMStatusCodeSuccess session:session.sessionId];
        }
        else {
            return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
        }
    }];
}

// GET /session/:sessionId/ime/available_engines
// List all available engines on the machine. To use an engine, it has to be present in this list.

// GET /session/:sessionId/ime/active_engine
// Get the name of the active IME engine. The name string is platform specific.

// GET /session/:sessionId/ime/activated
// Indicates whether IME input is active at the moment (not if it's available).

// POST /session/:sessionId/ime/deactivate
// De-activates the currently-active IME engine.

// POST /session/:sessionId/ime/activate
// Make an engine that is available (appears on the list returned by getAvailableEngines) active. 

// POST /session/:sessionId/frame
// Change focus to another frame on the page. If the frame id is null, the server should switch to the page's default content.

// POST /session/:sessionId/frame/parent
// Change focus to the parent context. If the current context is the top level browsing context, the context remains unchanged.

// POST /session/:sessionId/window
// Change focus to another window. The window to change focus to may be specified by its server assigned window handle, or by the value of its name attribute.
- (AppiumMacHTTPJSONResponse *)post_window:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        // activate application for supplied process
        NSString *windowHandle = (NSString*)[commandParams objectForKey:@"name"];
        if ([windowHandle intValue] >= session.allWindows.count)
        {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
        }
        
        [session setCurrentWindowHandle:windowHandle];
        [session activateWindow];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// DELETE /session/:sessionId/window
// Close the current window.
- (AppiumMacHTTPJSONResponse *)delete_window:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        // check that there is at least one window
        NSUInteger originalWindowCount = session.allWindows.count;
        if (originalWindowCount < 1)
        {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
        }
        
        // close the window
        [session closeWindow];
        return (session.allWindows.count < originalWindowCount) ? [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId] : [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
    }];
}

// POST /session/:sessionId/window/:windowHandle/size
// Change the size of the specified window. If the :windowHandle URL parameter is "current", the currently active window will be resized.
- (AppiumMacHTTPJSONResponse *)post_window_size:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
        if (windowHandle == nil) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
        }
        
        PFUIElement *window = nil;
        if ([windowHandle isEqualToString:@"current"]) {
            window = [session currentWindow];
        } else {
            window = [session windowForHandle:windowHandle];
        }
                
        CGFloat width = [(NSNumber *)[commandParams objectForKey:@"width"] floatValue];
        CGFloat height = [(NSNumber *)[commandParams objectForKey:@"height"] floatValue];
        
        NSSize size = [window.AXSize sizeValue];
        size.width = width;
        size.height = height;
        
        [window setAXSize:[NSValue valueWithSize:size]];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/window/:windowHandle/size
// Get the size of the specified window. If the :windowHandle URL parameter is "current", the size of the currently active window will be returned.
- (AppiumMacHTTPJSONResponse *)get_window_size:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
        if (windowHandle == nil) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
        }
        
        PFUIElement *window = nil;
        if ([windowHandle isEqualToString:@"current"]) {
            window = [session currentWindow];
        } else {
            window = [session windowForHandle:windowHandle];
        }
        
        NSSize size = [window.AXSize sizeValue];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:size.width], @"width", [NSNumber numberWithFloat:size.height], @"height", nil] status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/window/:windowHandle/position
// Change the position of the specified window. If the :windowHandle URL parameter is "current", the currently active window will be moved.
- (AppiumMacHTTPJSONResponse *)post_window_position:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
        if (windowHandle == nil) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
        }
        
        PFUIElement *window = nil;
        if ([windowHandle isEqualToString:@"current"]) {
            window = [session currentWindow];
        } else {
            window = [session windowForHandle:windowHandle];
        }
        
        CGFloat x = [(NSNumber*)[commandParams objectForKey:@"x"] floatValue];
        CGFloat y = [(NSNumber*)[commandParams objectForKey:@"y"] floatValue];
        
        NSPoint position = [window.AXPosition pointValue];
        position.x = x;
        position.y = y;
        
        [window setAXPosition:[NSValue valueWithPoint:position]];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/window/:windowHandle/position
// Get the position of the specified window. If the :windowHandle URL parameter is "current", the position of the currently active window will be returned.
- (AppiumMacHTTPJSONResponse *)get_window_position:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
        if (windowHandle == nil) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchWindow session:session.sessionId];
        }
        
        PFUIElement *window = nil;
        if ([windowHandle isEqualToString:@"current"]) {
            window = [session currentWindow];
        } else {
            window = [session windowForHandle:windowHandle];
        }
        
        NSPoint position = [[window AXPosition] pointValue];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:position.x], "x", [NSNumber numberWithFloat:position.y], @"y", nil] status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/window/:windowHandle/maximize
// Maximize the specified window if not already maximized. If the :windowHandle URL parameter is "current", the currently active window will be maximized.

// GET /session/:sessionId/cookie
// Retrieve all cookies visible to the current page.
- (AppiumMacHTTPJSONResponse *)get_cookie:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        NSArray *allCookies = [session getAllCookies];
        DDLogCInfo(@"get_cookie allCookies:%@", allCookies);
        
        return [AppiumMacHTTPJSONResponse responseWithJson:allCookies status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/cookie
// Set a cookie. If the cookie path is not specified, it should be set to "/". Likewise, if the domain is omitted, it should default to the current page's domain.
- (AppiumMacHTTPJSONResponse *)post_cookie:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogInfo(@"post_cookie commandParams:%@", commandParams);
        NSDictionary *cookie = commandParams[@"cookie"];
        [session setCookie:cookie];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// DELETE /session/:sessionId/cookie
// Delete all cookies visible to the current page.

// DELETE /session/:sessionId/cookie/:name
// Delete the cookie with the given name. This command should be a no-op if there is no such cookie visible to the current page.

// GET /session/:sessionId/source
// Get the current page source.
- (AppiumMacHTTPJSONResponse *)get_source:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        // xml page source
        NSString *pageSource = [[NSString alloc]initWithData:[session xmlPageSource].XMLData encoding:NSUTF8StringEncoding];
        //NSLog(@"%@", pageSource);
        return [AppiumMacHTTPJSONResponse responseWithJson:pageSource status:kAfMStatusCodeSuccess session:session.sessionId];
        
        // json page source
        //return [AppiumMacHTTPJSONResponse responseWithJson:[session pageSource] status:0 session:session.sessionId];
    }];
}

// POST /session/:sessionId/element
// Search for an element on the page, starting from the document root. 
- (AppiumMacHTTPJSONResponse *)post_element:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        NSString *using = (NSString*)[commandParams objectForKey:@"using"];
        NSString *value = (NSString*)[commandParams objectForKey:@"value"];
        
        AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];
        
        // initialize status as though no element were found
        *statusCode = kAfMStatusCodeNoSuchElement;
        
        if (locator != nil)
        {
            PFUIElement *element = [locator findUsingBaseUIElement:nil statusCode:statusCode];
            if (element != nil)
            {
                session.elementIndex++;
                NSString *myKey = [NSString stringWithFormat:@"%d", session.elementIndex];
                [session.elements setValue:element forKey:myKey];
                return [AppiumMacHTTPJSONResponse responseWithJson:[NSDictionary dictionaryWithObject:myKey forKey:@"ELEMENT"] status:kAfMStatusCodeSuccess session:session.sessionId];
            }
        }
        
        return [AppiumMacHTTPJSONResponse responseWithJsonError:*statusCode session:session.sessionId];
    }];
}

// POST /session/:sessionId/elements
// Search for multiple elements on the page, starting from the document root. 
- (AppiumMacHTTPJSONResponse *)post_elements:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        NSString *using = (NSString*)[commandParams objectForKey:@"using"];
        NSString *value = (NSString*)[commandParams objectForKey:@"value"];
        
        AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];
        
        // initialize status as though no element were found
        *statusCode = kAfMStatusCodeNoSuchElement;
        
        if (locator != nil)
        {
            NSMutableArray *matches = [NSMutableArray new];
            [locator findAllUsingBaseUIElement:nil results:matches statusCode:statusCode];
            if (matches.count > 0)
            {
                NSMutableArray *elements = [NSMutableArray new];
                for(PFUIElement *element in matches)
                {
                    session.elementIndex++;
                    NSString *myKey = [NSString stringWithFormat:@"%d", session.elementIndex];
                    [session.elements setValue:element forKey:myKey];
                    [elements addObject:[NSDictionary dictionaryWithObject:myKey forKey:@"ELEMENT"]];
                }
                
                return [AppiumMacHTTPJSONResponse responseWithJson:elements status:kAfMStatusCodeSuccess session:session.sessionId];
            }
            return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeNoSuchElement session:session.sessionId];
        }
        return [AppiumMacHTTPJSONResponse responseWithJsonError:*statusCode session:session.sessionId];
    }];
}

// POST /session/:sessionId/element/active
// Get the element on the page that currently has focus. 

// GET /session/:sessionId/element/:id
// Describe the identified element.

// POST /session/:sessionId/element/:id/element
// Search for an element on the page, starting from the identified element. 
- (AppiumMacHTTPJSONResponse *)post_element_element:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *rootUIElement = [commandParams objectForKey:@"elementObject"];
        NSString *using = (NSString*)[commandParams objectForKey:@"using"];
        NSString *value = (NSString*)[commandParams objectForKey:@"value"];
        
        AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];
        
        // initialize status as though no element were found
        *statusCode = kAfMStatusCodeNoSuchElement;
        
        if (locator != nil)
        {
            PFUIElement *uiElement = [locator findUsingBaseUIElement:rootUIElement statusCode:statusCode];
            if (uiElement != nil)
            {
                session.elementIndex++;
                NSString *myKey = [NSString stringWithFormat:@"%d", session.elementIndex];
                [session.elements setValue:uiElement forKey:myKey];
                return [AppiumMacHTTPJSONResponse responseWithJson:[NSDictionary dictionaryWithObject:myKey forKey:@"ELEMENT"] status:kAfMStatusCodeSuccess session:session.sessionId];
            }
        }
        
        return [AppiumMacHTTPJSONResponse responseWithJsonError:*statusCode session:session.sessionId];
    }];
}

// POST /session/:sessionId/element/:id/elements
// Search for multiple elements on the page, starting from the identified element. 
- (AppiumMacHTTPJSONResponse *)post_element_elements:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *rootUIElement = [commandParams objectForKey:@"elementObject"];
        NSString *using = (NSString*)[commandParams objectForKey:@"using"];
        NSString *value = (NSString*)[commandParams objectForKey:@"value"];
        
        AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];
        
        // initialize status as though no element were found
        *statusCode = kAfMStatusCodeNoSuchElement;
        
        if (locator != nil)
        {
            NSMutableArray *matches = [NSMutableArray new];
            [locator findAllUsingBaseUIElement:rootUIElement results:matches statusCode:statusCode];
            if (matches.count > 0)
            {
                NSMutableArray *elements = [NSMutableArray new];
                for(PFUIElement *uiElement in matches)
                {
                    session.elementIndex++;
                    NSString *myKey = [NSString stringWithFormat:@"%d", session.elementIndex];
                    [session.elements setValue:uiElement forKey:myKey];
                    [elements addObject:[NSDictionary dictionaryWithObject:myKey forKey:@"ELEMENT"]];
                }
                return [AppiumMacHTTPJSONResponse responseWithJson:elements status:kAfMStatusCodeSuccess session:session.sessionId];
            }
        }
        
        return [AppiumMacHTTPJSONResponse responseWithJsonError:*statusCode session:session.sessionId];
    }];
}


// POST /session/:sessionId/element/:id/click
// Click on an element.
-(AppiumMacHTTPJSONResponse *)post_element_click:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        
        id element = [commandParams objectForKey:@"elementObject"];
        DDLogCInfo(@"Request:POST /session/:%@//element/:%@/click",session.sessionId,element);
        if (![session isElementDisplayed:element]) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeElementNotVisible session:session.sessionId];
        }
        
        BOOL result = [session clickElement:element];
        
        return result ? [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId] :
        [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownError session:session.sessionId];
    }];
}

// POST /session/:sessionId/element/:id/submit
// Submit a FORM element. The submit command may also be applied to any element that is a descendant of a FORM element.

// GET /session/:sessionId/element/:id/text
// Returns the visible text for the element.
- (AppiumMacHTTPJSONResponse *)get_element_text:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/text",session.sessionId,element);
        id valueAttribute = element.AXValue;
        if (valueAttribute != nil)
        {
            NSString *text = [NSString stringWithFormat:@"%@", valueAttribute];
            return [AppiumMacHTTPJSONResponse responseWithJson:text status:kAfMStatusCodeSuccess session:session.sessionId];
        }
        
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeUnknownError session:session.sessionId];
    }];
}

// POST /session/:sessionId/element/:id/value
// Send a sequence of key strokes to an element.
- (AppiumMacHTTPJSONResponse *)post_element_value:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        NSArray *value = [commandParams objectForKey:@"value"];
        DDLogCInfo(@"Request:POST /session/:%@/element/:%@/value",session.sessionId,element);
        // For some reason Selenium is sending element.setValue() when a sendKeys() command was requested.
        // If element is a field that must receive keystrokes to trigger other behaviors, then type the value.
        if ([element.AXRole isEqualToString:@"AXTextField"]) {
            [session clickElement:element];
            [session sendKeys:value toElement:element];
        } else {
            // Not a text field so set the value programmatically, without typing.
            // This is the original Appium solution for every AXRole.
            [element setAXValue:[value componentsJoinedByString:@""]];
        }    
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/keys
// Send a sequence of key strokes to the active element. This command is similar to the 'value' command in every aspect except the implicit termination: The modifiers are _not_ released at the end of the call. Rather, the state of the modifier keys is kept between calls, so mouse interactions can be performed while modifier keys are depressed.
- (AppiumMacHTTPJSONResponse *)post_keys:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {   DDLogCInfo(@"Request:POST /session/:%@/keys",session.sessionId);
        NSArray *value = [commandParams objectForKey:@"value"];
        [session sendKeys:value];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];        
    }];
}

// GET /session/:sessionId/element/:id/name
// Query for an element's tag name.
- (AppiumMacHTTPJSONResponse *)get_element_name:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/name",session.sessionId,element.AXIdentifier);
        
        return [AppiumMacHTTPJSONResponse responseWithJson:element.AXTitle status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// POST /session/:sessionId/element/:id/clear
// Clear a TEXTAREA or text INPUT element's value.
- (AppiumMacHTTPJSONResponse *)post_element_clear:(NSString*)path data:(NSData*)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        DDLogCInfo(@"Request:POST /session/:%@/element/:%@/clear",session.sessionId,element.AXIdentifier);
        if (![session isElementDisplayed:element]) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeElementNotVisible session:session.sessionId];
        }
        
        id value = [element AXValue];
        if (value != nil && [value isKindOfClass:[NSString class]])
        {
            [element setAXValue:@""];
        }
        
        // TODO: Add error handling
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/element/:id/selected
// Determine if an OPTION element, or an INPUT element of type checkbox or radiobutton is currently selected.
- (AppiumMacHTTPJSONResponse *)get_element_selected:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/selected",session.sessionId,element.AXIdentifier);
        
        return [AppiumMacHTTPJSONResponse responseWithJson:element.AXFocused status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/element/:id/enabled
// Determine if an element is currently enabled.
- (AppiumMacHTTPJSONResponse *)get_element_enabled:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/enabled",session.sessionId,element.AXIdentifier);
        
        return [AppiumMacHTTPJSONResponse responseWithJson:element.AXEnabled status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/element/:id/attribute/:name
// Get the value of an element's attribute.
- (AppiumMacHTTPJSONResponse *)get_element_attribute:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil
                                    onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        
        NSString *attributeName = [Utility getItemFromPath:path withSeparator:@"/attribute/"];
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/attribute/:%@",session.sessionId,element.AXIdentifier,attributeName);
        id attributeValue = [element valueForAttribute:attributeName];
        
        // The original AfM returned "(null)" for a nil (non-existent) attribute.
        // This was changed to meet the standard, which is the empty string, "".
        
        if (attributeValue == nil) {
            attributeValue = @"";
        }
        return [AppiumMacHTTPJSONResponse responseWithJson:[NSString stringWithFormat:@"%@", attributeValue] status:0 session:session.sessionId];
    }];
}

// GET /session/:sessionId/element/:id/equals/:other
// Test if two element IDs refer to the same DOM element.
- (AppiumMacHTTPJSONResponse *)get_element_equals:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        // The first element was already validated.
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        
        // get the second element
        NSString *otherElementId = [Utility getItemFromPath:path withSeparator:@"/equals/"];
        PFUIElement *otherElement = [session.elements objectForKey:otherElementId];
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/equals/:%@",session.sessionId,element.AXIdentifier,otherElement.AXIdentifier);
        // check that the second element is valid
        int status2 = [session checkElement:otherElement];
        if (status2 != kAfMStatusCodeSuccess) {
            [AppiumMacHTTPJSONResponse responseWithJsonError:status2 session:session.sessionId];
        }
        
        return [AppiumMacHTTPJSONResponse responseWithJson:[NSNumber numberWithBool:[element isEqualToElement:otherElement]] status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/element/:id/displayed
// Determine if an element is currently displayed.
- (AppiumMacHTTPJSONResponse *)get_element_displayed:(NSString*)path
{
    // Can't use executeWebDriverCommandWithPath here, because it filters out elements that are not visible.
    
    if (![NSThread isMainThread]) {
        [self performSelectorOnMainThread:_cmd withObject:path waitUntilDone:YES];
    }
    
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    if (!session) {
        return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchDriver session:session.sessionId];
    }
    
    BOOL displayed;
    
    // get the element
    NSString *elementId = [Utility getElementIDFromPath:path];
    PFUIElement *element = [session.elements objectForKey:elementId];
    DDLogCInfo(@"Request:GET /session/:%@/element/:%@/displayed",session.sessionId,element.AXIdentifier);
    // check the element is valid
    int status = [session checkElement:element];
    if (status == kAfMStatusCodeSuccess) {
        // By definition, usable elements are displayed.
        displayed = YES;
    } else if (status == kAfMStatusCodeElementNotVisible) {
        displayed = NO;
    } else {
        // The element was not valid and its visible state is undefined.
        return [AppiumMacHTTPJSONResponse responseWithJsonError:status session:sessionId];
    }
    return [AppiumMacHTTPJSONResponse responseWithJson:[NSNumber numberWithBool:displayed] status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/element/:id/location
// Determine an element's location on the page. The point (0, 0) refers to the upper-left corner of the page. The element's coordinates are returned as a JSON object with x and y properties.
- (AppiumMacHTTPJSONResponse *)get_element_location:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/location",session.sessionId,element.AXIdentifier);
        NSPoint point = [element.AXPosition pointValue];
        NSDictionary *position = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:point.x], @"x", [NSNumber numberWithFloat:point.y], @"y", nil];
        return [AppiumMacHTTPJSONResponse responseWithJson:position status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/element/:id/location_in_view
// Determine an element's location on the screen once it has been scrolled into view.

// GET /session/:sessionId/element/:id/size
// Determine an element's size in pixels. The size will be returned as a JSON object with width and height properties.
- (AppiumMacHTTPJSONResponse *)get_element_size:(NSString*)path
{
    return [self executeWebDriverCommandWithPath:path data:nil onMainThread:YES commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        PFUIElement *element = [commandParams objectForKey:@"elementObject"];
        DDLogCInfo(@"Request:GET /session/:%@/element/:%@/location_in_view",session.sessionId,element.AXIdentifier);
        NSSize size = [element.AXSize sizeValue];
        NSDictionary *sizeDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:size.width], @"width", [NSNumber numberWithFloat:size.height], @"height", nil];
        return [AppiumMacHTTPJSONResponse responseWithJson:sizeDict status:kAfMStatusCodeSuccess session:session.sessionId];
    }];
}

// GET /session/:sessionId/element/:id/css/:propertyName
// Query the value of an element's computed CSS property. 

// GET /session/:sessionId/orientation
// Get the current browser orientation. The server should return a valid orientation value as defined in ScreenOrientation: {LANDSCAPE|PORTRAIT}.

// POST /session/:sessionId/orientation
// Set the browser orientation. The orientation should be specified as defined in ScreenOrientation: {LANDSCAPE|PORTRAIT}.

// GET /session/:sessionId/alert_text
// Gets the text of the currently displayed JavaScript alert(), confirm(), or prompt() dialog.

// POST /session/:sessionId/alert_text
// Sends keystrokes to a JavaScript prompt() dialog.

// POST /session/:sessionId/accept_alert
// Accepts the currently displayed alert dialog. Usually, this is equivalent to clicking on the 'OK' button in the dialog.

// POST /session/:sessionId/dismiss_alert
// Dismisses the currently displayed alert dialog. For confirm() and prompt() dialogs, this is equivalent to clicking the 'Cancel' button. For alert() dialogs, this is equivalent to clicking the 'OK' button.

// POST /session/:sessionId/moveto
// Move the mouse by an offset of the specificed element. If no element is specified, the move is relative to the current mouse cursor. If an element is provided but no offset, the mouse will be moved to the center of the element. If the element is not visible, it will be scrolled into view.
- (AppiumMacHTTPJSONResponse *)post_moveto:(NSString*)path data:(NSData *)postData;
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/moveto",session.sessionId);
        if ([Utility isRunningInSandbox]) {
            return [session moveMouseInsideSandbox:commandParams statusCode:statusCode];
        } else {
            return [session moveMouseOutsideSandbox:commandParams statusCode:statusCode];
        }
    }];
}

// POST /session/:sessionId/click
// Click any mouse button (at the coordinates set by the last moveto command). Note that calling this command after calling buttondown and before calling button up (or any out-of-order interactions sequence) will yield undefined behaviour).
- (AppiumMacHTTPJSONResponse *)post_click:(NSString*)path data:(NSData *)postData
{
    
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/click",session.sessionId);
        if ([Utility isRunningInSandbox]) {
            return [session mouseButtonActionInsideSandbox:@"click" commandParams:commandParams statusCode:statusCode];
        } else {
            return [session mouseButtonActionOutsideSandbox:@"click" commandParams:commandParams statusCode:statusCode];
        }
    }];
}

// POST /session/:sessionId/buttondown
// Click and hold the left mouse button (at the coordinates set by the last moveto command). Note that the next mouse-related command that should follow is buttonup . Any other mouse command (such as click or another call to buttondown) will yield undefined behaviour.
- (AppiumMacHTTPJSONResponse *)post_buttondown:(NSString*)path data:(NSData *)postData
{
    
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/buttondown",session.sessionId);
        if ([Utility isRunningInSandbox]) {
            return [session mouseButtonActionInsideSandbox:@"buttondown" commandParams:commandParams statusCode:statusCode];
        } else {
            return [session mouseButtonActionOutsideSandbox:@"buttondown" commandParams:commandParams statusCode:statusCode];
        }
    }];
}

// POST /session/:sessionId/buttonup
// Releases the mouse button previously held (where the mouse is currently at). Must be called once for every buttondown command issued. See the note in click and buttondown about implications of out-of-order commands.
- (AppiumMacHTTPJSONResponse *)post_buttonup:(NSString*)path data:(NSData *)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/buttonup",session.sessionId);
        if ([Utility isRunningInSandbox]) {
            return [session mouseButtonActionInsideSandbox:@"buttonup" commandParams:commandParams statusCode:statusCode];
        } else {
            return [session mouseButtonActionOutsideSandbox:@"buttonup" commandParams:commandParams statusCode:statusCode];
        }
    }];
}

// POST /session/:sessionId/doubleclick
// Double-clicks at the current mouse coordinates (set by moveto).
- (AppiumMacHTTPJSONResponse *)post_doubleclick:(NSString*)path data:(NSData *)postData
{
    return [self executeWebDriverCommandWithPath:path data:postData onMainThread:NO commandBlock:^(AfMSessionController *session, NSDictionary *commandParams, int *statusCode)
    {
        DDLogCInfo(@"Request:POST /session/:%@/doubleclick",session.sessionId);
        if ([Utility isRunningInSandbox]) {
            return [session mouseButtonActionInsideSandbox:@"doubleclick" commandParams:commandParams statusCode:statusCode];
        } else {
            return [session mouseButtonActionOutsideSandbox:@"doubleclick" commandParams:commandParams statusCode:statusCode];
        }
    }];
}

// POST /session/:sessionId/touch/click
// Single tap on the touch enabled device.

// POST /session/:sessionId/touch/down
// Finger down on the screen.

// POST /session/:sessionId/touch/up
// Finger up on the screen.

// POST /session/:sessionId/touch/move
// Finger move on the screen.

// POST /session/:sessionId/touch/scroll
// Scroll on the touch screen using finger based motion events. Use this command to start scrolling at a particular screen location.
// (The difference between this and the next command is an extra JSON paremeter for this command:
//      element - {string} ID of the element where the scroll starts.)

// POST /session/:sessionId/touch/scroll
// Scroll on the touch screen using finger based motion events. Use this command if you don't care where the scroll starts on the screen.

// POST /session/:sessionId/touch/doubleclick
// Double tap on the touch screen using finger motion events.

// POST /session/:sessionId/touch/longclick
// Long press on the touch screen using finger motion events.

// POST /session/:sessionId/touch/flick
// Flick on the touch screen using finger motion events. This flickcommand starts at a particulat screen location.

// POST /session/:sessionId/touch/flick
// Flick on the touch screen using finger motion events. Use this flick command if you don't care where the flick starts on the screen.

// GET /session/:sessionId/location
// Get the current geo location.

// POST /session/:sessionId/location
// Set the current geo location.

// GET /session/:sessionId/local_storage
// Get all keys of the storage.

// POST /session/:sessionId/local_storage
// Set the storage item for the given key.

// DELETE /session/:sessionId/local_storage
// Clear the storage.

// GET /session/:sessionId/local_storage/key/:key
// Get the storage item for the given key.

// DELETE /session/:sessionId/local_storage/key/:key
// Remove the storage item for the given key.

// GET /session/:sessionId/local_storage/size
// Get the number of items in the storage.

// GET /session/:sessionId/session_storage
// Get all keys of the storage.

// POST /session/:sessionId/session_storage
// Set the storage item for the given key.

// DELETE /session/:sessionId/session_storage
// Clear the storage.

// GET /session/:sessionId/session_storage/key/:key
// Get the storage item for the given key.

// DELETE /session/:sessionId/session_storage/key/:key
// Remove the storage item for the given key.

// GET /session/:sessionId/session_storage/size
// Get the number of items in the storage.

// POST /session/:sessionId/log
// Get the log for a given log type. Log buffer is reset after each request.

// GET /session/:sessionId/log/types
// Get available log types.

// GET /session/:sessionId/application_cache/status
// Get the status of the html5 application cache.


// This method is called from AfMHTTPConnection for each json wire protocol command.
- (AppiumMacHTTPJSONResponse *)httpResponseForMethod:(NSString *)method URI:(NSString *)path data:(NSData*)postData
{
    path = [path stringByReplacingOccurrencesOfString:@"/wd/hub/" withString:@"/"];
    NSArray *pathComponents = [[path substringFromIndex:1] componentsSeparatedByString:@"/"];
    
//    NSLog(@"method: %@", method);
//    NSLog(@"path: %@", path);
//    if ([method isEqualToString:@"POST"]) {
//        NSDictionary *postParams = [Utility dictionaryFromPostData:postData];
//        NSLog(@"postParams: %@", postParams);
//    }
    
    NSString *errorString = nil;
    
    // Construct a selector from the http path. If self responds to the selector,
    // invoke the selector and return the value.
    SEL selector = [self selectorForMethod:method andPathComponents:pathComponents];
    if (selector) {
        NSMethodSignature *signature = [self methodSignatureForSelector:selector];
        if (signature) {
            NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:signature];
            invocation.selector = selector;
            [invocation setArgument:&path atIndex:2];
            if (postData) {
                [invocation setArgument:&postData atIndex:3];
            }
            
            [invocation invokeWithTarget:self];
            
            AppiumMacHTTPJSONResponse * __unsafe_unretained tempVar;
            [invocation getReturnValue:&tempVar];
            
            return tempVar;
        } else {
            errorString = [NSString stringWithFormat:@"Could not find selector (%@) for path '%@'.", NSStringFromSelector(selector), path];
        }
    } else {
        errorString = [NSString stringWithFormat:@"Could not create a selector for path '%@'.", path];
    }
    
    // We didn't find a method to handle this request. Report the bad news.
//    NSLog(@"%@", errorString);
    NSString *sessionId = nil;
    if (pathComponents.count >= 2 ) {
        sessionId = pathComponents[1];
    }
    NSDictionary *errorJson = @{@"message":errorString};
    return [AppiumMacHTTPJSONResponse responseWithJson:errorJson status:kAfMStatusCodeUnknownCommand session:sessionId];
}

// Automatically create a selector out of the request. 
- (SEL)selectorForMethod:(NSString *)method andPathComponents:(NSArray *)pathComponents
{
    NSUInteger pathComponentCount = pathComponents.count;
    NSMutableString *selectorString = [NSMutableString string];
    
    if ([method isEqualToString:@"GET"]) {
        [selectorString appendString:@"get_"];
        switch (pathComponentCount) {
            case 1:
            case 2:
                // /status, /settings, or /session/:sessionId
                [selectorString appendFormat:@"%@:", pathComponents[0]];
                break;
            case 3:
                // e.g. /session/:sessionId/window_handle
                [selectorString appendFormat:@"%@:", pathComponents[2]];
                break;
            case 4:
                if ([pathComponents[2] isEqualToString:@"element"]) {
                    // Special case: /session/:sessionId/element/:id  ends with an id
                    [selectorString appendFormat:@"%@:", pathComponents[2]];
                } else {
                    // e.g. /session/:sessionId/local_storage/size
                    [selectorString appendFormat:@"%@_%@:", pathComponents[2], pathComponents[3]];
                }
                break;
            case 5:
                if ([@[@"local_storage", @"session_storage"] containsObject: pathComponents[2]]) {
                    // Special case: /session/:sessionId/local_storage/key/:key  ends with an id
                    // Special case: /session/:sessionId/session_storage/key/:key  ends with an id
                    [selectorString appendFormat:@"%@_%@:", pathComponents[2], pathComponents[3]];
                } else {
                    // e.g. /session/:sessionId/element/:id/enabled
                    [selectorString appendFormat:@"%@_%@:", pathComponents[2], pathComponents[4]];
                }
                break;
            case 6:
                // e.g. /session/:sessionId/element/:id/attribute/:name
                [selectorString appendFormat:@"%@_%@:", pathComponents[2], pathComponents[4]];
                break;
        }
    } else if ([method isEqualToString:@"POST"]) {
        [selectorString appendString:@"post_"];
        switch (pathComponentCount) {
            case 1:
                // /session
                [selectorString appendFormat:@"%@:", pathComponents[0]];
                break;
            case 3:
                // e.g.. /session/:sessionId/timeouts
                [selectorString appendFormat:@"%@:", pathComponents[2]];
                break;
            case 4:
                // e.g.. /session/:sessionId/timeouts/async_script
                [selectorString appendFormat:@"%@_%@:", pathComponents[2], pathComponents[3]];
                break;
            case 5:
                // e.g. /session/:sessionId/window/:windowHandle/size
                // e.g. /session/:sessionId/element/:id/element
                [selectorString appendFormat:@"%@_%@:", pathComponents[2], pathComponents[4]];
                break;
        }
        // All post methods have a data: argument whether it is used or not inside the method. 
        // E.g. post_element_click does not have postData, because it gets the element Id from the path.
        [selectorString appendFormat:@"data:"];
        
    } else if ([method isEqualToString:@"DELETE"]) {
        [selectorString appendString:@"delete_"];
        switch (pathComponentCount) {
            case 2:
                // /session/:sessionId
                [selectorString appendFormat:@"%@:", pathComponents[0]];
                break;
            case 3:
                // e.g. /session/:sessionId/cookie
                [selectorString appendFormat:@"%@:", pathComponents[2]];
                break;
            case 4:
                // There's only one: /session/:sessionId/cookie/:name
                // (This results in the same selector as /session/:sessionId/cookie, 
                // but it will be invoked with a different path argument.)
                [selectorString appendFormat:@"%@:", pathComponents[2]];
                break;
            case 5:
                // e.g. /session/:sessionId/local_storage/key/:key ends with a key
                // e.g. /session/:sessionId/session_storage/key/:key ends with a key
                [selectorString appendFormat:@"%@_%@:", pathComponents[2], pathComponents[3]];
                break;
        }
        
    }
    if (selectorString.length > 0) {
        return NSSelectorFromString(selectorString);
    }
    return 0;
}

@end
