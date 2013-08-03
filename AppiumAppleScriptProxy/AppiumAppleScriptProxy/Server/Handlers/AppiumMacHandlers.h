//
//  AppiumMacController.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AppiumMacHTTPJSONResponse.h"
#import "AppiumMacSessionController.h"
#import "SystemEvents.h"

@interface AppiumMacHandlers : NSObject

@property NSMutableDictionary *sessions;

// GET /status
-(AppiumMacHTTPJSONResponse*) getStatus:(NSString*)path;

// POST /session
-(AppiumMacHTTPJSONResponse*) postSession:(NSString*)path data:(NSData*)postData;

// GET /sessions
-(HTTPDataResponse*) getSessions:(NSString*)path;

// GET /session/:sessionId
-(AppiumMacHTTPJSONResponse*) getSession:(NSString*)path;

// DELETE /session/:sessionId
-(AppiumMacHTTPJSONResponse*) deleteSession:(NSString*)path;

// /session/:sessionId/timeouts
// /session/:sessionId/timeouts/async_script
// /session/:sessionId/timeouts/implicit_wait

// GET /session/:sessionId/window_handle
-(AppiumMacHTTPJSONResponse*) getWindowHandle:(NSString*)path;

// GET /session/:sessionId/window_handles
-(AppiumMacHTTPJSONResponse*) getWindowHandles:(NSString*)path;

// GET /session/:sessionId/url
-(AppiumMacHTTPJSONResponse*) getUrl:(NSString*)path;

// POST /session/:sessionId/url
-(AppiumMacHTTPJSONResponse*) postUrl:(NSString*)path data:(NSData*)postData;

// /session/:sessionId/forward
// /session/:sessionId/back
// /session/:sessionId/refresh
// /session/:sessionId/execute
// /session/:sessionId/execute_async

// GET /session/:sessionId/screenshot
-(AppiumMacHTTPJSONResponse*) getScreenshot:(NSString*)path;

// /session/:sessionId/ime/available_engines
// /session/:sessionId/ime/active_engine
// /session/:sessionId/ime/activated
// /session/:sessionId/ime/deactivate
// /session/:sessionId/ime/activate
// /session/:sessionId/frame

// GET /session/:sessionId/window
-(AppiumMacHTTPJSONResponse*) postWindow:(NSString*)path data:(NSData*)postData;

// DELETE /session/:sessionId/window
-(AppiumMacHTTPJSONResponse*) deleteWindow:(NSString *)path;

// /session/:sessionId/window/:windowHandle/size
// /session/:sessionId/window/:windowHandle/position
// /session/:sessionId/window/:windowHandle/maximize
// /session/:sessionId/cookie
// /session/:sessionId/cookie/:name

// GET /session/:sessionId/source
-(AppiumMacHTTPJSONResponse*) getSource:(NSString*)path;

// /session/:sessionId/title

// POST /session/:sessionId/element
-(AppiumMacHTTPJSONResponse*) postElement:(NSString*)path data:(NSData*)postData;

// /session/:sessionId/elements
// /session/:sessionId/element/active
// /session/:sessionId/element/:id

// POST /session/:sessionId/element/:id/element
-(AppiumMacHTTPJSONResponse*) postElementInElement:(NSString*)path data:(NSData*)postData;

// /session/:sessionId/element/:id/elements

// POST /session/:sessionId/element/:id/click
-(AppiumMacHTTPJSONResponse*) postElementClick:(NSString*)path;

// /session/:sessionId/element/:id/submit

// GET /session/:sessionId/element/:id/text
-(AppiumMacHTTPJSONResponse*) getElementText:(NSString*)path;

// POST /session/:sessionId/element/:id/value
-(AppiumMacHTTPJSONResponse*) postElementValue:(NSString*)path data:(NSData*)postData;


// POST /session/:sessionId/keys
-(AppiumMacHTTPJSONResponse*) postKeys:(NSString*)path data:(NSData*)postData;

// GET /session/:sessionId/element/:id/name
-(AppiumMacHTTPJSONResponse*) getElementName:(NSString*)path;

// POST /session/:sessionId/element/:id/clear
-(AppiumMacHTTPJSONResponse*) postElementClear:(NSString*)path;

// GET /session/:sessionId/element/:id/selected
-(AppiumMacHTTPJSONResponse*) getElementIsSelected:(NSString*)path;

// GET /session/:sessionId/element/:id/enabled
-(AppiumMacHTTPJSONResponse*) getElementIsEnabled:(NSString*)path;

// GET /session/:sessionId/element/:id/attribute/:name
-(AppiumMacHTTPJSONResponse*) getElementAttribute:(NSString*)path;

// GET /session/:sessionId/element/:id/equals/:other
-(AppiumMacHTTPJSONResponse*) getElementIsEqual:(NSString*)path;

// /session/:sessionId/element/:id/displayed

// GET /session/:sessionId/element/:id/location
-(AppiumMacHTTPJSONResponse*) getElementLocation:(NSString*)path;

// /session/:sessionId/element/:id/location_in_view

// GET /session/:sessionId/element/:id/size
-(AppiumMacHTTPJSONResponse*) getElementSize:(NSString*)path;

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
