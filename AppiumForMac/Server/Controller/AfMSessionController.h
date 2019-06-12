//
//  AfMSessionController.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Improvements by Stuart Russell at Intuit, Inc.
//  Copyright (c) 2013-2016 Appium. All rights reserved.
//

#import <Quartz/Quartz.h>

#import "Finder.h"
#import "SystemEvents.h"
#import "GDataXMLNode.h"
#import "AfMStatusCodes.h"
#import "AppiumMacHTTPJSONResponse.h"

typedef NS_ENUM(NSUInteger, WebDriverMouseButtonType) {
    webDriverLeftButton		= 0,
    webDriverMiddleButton	= 1,
    webDriverRightButton	= 2
};

typedef NS_ENUM(NSUInteger, PageLoadStrategy) {
    pageLoadStrategyNone	= 0,
    pageLoadStrategyEager	= 1,
    pageLoadStrategyNormal	= 2
};

typedef NS_ENUM(NSUInteger, ElementPointLocation) {
    elementPointCenter      = 0,
    elementPointUpperLeft   = 1
};

// If not specified by desiredCapability, use this value in seconds.
#define kDefaultImplicitTimeout (NSTimeInterval)0.0f

// If not specified by desiredCapability, use this value in seconds.
#define kDefaultPageLoadTimeout (NSTimeInterval)30.0f

// If not specified by desiredCapability, use this value in seconds.
#define kDefaultScriptTimeout (NSTimeInterval)0.0f

// If not specified by desiredCapability, use this value in seconds.
#define kDefaultLoopDelay (NSTimeInterval)1.0f

// If not specified by desiredCapability, use this value in seconds.
#define kDefaultCommandDelay (NSTimeInterval)0.0f

// How long to wait between changes in mouse up/down state.
#define kMouseButtonEventDelay (NSTimeInterval)0.3f

// How long to wait between kCGEventMouseMoved events.
#define kMouseMoveEventDelay (NSTimeInterval)0.01f

// If not specified by desiredCapability, use this value in points per second.
#define kDefaultMouseMoveSpeed (float)100.0

// https://w3c.github.io/webdriver/webdriver-spec.html#new-session
// A Selenium endpoint node (i.e. this application) can have at most 1 session.
// --- Theory vs Practice: In practice, clients often create sessions and only close some of them. 
// --- E.g. During Selenium script development, when the script is stopped while running.
// --- Therefore, allow multiple sessions.
#define kMaximumActiveSessions 1000

extern NSString * const kCookieLoopDelay;
extern NSString * const kCookieImplicitTimeout;
extern NSString * const kCookieCommandDelay;
extern NSString * const kCookiePageLoadTimeout;
extern NSString * const kCookieScriptTimeout;
extern NSString * const kCookieMouseSpeed;
extern NSString * const kCookieScreenShotOnError;
extern NSString * const kCookieGlobalDiagnosticsDirectory;
extern NSString * const kCookieDiagnosticsDirectory;

@class PFApplicationUIElement;
@class PFUIElement;

@interface AfMSessionController : NSObject

@property NSString *sessionId;
@property NSMutableDictionary *elements;
@property int elementIndex;

@property (readonly) NSArray* allProcesses;
@property (readonly) NSArray* allWindows;
@property (readonly) NSArray* allWindowHandles;
@property NSDictionary *capabilities;
@property PFApplicationUIElement *currentApplication;
@property NSString *currentApplicationName;
@property NSString *currentProcessName;
@property (readonly) PFUIElement *currentWindow;
@property NSString *currentWindowHandle;
@property (readonly) SystemEventsProcess *currentProcess;
@property FinderApplication *finder;
@property SystemEventsApplication *systemEvents;
@property PageLoadStrategy pageLoadingStrategy;

// Timeouts and delays can be changed using a DesiredCapabilities instance in the Selenium script.

// How long to wait between retry attempts, during an implicit timeout.
@property (nonatomic) NSTimeInterval loopDelay;

// This slows down the script a small amount per command. Using this is sub-optimal for high-performance scripts.
// Instead, use findElement and other commands that automatically wait for something to happen.
@property (nonatomic) NSTimeInterval commandDelay;

// The WebDriver implicit wait timeout in seconds.
@property (nonatomic) NSTimeInterval implicitTimeout;

// The WebDriver page load timeout in seconds.
@property (nonatomic) NSTimeInterval pageLoadTimeout;

// The WebDriver asynchronous script timeout in seconds.
@property (nonatomic) NSTimeInterval scriptTimeout;

