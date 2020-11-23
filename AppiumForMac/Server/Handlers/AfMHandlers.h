//
//  AfMHandlers.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Improvements by Stuart Russell at Intuit.
//  Copyright (c) 2013-2016 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

@class AppiumMacHTTPJSONResponse;

@interface AfMHandlers : NSObject

@property NSMutableDictionary *sessions;

// GET /status
- (AppiumMacHTTPJSONResponse *)get_status:(NSString*)path;

// POST /session
- (AppiumMacHTTPJSONResponse *)post_session:(NSString*)path data:(NSData*)postData;

// GET /sessions
- (AppiumMacHTTPJSONResponse *)get_sessions:(NSString*)path;

// GET /session/:sessionId
- (AppiumMacHTTPJSONResponse *)get_session:(NSString*)path;

// DELETE /session/:sessionId
- (AppiumMacHTTPJSONResponse *)delete_session:(NSString*)path;

// POST /session/:sessionId/timeouts
- (AppiumMacHTTPJSONResponse *)post_timeouts:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/timeouts/async_script
- (AppiumMacHTTPJSONResponse *)post_timeouts_async_script:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/timeouts/implicit_wait
- (AppiumMacHTTPJSONResponse *)post_timeouts_implicit_wait:(NSString*)path data:(NSData*)postData;

// GET /session/:sessionId/window_handle
- (AppiumMacHTTPJSONResponse *)get_window_handle:(NSString*)path;

// GET /session/:sessionId/window_handles
- (AppiumMacHTTPJSONResponse *)get_window_handles:(NSString*)path;

// GET /session/:sessionId/url
- (AppiumMacHTTPJSONResponse *)get_url:(NSString*)path;

// POST /session/:sessionId/url
- (AppiumMacHTTPJSONResponse *)post_url:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/forward
// POST /session/:sessionId/back
// POST /session/:sessionId/refresh
// POST /session/:sessionId/execute
// POST /session/:sessionId/execute_async

// GET /session/:sessionId/screenshot
- (AppiumMacHTTPJSONResponse *)get_screenshot:(NSString*)path;

// GET /session/:sessionId/ime/available_engines
// GET /session/:sessionId/ime/active_engine
// GET /session/:sessionId/ime/activated
// POST /session/:sessionId/ime/deactivate
// POST /session/:sessionId/ime/activate

// POST /session/:sessionId/window
- (AppiumMacHTTPJSONResponse *)post_window:(NSString*)path data:(NSData*)postData;

// DELETE /session/:sessionId/window
- (AppiumMacHTTPJSONResponse *)delete_window:(NSString *)path;

// POST /session/:sessionId/window/:windowHandle/size
- (AppiumMacHTTPJSONResponse *)post_window_size:(NSString*)path data:(NSData*)postData;

// GET /session/:sessionId/window/:windowHandle/size
- (AppiumMacHTTPJSONResponse *)get_window_size:(NSString*)path;

// POST /session/:sessionId/window/:windowHandle/position
- (AppiumMacHTTPJSONResponse *)post_window_position:(NSString*)path data:(NSData*)postData;

// GET /session/:sessionId/window/:windowHandle/position
- (AppiumMacHTTPJSONResponse *)get_window_position:(NSString*)path;

// POST /session/:sessionId/window/:windowHandle/maximize

// GET /session/:sessionId/cookie
- (AppiumMacHTTPJSONResponse *)get_cookie:(NSString*)path;

// POST /session/:sessionId/cookie
- (AppiumMacHTTPJSONResponse *)post_cookie:(NSString*)path data:(NSData*)postData;

// DELETE /session/:sessionId/cookie
// DELETE /session/:sessionId/cookie/:name

// GET /session/:sessionId/source
- (AppiumMacHTTPJSONResponse *)get_source:(NSString*)path;

// POST /session/:sessionId/element
- (AppiumMacHTTPJSONResponse *)post_element:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/elements
- (AppiumMacHTTPJSONResponse *)post_elements:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/element/active
// GET /session/:sessionId/element/:id

// POST /session/:sessionId/element/:id/element
- (AppiumMacHTTPJSONResponse *)post_element_element:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/element/:id/elements
- (AppiumMacHTTPJSONResponse *)post_element_elements:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/element/:id/click
- (AppiumMacHTTPJSONResponse *)post_element_click:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/element/:id/submit

// GET /session/:sessionId/element/:id/text
- (AppiumMacHTTPJSONResponse *)get_element_text:(NSString*)path;

// POST /session/:sessionId/element/:id/value
- (AppiumMacHTTPJSONResponse *)post_element_value:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/keys
- (AppiumMacHTTPJSONResponse *)post_keys:(NSString*)path data:(NSData*)postData;

// GET /session/:sessionId/element/:id/name
- (AppiumMacHTTPJSONResponse *)get_element_name:(NSString*)path;

// POST /session/:sessionId/element/:id/clear
- (AppiumMacHTTPJSONResponse *)post_element_clear:(NSString*)path data:(NSData*)postData;

// GET /session/:sessionId/element/:id/selected
- (AppiumMacHTTPJSONResponse *)get_element_selected:(NSString*)path;

// GET /session/:sessionId/element/:id/enabled
- (AppiumMacHTTPJSONResponse *)get_element_enabled:(NSString*)path;

// GET /session/:sessionId/element/:id/attribute/:name
- (AppiumMacHTTPJSONResponse *)get_element_attribute:(NSString*)path;

// GET /session/:sessionId/element/:id/equals/:other
- (AppiumMacHTTPJSONResponse *)get_element_equals:(NSString*)path;

// GET /session/:sessionId/element/:id/displayed
- (AppiumMacHTTPJSONResponse *)get_element_displayed:(NSString*)path;

// GET /session/:sessionId/element/:id/location
- (AppiumMacHTTPJSONResponse *)get_element_location:(NSString*)path;

// GET /session/:sessionId/element/:id/location_in_view

// GET /session/:sessionId/element/:id/size
- (AppiumMacHTTPJSONResponse *)get_element_size:(NSString*)path;

// GET /session/:sessionId/element/:id/css/:propertyName
// GET /session/:sessionId/orientation
// POST /session/:sessionId/orientation
// GET /session/:sessionId/alert_text
// POST /session/:sessionId/alert_text
// POST /session/:sessionId/accept_alert
// POST /session/:sessionId/dismiss_alert

// POST /session/:sessionId/moveto
-(AppiumMacHTTPJSONResponse *)post_moveto:(NSString*)path data:(NSData *)postData;

// POST /session/:sessionId/click
- (AppiumMacHTTPJSONResponse *)post_click:(NSString*)path data:(NSData *)postData;

// POST /session/:sessionId/buttondown
- (AppiumMacHTTPJSONResponse *)post_buttondown:(NSString*)path data:(NSData *)postData;

// POST /session/:sessionId/buttonup
- (AppiumMacHTTPJSONResponse *)post_buttonup:(NSString*)path data:(NSData *)postData;

// POST /session/:sessionId/doubleclick
- (AppiumMacHTTPJSONResponse *)post_doubleclick:(NSString*)path data:(NSData *)postData;

// POST /session/:sessionId/element/:id/scrollTo
- (AppiumMacHTTPJSONResponse *)post_element_scrollTo:(NSString*)path data:(NSData*)postData;

// POST /session/:sessionId/touch/click
// POST /session/:sessionId/touch/down
// POST /session/:sessionId/touch/up
// POST /session/:sessionId/touch/move
// POST /session/:sessionId/touch/scroll
// POST /session/:sessionId/touch/scroll
// POST /session/:sessionId/touch/doubleclick
// POST /session/:sessionId/touch/longclick
// POST /session/:sessionId/touch/flick
// POST /session/:sessionId/touch/flick
// GET /session/:sessionId/location
// POST /session/:sessionId/location
// GET /session/:sessionId/local_storage
// POST /session/:sessionId/local_storage
// DELETE /session/:sessionId/local_storage
// GET /session/:sessionId/local_storage/key/:key
// DELETE /session/:sessionId/local_storage/key/:key
// GET /session/:sessionId/local_storage/size
// GET /session/:sessionId/session_storage
// POST /session/:sessionId/session_storage
// DELETE /session/:sessionId/session_storage
// GET /session/:sessionId/session_storage/key/:key
// DELETE /session/:sessionId/session_storage/key/:key
// GET /session/:sessionId/session_storage/size
// POST /session/:sessionId/log
// GET /session/:sessionId/log/types
// GET /session/:sessionId/application_cache/status

- (AppiumMacHTTPJSONResponse *)httpResponseForMethod:(NSString *)method URI:(NSString *)path data:(NSData*)postData;
- (SEL)selectorForMethod:(NSString *)method andPathComponents:(NSArray *)pathComponents;

@end