// Take a screen shot when an element is not found. Save in the diagnosticsDirectory.
@property (nonatomic) BOOL shouldTakeScreenShot;

// The top level directory containing diagnostic directories for _all_ sessions.
@property (nonatomic, readwrite, strong) NSString *globalDiagnosticsDirectory;

// The directory for _this_ session's diagnostic information, e.g. screen shots, logs.
@property (nonatomic, readwrite, strong) NSString *diagnosticsDirectory;

// The mouse speed in points per second.
@property (nonatomic, assign) float mouseMoveSpeed;


// Set this to YES to cancel a timeout loop. This is only a flag. The timeout loop has to check the value.
@property (assign) BOOL isCanceled;
// The last known global screen mouse location, for calculating offsets.
@property (assign) CGPoint lastGlobalMouseLocation;

// Does this session support nativeEvents?
@property (assign) BOOL nativeEventSupport;

- (BOOL)finishSetup;
-(void) activateApplication;
-(BOOL) activateWindow;
//-(NSString*) applicationNameForProcessName:(NSString*)processName;
- (int) checkElement:(id)element;
- (BOOL)isElementDisplayed:(id)element;
-(BOOL) clickElement:(id)element;
-(void) closeWindow;
-(NSDictionary *)executeShellScript:(NSString *)script;
-(NSDictionary*) pageSource;
-(NSInteger) pidForProcessName:(NSString*)processName;
-(SystemEventsProcess*) processForName:(NSString*)processName;
-(NSString*) processNameForApplicationName:(NSString*) applicationName;
- (AppiumMacHTTPJSONResponse *)postURL:(NSString *)url;
- (void)sendKeys:(NSArray*)keys toElement:(id)element;
- (void)sendKeys:(NSArray*)keys;
-(PFUIElement*) windowForHandle:(NSString*)windowHandle;
-(GDataXMLDocument*)xmlPageSource;
-(GDataXMLDocument*)xmlPageSourceFromRootUIElement:(PFUIElement*)rootUIElement pathMap:(NSMutableDictionary*)pathMap xPath:(NSString*)xPath;
- (NSArray *)findAllUsingAbsoluteAXPath:(NSString *)path;

- (AppiumMacHTTPJSONResponse *)executeWebDriverCommandWithPath:(NSString*)path data:(NSData*)postData onMainThread:(BOOL)onMainThread commandBlock:(AppiumMacHTTPJSONResponse *(^)(AfMSessionController *session, NSDictionary *commandParams, int *statusCode))commandBlock;

- (void)setDesiredCapabilities:(NSDictionary *)desiredCapabilities;

// Return YES if the current frame is ready according to the session pageLoadingStrategy.
- (BOOL)isPageLoaded;

// Utility methods for post_moveto:
- (AppiumMacHTTPJSONResponse *)moveMouseInsideSandbox:(NSDictionary *)commandParams statusCode:(int *)statusCode;
- (AppiumMacHTTPJSONResponse *)moveMouseOutsideSandbox:(NSDictionary *)commandParams statusCode:(int *)statusCode;
- (CGPoint)getGlobalDisplayPointWithCommandParams:(NSDictionary *)commandParams;
- (CGPoint)getGlobalDisplayPoint:(ElementPointLocation)whichPoint forElement:(id)element;

// Utility method for post_click:, post_buttondown:, post_buttonup:, post_doubleclick:.
- (AppiumMacHTTPJSONResponse *)mouseButtonActionInsideSandbox:(NSString *)commandName commandParams:(NSDictionary *)commandParams statusCode:(int *)statusCode;
- (AppiumMacHTTPJSONResponse *)mouseButtonActionOutsideSandbox:(NSString *)commandName commandParams:(NSDictionary *)commandParams statusCode:(int *)statusCode;

// If imageDirectory is nil, the image is saved in a subdirectory of diagnosticsDirectory.
- (void)screenCaptureToFile:(NSString *)fileName withDateStamp:(BOOL)appendDateStamp;

// Utility methods for get_screenshot, and for generic screen shots outside of a WebDriver command handler.
// Each method returns YES if the screen capture succeeded, else NO.
- (BOOL)screenCaptureToClipboard;

// Utilities to get and set a cookie. This examines the cookie name and determines
// if the value needs any special handling. If so it calls the special methods.
// If not, then just add the key/value dictionary.
- (NSArray *)getAllCookies;
- (NSMutableDictionary *)getCookieWithName:(NSString *)name;
- (void)setCookie:(NSDictionary *)cookie;

@end
