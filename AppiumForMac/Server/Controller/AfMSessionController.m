//
//  AfMSessionController.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Improvements by Stuart Russell at Intuit, Inc.
//  Copyright (c) 2013-2016 Appium. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <Quartz/Quartz.h>
#import <PFAssistive/PFAssistive.h>

#import "AfMSessionController.h"
#import "AppiumForMacAppDelegate.h"
#import "KeystrokesAndKeyCodes.h"
#import "Utility.h"

NSString * const kCookieLoopDelay = @"loop_delay";
NSString * const kCookieImplicitTimeout = @"implicit_timeout";
NSString * const kCookieCommandDelay = @"command_delay";
NSString * const kCookiePageLoadTimeout = @"page_load_timeout";
NSString * const kCookieScriptTimeout = @"script_timeout";
NSString * const kCookieMouseSpeed = @"mouse_speed";
NSString * const kCookieScreenShotOnError = @"screen_shot_on_error";
NSString * const kCookieGlobalDiagnosticsDirectory = @"global_diagnostics_directory";
NSString * const kCookieDiagnosticsDirectory = @"diagnostics_directory";

NSString * const kNodeRole = @"kNodeRole";
NSString * const kNodePredicate = @"kNodePredicate";
NSString * const kPredicateOperations = @"kPredicateOperations";
NSString * const kPredicateAttributeName = @"kPredicateAttributeName";
NSString * const kPredicateAttributeValue = @"kPredicateAttributeValue";
NSString * const kPredicateIndex = @"kPredicateIndex";
NSString * const kPredicateBooleanOperator = @"kPredicateBooleanOperator";
NSString * const kPredicateAND = @" and @";
NSString * const kPredicateOR = @" or @";
NSString * const kPredicateNone = @"None";
NSString * const kPredicateComparisonOperation = @"kPredicateComparisonOperation";
NSString * const kPredicateEQUALS = @"=";
NSString * const kPredicateNOTEQUALS = @"!=";
NSInteger const kPredicateLeftOperand = 0;
NSInteger const kPredicateRightOperand = 1;

@interface AfMSessionController()
@property NSString *_currentApplicationName;

@property BOOL isLeftMouseDown;
@property BOOL isRightMouseDown;
@property BOOL isOtherMouseDown;

@property BOOL isShiftKeyDown;
@property BOOL isControlKeyDown;
@property BOOL isAltKeyDown;
@property BOOL isCommandKeyDown;

// Properties for partial AXPath fix
@property NSString* elementRole;
@property NSMutableDictionary* predicateDict;

// Properties for handling timed events such as mouse moves.
@property NSMutableArray *timedEvents;
@property NSUInteger timedEventIndex;
@property NSTimer *timedEventTimer;
@property dispatch_semaphore_t timedEventsCompleted;

// Our cookies array. Each cookie is a mutable dictionary with minimum keys "name" and "value".
// Even though we don't support web views, this is a sneaky 
// workaround for a scripter to get and set webdriver properties like mouse_speed. 
@property (readwrite) NSMutableArray *afmCookies;

@property id processActivity; // to avoid App Nap
@end

@implementation AfMSessionController

- (id)init
{
    self = [super init];
    if (self) {
        // Generate a unique opaque session Id.
        self.sessionId = [Utility randomStringOfLength:8];
        
        [self setElementIndex:0];
        [self setElements:[NSMutableDictionary new]];
        [self setCurrentApplicationName:@"Finder"];
        [self setFinder:[SBApplication applicationWithBundleIdentifier:@"com.apple.finder"]];
        [self setSystemEvents:[SBApplication applicationWithBundleIdentifier:@"com.apple.systemevents"]];
		[self setCapabilities:[NSDictionary dictionaryWithObjectsAndKeys:
								[Utility version], @"version",
								[NSNumber numberWithBool:NO], @"webStorageEnabled",
								[NSNumber numberWithBool:NO], @"locationContextEnabled",
								@"Mac", @"browserName",
								@"Mac", @"platform",
								[NSNumber numberWithBool:YES], @"javascriptEnabled",
								[NSNumber numberWithBool:NO], @"databaseEnabled",
                               [NSNumber numberWithBool:YES], @"takesScreenshot",
                               [NSNumber numberWithBool:YES], @"nativeEvents",
								nil]];
        
        [self setPageLoadingStrategy:pageLoadStrategyNormal];
        
        // Non-element offsets start from the last location, or {0, 0}.
        self.lastGlobalMouseLocation = CGPointMake(0.0, 0.0);
        
        self.nativeEventSupport = YES;
        
        self.isLeftMouseDown = NO;
        self.isRightMouseDown = NO;
        self.isOtherMouseDown = NO;
        self.timedEvents = [NSMutableArray array];
        self.timedEventIndex = 0;
        
        self.afmCookies = [NSMutableArray array];

        [self setCookie:@{@"name": kCookieLoopDelay, @"value": [NSNumber numberWithFloat:kDefaultLoopDelay]}];
        [self setCookie:@{@"name": kCookieCommandDelay, @"value": [NSNumber numberWithFloat:kDefaultCommandDelay]}];
        [self setCookie:@{@"name": kCookieImplicitTimeout, @"value": [NSNumber numberWithFloat:kDefaultImplicitTimeout]}];
        [self setCookie:@{@"name": kCookiePageLoadTimeout, @"value": [NSNumber numberWithFloat:kDefaultPageLoadTimeout]}];
        [self setCookie:@{@"name": kCookieScriptTimeout, @"value": [NSNumber numberWithFloat:kDefaultScriptTimeout]}];
        [self setCookie:@{@"name": kCookieMouseSpeed, @"value": [NSNumber numberWithFloat:kDefaultMouseMoveSpeed]}];
        [self setCookie:@{@"name": kCookieScreenShotOnError, @"value": [NSNumber numberWithFloat:NO]}];
        [self setCookie:@{@"name": kCookieGlobalDiagnosticsDirectory, @"value": @""}];
        [self setCookie:@{@"name": kCookieDiagnosticsDirectory, @"value": @""}];

        // Make sure all modifiers are up.
        [self releaseAllModifiers];

        // Disable OS X "App Nap" while a session is in progress.
        NSString *activityDescription = [NSString stringWithFormat:@"AfMSessionController sessionId:%@", self.sessionId];
        // https://developer.apple.com/library/mac/documentation/Performance/Conceptual/power_efficiency_guidelines_osx/AppNap.html
        // https://developer.apple.com/library/mac/documentation/Performance/Conceptual/power_efficiency_guidelines_osx/PrioritizeWorkAtTheAppLevel.html#//apple_ref/doc/uid/TP40013929-CH36-SW3
        self.processActivity = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiated reason:activityDescription];
        
        self.isLeftMouseDown = NO;
        self.isRightMouseDown = NO;
        self.isOtherMouseDown = NO;
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(fnKeyTrigger:) name:AFMFnKeyTriggerNotification object:nil];
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    // When the session ends, we will make sure all modifiers are up.
    [self releaseAllModifiers];
    
    // Enable "App Nap" because the session is done.
    [[NSProcessInfo processInfo] endActivity:self.processActivity];
}

// This is called by post_session:data: after setDesiredCapabilities. 
// It allows the session to finish private setup operations and double-check its state.
// Returns YES if the session is happy, NO if there was a problem.
// In this class, the method doesn't do anything. Subclasses can add more code and logic.
- (BOOL)finishSetup
{
    return YES;
}

#pragma mark - Getters and Setters
- (NSTimeInterval)loopDelay
{
    NSNumber *value = [self getCookieWithName:kCookieLoopDelay][@"value"];
//    NSLog(@"loopDelay: %f", [value floatValue]);
    return [value floatValue];
}

- (NSTimeInterval)commandDelay
{
    NSNumber *value = [self getCookieWithName:kCookieCommandDelay][@"value"];
//    NSLog(@"commandDelay: %f", [value floatValue]);
    return [value floatValue];
}

- (NSTimeInterval)implicitTimeout
{
    NSNumber *value = [self getCookieWithName:kCookieImplicitTimeout][@"value"];
//    NSLog(@"implicitTimeout: %f", [value floatValue]);
    return [value floatValue];
}

- (NSTimeInterval)pageLoadTimeout
{
    NSNumber *value = [self getCookieWithName:kCookiePageLoadTimeout][@"value"];
//    NSLog(@"pageLoadTimeout: %f", [value floatValue]);
    return [value floatValue];
}

- (NSTimeInterval)scriptTimeout
{
    NSNumber *value = [self getCookieWithName:kCookieScriptTimeout][@"value"];
//    NSLog(@"scriptTimeout: %f", [value floatValue]);
    return [value floatValue];
}

- (float)mouseMoveSpeed
{
    NSNumber *value = [self getCookieWithName:kCookieMouseSpeed][@"value"];
//    NSLog(@"mouseMoveSpeed: %f", [value floatValue]);
    return [value floatValue];
}

- (BOOL)shouldTakeScreenShot
{
    NSNumber *value = [self getCookieWithName:kCookieScreenShotOnError][@"value"];
//    NSLog(@"shouldTakeScreenShot: %@", [value boolValue] ? @"YES":@"NO");
    return [value boolValue];
}

- (NSString *)globalDiagnosticsDirectory
{
    NSString *value = [self getCookieWithName:kCookieGlobalDiagnosticsDirectory][@"value"];
//    NSLog(@"globalDiagnosticsDirectory: %@", value);
    return value;
}

- (NSString *)diagnosticsDirectory
{
    NSString *value = [self getCookieWithName:kCookieDiagnosticsDirectory][@"value"];
//    NSLog(@"diagnosticsDirectory: %@", value);
    return value;
}

- (void)setLoopDelay:(NSTimeInterval)interval
{
    NSNumber *value = [NSNumber numberWithFloat:interval];
//    NSLog(@"setLoopDelay: %f", [value floatValue]);
    [self setCookie:@{@"name": kCookieLoopDelay, @"value": value}];
}

- (void)setCommandDelay:(NSTimeInterval)interval
{
    NSNumber *value = [NSNumber numberWithFloat:interval];
//    NSLog(@"setCommandDelay: %f", [value floatValue]);
    [self setCookie:@{@"name": kCookieCommandDelay, @"value": value}];
}

- (void)setImplicitTimeout:(NSTimeInterval)interval
{
    NSNumber *value = [NSNumber numberWithFloat:interval];
//    NSLog(@"setImplicitTimeout: %f", [value floatValue]);
    [self setCookie:@{@"name": kCookieImplicitTimeout, @"value": value}];
}

- (void)setPageLoadTimeout:(NSTimeInterval)interval
{
    NSNumber *value = [NSNumber numberWithFloat:interval];
//    NSLog(@"setPageLoadTimeout: %f", [value floatValue]);
    [self setCookie:@{@"name": kCookiePageLoadTimeout, @"value": value}];
}

- (void)setScriptTimeout:(NSTimeInterval)interval
{
    NSNumber *value = [NSNumber numberWithFloat:interval];
//    NSLog(@"setScriptTimeout: %f", [value floatValue]);
    [self setCookie:@{@"name": kCookieScriptTimeout, @"value": value}];
}

- (void)setMouseMoveSpeed:(float)interval
{
    NSNumber *value = [NSNumber numberWithFloat:interval];
//    NSLog(@"setMouseMoveSpeed: %f", [value floatValue]);
    [self setCookie:@{@"name": kCookieMouseSpeed, @"value": value}];
}

- (void)setShouldTakeScreenShot:(BOOL)yesNo
{
    NSNumber *value = [NSNumber numberWithBool:yesNo];
//    NSLog(@"setShouldTakeScreenShot: %@", [value boolValue] ? @"YES":@"NO");
    [self setCookie:@{@"name": kCookieScreenShotOnError, @"value": value}];
}

//- (void)setGlobalDiagnosticsDirectory:(NSString *)value
//{
//    NSLog(@"setGlobalDiagnosticsDirectory: %@", value);
//    [self setCookie:@{@"name": kCookieGlobalDiagnosticsDirectory, @"value": value}];
//}

- (void)setDiagnosticsDirectory:(NSString *)value
{
//    NSLog(@"setDiagnosticsDirectory: %@", value);
    [self setCookie:@{@"name": kCookieDiagnosticsDirectory, @"value": value}];
}

#pragma mark - Cookies
- (NSArray *)getAllCookies
{
    return self.afmCookies;
}

- (NSMutableDictionary *)getCookieWithName:(NSString *)name
{
    for (NSMutableDictionary *cookie in self.afmCookies) {
        if (cookie && [cookie isKindOfClass:[NSMutableDictionary class]] && [cookie[@"name"] isEqualToString:name]) {
            return cookie;
        }
    }
    return nil;
}

// If the cookie with this name exists, replace it (do not merge).
- (void)setCookie:(NSDictionary *)cookie
{
//    NSLog(@"setCookie:%@", cookie);
    id existingCookie = [self getCookieWithName:cookie[@"name"]];
    if (existingCookie) {
//        (@"existingCookie:%@", existingCookie);
        [self.afmCookies removeObject:existingCookie];
    }
    NSMutableDictionary *mutableCookie = [cookie mutableCopy];
    if ([mutableCookie[@"name"] isEqualToString:kCookieGlobalDiagnosticsDirectory]) {
        [self setGlobalDiagnosticsDirectoryCookie:mutableCookie];
    } else {
        [self.afmCookies addObject:mutableCookie];
    }
    
}

#pragma mark
-(NSArray*) allProcessNames
{
    NSMutableArray *processes = [NSMutableArray new];
    for(SystemEventsProcess *process in [self.systemEvents processes])
    {
        [processes addObject:[process name]];
    }
    return processes;
}

-(NSArray*) allWindows
{
    return [[NSArray arrayWithObject:self.currentApplication] arrayByAddingObjectsFromArray:[self.currentApplication AXWindows]];
//    return [self.currentApplication AXWindows];
}

-(NSArray*) allWindowHandles
{
	NSMutableArray *windowHandles = [NSMutableArray new];
	NSArray *windows = [self allWindows];
	for(int i=0; i < windows.count; i++)
	{
		[windowHandles addObject:[NSString stringWithFormat:@"%d", i]];
	}
	return windowHandles;
}

-(void) activateApplication
{
    [self.currentApplication activateApplication];
}

-(BOOL) activateWindow
{
	if ([self.currentWindow isEqualToElement:self.currentApplication])
	{
		[self activateApplication];
		return YES;
	}
	return [self.currentWindow performAction:(NSString*)kAXRaiseAction];
}

// Script can send a list of cookies to add wholesale.
// Each cookie is a dictionary: {@"name": nameString, @"value": value}.
// NON-STANDARD
- (void)setDesiredCapabilities:(NSDictionary *)desiredCapabilities
{
    if (desiredCapabilities) {
//        NSLog(@"desiredCapabilities %@", desiredCapabilities);
        
        NSArray *cookies = desiredCapabilities[@"cookies"];
        if (cookies && [cookies isKindOfClass:[NSArray class]]) {
            for (NSDictionary *cookie in cookies) {
                if ([cookie isKindOfClass:[NSDictionary class]]) {
                    [self setCookie:cookie];
                }
            }
        }
    }
}

// If applicationName is nil, this returns the front (key) application.
-(PFApplicationUIElement *)applicationForName:(NSString*)applicationName
{
    if (!applicationName) {
        NSRunningApplication *frontApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
        applicationName = [frontApp localizedName];
    }
    NSArray *runningApps = [[NSWorkspace sharedWorkspace] runningApplications];
    for (NSRunningApplication *runningApp in runningApps) {
        if ([runningApp.localizedName isEqualToString:applicationName]) {
            NSURL *appURL = runningApp.bundleURL;
            PFApplicationUIElement *appUIElement = [PFApplicationUIElement applicationUIElementWithURL:appURL delegate:nil];
            return appUIElement;
        }
    }
	return nil;
}

//-(NSString*) applicationNameForProcessName:(NSString*)processName
//{
//    NSDictionary *errorDict;
//    NSAppleScript *appForProcNameScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset process_bid to get the bundle identifier of process \"%@\"\nset application_name to file of (application processes where bundle identifier is process_bid)\nend tell\nreturn application_name", processName]];
//    NSString *statusString = [[appForProcNameScript executeAndReturnError:&errorDict] stringValue];
//    return errorDict.count > 0 ? nil : statusString;
//}


// This is where all WebDriver commands are executed.
// Common tasks are handled here, so each command does not have to implement. 
// Tasks include:
//      Convert postData into a dictionary.
//      Extract an element ID from the path or postData. If there is one, look up the element object and validate it.
//      Automatic ImplicitTimeout-limited retries.
//      Exception handling.
- (AppiumMacHTTPJSONResponse *)executeWebDriverCommandWithPath:(NSString *)path data:(NSData *)postData onMainThread:(BOOL)onMainThread 
                                                  commandBlock:(AppiumMacHTTPJSONResponse *(^)(AfMSessionController *session, NSDictionary *commandParams, int *statusCode))commandBlock
{
    // initialize status as though no element were found
    __block AppiumMacHTTPJSONResponse *handlerReturnValue = nil;
    __block int statusCode = kAfMStatusCodeNoSuchElement;
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    
    // Delay before starting the command.
    if (self.commandDelay > 0) {
        [NSThread sleepForTimeInterval:self.commandDelay];
    }
    
    NSMutableDictionary *commandParams = [NSMutableDictionary dictionary];
    if (postData) {
        NSError *error = nil;
        NSDictionary *jsonParams = [NSJSONSerialization JSONObjectWithData:postData options:0 error:&error];
        if (!jsonParams || ![jsonParams isKindOfClass:[NSDictionary class]]) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeInvalidArgument session:sessionId];
        }
        [commandParams addEntriesFromDictionary:jsonParams];
    }
    
    // Many commands act on a previously located element. The element id is specified in either the URL path, 
    // or in the JSON parameters as "element". If there is an id, then get the element object.
    NSString *elementId = [Utility getElementIDFromPath:path];
    if (!elementId || [elementId length] == 0) {
        // Check the JSON parameters.
        elementId = [commandParams objectForKey:@"element"];
    }
    
    if (elementId) {
        // get the element
        id element = [self.elements objectForKey:elementId];
        
        // Check the element is valid. This does NOT check for displayed. 
        // Each handler method that requires the element be visible (e.g. click) must verify displayed. 
        int status = [self checkElement:element];
        if (status != kAfMStatusCodeSuccess) {
            return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchElement session:sessionId];
        }
        
        // Add the object to the parameters
        [commandParams setObject:element forKey:@"elementObject"];
    }
    
    // Timeouts use a large loop.
    NSDate *timeoutDate = [[NSDate date] dateByAddingTimeInterval:self.implicitTimeout ? self.implicitTimeout : 0.001];
    
    while (!self.isCanceled && [timeoutDate timeIntervalSinceNow] > 0) {
        // We will execute queueBlock on the main thread, or the server thread, as determined by onMainThread.
        dispatch_block_t queueBlock = ^(){
            @try {
                handlerReturnValue = commandBlock(self, commandParams, &statusCode);
            }
            @catch (NSException *e) {
                NSLog(@"Exception in dispatch_sync(dispatch_get_main_queue()): %@", e);
            }
        };
        
        // Calling dispatch_sync will deadlock if called on the same thread as the queue. 
        // Use it only if a caller from a non-main thread requests main thread execution.
        if (onMainThread && ![NSThread isMainThread]) {
            dispatch_sync(dispatch_get_main_queue(), queueBlock);
        } else {
            queueBlock();
        }
        
        if (!handlerReturnValue) {
            break;
        }
        
        if ([handlerReturnValue afmStatusCode] == kAfMStatusCodeNoSuchElement) {
            // Element not found. Wait for the loop delay, before trying again.
            // Regardless of loop delay, don't wait past the timeout date.
            if ([timeoutDate timeIntervalSinceNow] >= self.loopDelay) {
                [NSThread sleepForTimeInterval:self.loopDelay];
            } else if ([timeoutDate timeIntervalSinceNow] > 0) {
                [NSThread sleepForTimeInterval:[timeoutDate timeIntervalSinceNow]];
            }
            //NSLog(@"Session %@ waiting for commandParams: %@ time remaining: %f", sessionId, commandParams, [timeoutDate timeIntervalSinceNow]);
            continue;
        } else {
            return handlerReturnValue;
        }
    }
    if (self.isCanceled) {
        NSLog(@"executeWebDriverCommandWithPath:: session isCanceled after waiting: %@", self.sessionId);
        return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeNoSuchDriver session:self.sessionId];
    }
    if ([handlerReturnValue afmStatusCode] == kAfMStatusCodeNoSuchElement
        ) {
        // Implicit timeout reached!
        // Take a screenshot.
        if (self.shouldTakeScreenShot) {
            // The file name starts with the selenium path, and will end with a date stamp.
            // E.g. session_hqAc1lh8_element__2016-02-23_01_34_22_+0000
            
            NSMutableArray *seleniumPathComponents = [[path componentsSeparatedByString:@"/"] mutableCopy];
            [seleniumPathComponents removeObjectsAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, 3)]];
            NSString *fileName = [seleniumPathComponents componentsJoinedByString:@"_"];
            
            [self screenCaptureToFile:fileName withDateStamp:YES];
        }
    }    
    return [AppiumMacHTTPJSONResponse responseWithJsonError:statusCode session:sessionId];
}

// Make sure the element exists, is in the current window.
// This does _not_ verify if the element is visible. Use isElementDisplayed:.
- (int) checkElement:(id)element
{
    if (element == nil || ![element isKindOfClass:[PFUIElement class]])
    {
        return kAfMStatusCodeNoSuchElement;
    }
    if (![self.currentWindow exists])
    {
        return kAfMStatusCodeNoSuchWindow;
    }
    if (![element exists])
    {
        return kAfMStatusCodeStaleElementReference;
    }
    return kAfMStatusCodeSuccess;
}

// Is the element displayed? Hidden elements can be
// found and examined, but certain actions (e.g. click) will fail.
- (BOOL) isElementDisplayed:(id)element
{
    if ([element isKindOfClass:[PFUIElement class]]) {
        NSPoint middlePoint = [[element AXPosition] pointValue];
        NSSize elementSize = [[element AXSize] sizeValue];
        middlePoint.x += elementSize.width/2.0;
        middlePoint.y += elementSize.height/2.0;
        if ([element isVisibleAtPoint:middlePoint]) {
            return YES;
        }
    }
    return NO;
}

-(BOOL) clickElement:(id)element
{
	if (element == nil)
	{
		return NO;
	}
	if ([element isKindOfClass:[PFUIElement class]])
	{
        return [(PFUIElement*)element performAction:(NSString*)kAXPressAction];
	}
    return NO;
}

-(void) closeWindow
{
	// TODO: Implement working close window method
    [self.currentWindow performAction:@"AXCancel"];
}

-(NSString*) currentApplicationName
{
	return self._currentApplicationName;
}

-(void) setCurrentApplicationName:(NSString *)currentApplicationName
{
	self._currentApplicationName = currentApplicationName;
	[self setCurrentApplication:[self applicationForName:currentApplicationName]];
	[self setCurrentProcessName:[self processNameForApplicationName:currentApplicationName]];
	[self setCurrentWindowHandle:@"0"];
}

-(SystemEventsProcess*) currentProcess
{
    return [self processForName:self.currentProcessName];
}

-(PFUIElement*) currentWindow
{
    PFUIElement *window = [self windowForHandle:self.currentWindowHandle];
    if (!window) {
        NSLog(@"currentWindow is nil! currentWindowHandle: %@", self.currentWindowHandle);
    }
    return window;
}

-(NSString*) pageSource
{
	NSMutableDictionary *dom = [NSMutableDictionary new];
	[self pageSourceHelperFromRootUIElement:self.currentWindow dictionary:dom];

	NSError *error;
	NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dom
													   options:0/*NSJSONWritingPrettyPrinted*/
														 error:&error];
	if (! jsonData)
	{
		return nil;
	}
	else
	{
		return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
	}
}

-(void)pageSourceHelperFromRootUIElement:(PFUIElement*)rootUIElement dictionary:(NSMutableDictionary*) dict
{
	[dict setValue:rootUIElement.AXRole forKey:@"AXRole"];
	[dict setValue:rootUIElement.AXTitle forKey:@"AXTitle"];
	[dict setValue:rootUIElement.AXDescription forKey:@"AXDescription"];
	NSMutableArray *children = [NSMutableArray new];
	[dict setValue:children forKey:@"AXChildren"];
	for (PFUIElement *childUIElement in rootUIElement.AXChildren)
	{
		NSMutableDictionary *childDict = [NSMutableDictionary new];
		[self pageSourceHelperFromRootUIElement:childUIElement dictionary:childDict];
		[children addObject:childDict];
	}
}

-(NSInteger) pidForProcessName:(NSString*)processName
{
	// TODO: Add error handling
    return [self processForName:processName].unixId;
}

-(SystemEventsProcess*) processForName:(NSString*)processName
{
	return [self.systemEvents.processes objectWithName:processName];
}

-(NSString*) processNameForApplicationName:(NSString*) applicationName
{
    NSDictionary *errorDict;
    NSAppleScript *frontMostProcessScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset application_id to (get the id of application \"%@\" as string)\nset process_name to name of (application processes where bundle identifier is application_id)\nend tell\nreturn item 1 of process_name as text", applicationName]];
    NSString *statusString = [[frontMostProcessScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return errorDict.count > 0 ? nil : statusString;
}

// Call this from a non-main thread.
- (AppiumMacHTTPJSONResponse *)postURL:(NSString *)url
{    
    // For better performance, use the system to launch the app.
    if (!url || !url.length) {
        return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeInvalidArgument session:self.sessionId];
    }
    
    // Check if it is already in front.
    NSRunningApplication *frontApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
    if ([[frontApp localizedName] isEqualToString:url] || [[[frontApp executableURL] path] isEqualToString:url]) {
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:self.sessionId];
    }
    // It may take some time to launch. Wait for the frontmost application to change before continuing.
    self.timedEventsCompleted = dispatch_semaphore_create(0);
    
    NSTimeInterval launchTimeout = 10.0;  // 10 seconds or bust!
    uint64_t interval = launchTimeout * NSEC_PER_SEC;
    dispatch_time_t waitInterval = dispatch_time(DISPATCH_TIME_NOW, interval);
    
    // Register to observe when the front app changes.
    [[NSWorkspace sharedWorkspace] addObserver:self
                                    forKeyPath:@"frontmostApplication"
                                       options:NSKeyValueObservingOptionNew
                                       context:nil];
    
    //NSLog(@"Launching app with url: %@", url);
    [[NSWorkspace sharedWorkspace] performSelectorOnMainThread:@selector(launchApplication:) withObject:url waitUntilDone:NO];
    
    long errorCode = dispatch_semaphore_wait(self.timedEventsCompleted, waitInterval);    
    
    if (errorCode == noErr) {
        //NSLog(@"Application launched with url: %@ (%@)", [[NSWorkspace sharedWorkspace] frontmostApplication], url);
        // Now use AppleScript and Accessibility to connect to the running app.
        [self performSelectorOnMainThread:@selector(setCurrentApplicationName:) withObject:url waitUntilDone:YES];
        [self performSelectorOnMainThread:@selector(activateApplication) withObject:nil waitUntilDone:YES];
        
        return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:self.sessionId];
    }
    
    //NSLog(@"Failed to launch app with url: %@", url);
    return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownError session:self.sessionId];
}

// Since we called NSWorkspace on the main thread, this should execute on the main thread.
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSString *,id> *)change context:(void *)context
{
    if ([object isEqual:[NSWorkspace sharedWorkspace]] && [keyPath isEqualToString:@"frontmostApplication"]) {
        
        [[NSWorkspace sharedWorkspace] removeObserver:self forKeyPath:@"frontmostApplication"];
        
        // Allow the observing thread to continue.
        dispatch_semaphore_signal(self.timedEventsCompleted);
    }
    
}

- (void)sendKeys:(NSArray*)keys toElement:(id)element
{
    if (element && [element isKindOfClass:[PFUIElement class]]) {
        [element setAXFocused:[NSNumber numberWithBool:YES]];
    }
    
    [self sendKeys:keys];
}

- (void)sendKeys:(NSArray*)keys
{
    // Convert an array of strings and characters to a single array of characters.
    for (id keyChunk in keys) {
        NSMutableArray *chars = [@[] mutableCopy];
        
        if ([keyChunk isKindOfClass:[NSString class]]) {
            for (int i = 0; i < [keyChunk length]; i ++) {
                UniChar thisChar = [keyChunk characterAtIndex:i];
                [chars addObject:[NSString stringWithCharacters:&thisChar length:1]];
            }
        } else  {
            // What else can we get besides a string? Time will tell, if anything...
            continue;
        }
        
        // Give the OS time to press and release keys. Going too fast will skip keystrokes.
        const NSTimeInterval kMinimumKeyStrokeInterval = 0.05;
        for (NSString *thisChar in chars) {
            UniChar thisUnichar = [thisChar characterAtIndex:0];
            [self performWebDriverKeystroke:thisUnichar];
            [NSThread sleepForTimeInterval:kMinimumKeyStrokeInterval];
        }
    }
}

// WebDriver keystrokes are a hybrid of pressing and releasing:
//  A modifier character will toggle the modifier from up to down and vice versa.
//  A non-modifier character will generate a key down followed by key up.
- (void)performWebDriverKeystroke:(UniChar)keyStroke
{
    UniChar character = keyStroke; // We can use this for pointer arguments.
    
    // Is this a modifier? Modifiers we look for are shift, control, option/alt, command/meta.
    NSString *modifierString = @"\uE008\uE009\uE00A\uE03D";
    if ([modifierString containsString:[NSString stringWithCharacters:&character length:1]]) {
        [self performToggleModifier:character];
        return;
    }    
    
    // Is this a character with a mapped keyCode?
    // Use the lower case version of the character.
    BOOL requiresShiftModifier = NO;
    BOOL unToggleShiftModifier = NO;
    CGKeyCode keyCode = [KeystrokesAndKeyCodes keyCodeWithWebDriverKeystroke:character isShifted:&requiresShiftModifier];
    
    if (requiresShiftModifier) {
        if (!self.isShiftKeyDown) {
            unToggleShiftModifier = YES;
            [self performToggleModifier:wdShiftKey];
        }
    }
    
    // Key down
    CGEventRef keyDownEvent = CGEventCreateKeyboardEvent(NULL, keyCode, true);
    if (keyDownEvent) {
        CGEventPost(kCGHIDEventTap, keyDownEvent);
        CFRelease(keyDownEvent);
        
        // Key up
        CGEventRef keyUpEvent = CGEventCreateKeyboardEvent(NULL, keyCode, false);
        if (keyUpEvent) {
            CGEventPost(kCGHIDEventTap, keyUpEvent);
            CFRelease(keyUpEvent);
        }
    }
    
    if (unToggleShiftModifier) {
        [self performToggleModifier:wdShiftKey];
    }
}

// Give the OS time to "actually" press and release the modifiers.
// Going too fast will skip modifiers.
const NSTimeInterval kModifierPause = 0.05;

// WebDriver keystrokes are a hybrid of pressing and releasing:
//  A modifier character will toggle the modifier from up to down and vice versa. 
- (void)performToggleModifier:(UniChar)character
{
    bool isKeyDownEvent;
    CGKeyCode virtualKeyCode;
    switch (character) {
        case wdShiftKey:
            isKeyDownEvent = !self.isShiftKeyDown;
            virtualKeyCode = [KeystrokesAndKeyCodes keyCodeWithWebDriverKeystroke:character isShifted:nil];
            break;
        case wdControlKey:
            isKeyDownEvent = !self.isControlKeyDown;
            virtualKeyCode = [KeystrokesAndKeyCodes keyCodeWithWebDriverKeystroke:character isShifted:nil];
            break;
        case wdAltKey:
            isKeyDownEvent = !self.isAltKeyDown;
            virtualKeyCode = [KeystrokesAndKeyCodes keyCodeWithWebDriverKeystroke:character isShifted:nil];
            break;
        case wdCommandKey:
            isKeyDownEvent = !self.isCommandKeyDown;
            virtualKeyCode = [KeystrokesAndKeyCodes keyCodeWithWebDriverKeystroke:character isShifted:nil];
            break;
        default:
            isKeyDownEvent = NO;
            virtualKeyCode = 0;
            break;
    }
    if (virtualKeyCode) {
        CGEventRef event = CGEventCreateKeyboardEvent(NULL, virtualKeyCode, isKeyDownEvent);
        if (event) {
            [NSThread sleepForTimeInterval:kModifierPause];
            CGEventPost(kCGHIDEventTap, event);
            [NSThread sleepForTimeInterval:kModifierPause];
            CFRelease(event);
            
            // On success, toggle the modifier state variable.
            switch (character) {
                case wdShiftKey:
                    self.isShiftKeyDown = !self.isShiftKeyDown;
                    break;
                case wdControlKey:
                    self.isControlKeyDown = !self.isControlKeyDown;
                    break;
                case wdAltKey:
                    self.isAltKeyDown = !self.isAltKeyDown;
                    break;
                case wdCommandKey:
                    self.isCommandKeyDown = !self.isCommandKeyDown;
                    break;
            }
        }
    }
}

// When ending a session (or quitting), be sure to release all modifiers. 
- (void)releaseAllModifiers
{
    // FIXME: skip all code for debugging
    return;
    
    
    // Mac keyboard: shift, control, option, command
    NSArray *webDriverModifiers = @[@(wdShiftKey), @(wdControlKey), @(wdAltKey), @(wdCommandKey)];
    for (NSNumber *number in webDriverModifiers) {
        unichar character = [number unsignedShortValue];
        CGKeyCode virtualKeyCode = [KeystrokesAndKeyCodes keyCodeWithWebDriverKeystroke:character isShifted:nil];
        if (virtualKeyCode) {
            BOOL isKeyDownEvent = NO;
            CGEventRef event = CGEventCreateKeyboardEvent(NULL, virtualKeyCode, isKeyDownEvent);
            if (event) {
                CGEventPost(kCGHIDEventTap, event);
                CFRelease(event);
            }
        }
    }
    self.isShiftKeyDown = NO;
    self.isControlKeyDown = NO;
    self.isAltKeyDown = NO;
    self.isCommandKeyDown = NO;
}


-(PFUIElement*) windowForHandle:(NSString*)windowHandle
{
	NSArray *windows = self.allWindows;
	int windowIndex = [windowHandle intValue];
	if (windowIndex > windows.count)
	{
		return nil;
	}
	return [windows objectAtIndex:windowIndex];
}

-(GDataXMLDocument*)xmlPageSource
{
	return [self xmlPageSourceFromRootUIElement:nil pathMap:nil xPath:nil];
}

-(GDataXMLDocument*)xmlPageSourceFromRootUIElement:(PFUIElement*)rootUIElement pathMap:(NSMutableDictionary*)pathMap xPath:(NSString *)xPath
{
    //NSLog(@"xmlPageSourceFromRootUIElement:%@ pathMap:%@ xPath:%@", [rootUIElement debugDescription], pathMap, xPath);

	if (rootUIElement == nil) {
		// self.allWindows includes the application uiElement as item 0, and self.currentWindow usually returns the item 0.
        // So, rootUIElement is usually the application element here.
        rootUIElement = self.currentApplication;
//        rootUIElement = self.currentWindow; 

        if ([xPath rangeOfString:@"AXMenuBar"].length > 0 || [xPath rangeOfString:@"AXMenuItem"].length > 0) {
            // Make sure to search the whole application if looking for a menu item
            rootUIElement = self.currentApplication; 
        }
    }

	GDataXMLElement *rootXMLElement = [GDataXMLNode elementWithName:rootUIElement.AXRole];
	[self xmlPageSourceHelperFromRootUIElement:rootUIElement rootXMLElement:rootXMLElement path:@"/*[1]" pathMap:pathMap xPath:xPath];
	GDataXMLDocument *doc = [[GDataXMLDocument alloc] initWithRootElement:rootXMLElement];
	return doc;
}

-(void)xmlPageSourceHelperFromRootUIElement:(PFUIElement*)rootUIElement rootXMLElement:(GDataXMLElement*)rootXMLElement path:(NSString*)path pathMap:(NSMutableDictionary*)pathMap xPath:(NSString *)xPath
{    
    // Cheap and dirty performance improvement by skipping XML branch parsing, based on simple keyword match in the XPath.
    if (xPath != nil && rootUIElement != nil) {
        if ([xPath rangeOfString:@"AXMenuBar"].length > 0) {
            // We are looking in the menu bar, so skip any windows.
            // The XPath string must contain AXMenuBar (e.g. "//AXMenuBarItem...") to search the menu bar faster. 
            // It will work if you do not use AXMenuBar (e.g. "//AXMenuItem...", but it takes much longer.
            if ([rootUIElement.AXRole rangeOfString:@"AXWindow"].length > 0) {
                return; 
            }
        }
    }
    
    [rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXRole" stringValue:rootUIElement.AXRole]];
    [rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXTitle" stringValue:rootUIElement.AXTitle]];
    [rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXDescription" stringValue:rootUIElement.AXDescription]];
    [rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXRoleDescription" stringValue:rootUIElement.AXRoleDescription]];
    
    //Added AXSize and AXPosition attributes to the Page Source so that we know where each element is
    NSSize elementSize = [rootUIElement.AXSize sizeValue];
    NSInteger elementWidth = (NSInteger) (floor(elementSize.width));
    NSInteger elementHeight = (NSInteger) (floor(elementSize.height));
    
    NSPoint elementPosition = [rootUIElement.AXPosition pointValue];
    NSInteger elementX = (NSInteger) (floor(elementPosition.x));
    NSInteger elementY = (NSInteger) (floor(elementPosition.y));
    
    [rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXSize" stringValue: [NSString stringWithFormat: @"%ld;%ld", elementWidth, elementHeight]]];
    [rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXPosition" stringValue: [NSString stringWithFormat: @"%ld;%ld", elementX, elementY]]];
    
    NSString *subrole = rootUIElement.AXSubrole;
    if (subrole) {
        [rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXSubrole" stringValue:rootUIElement.AXSubrole]];
    }

	if (rootUIElement.AXValue != nil)
	{
		[rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXValue" stringValue:[NSString stringWithFormat:@"%@",rootUIElement.AXValue]]];
	}

	NSString *identifier = [rootUIElement valueForAttribute:@"AXIdentifier"];
	if (identifier != nil)
	{
		[rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"AXIdentifier" stringValue:[NSString stringWithFormat:@"%@",[rootUIElement valueForAttribute:@"AXIdentifier"]]]];
	}

	if (pathMap != nil)
	{
		[pathMap setValue:rootUIElement forKey:path];
		[rootXMLElement addAttribute:[GDataXMLElement attributeWithName:@"path" stringValue:path]];
	}

	// Cheap and dirty performance improvement by checking more often if we are done.
    if (xPath != nil && rootXMLElement != nil) {
        NSError *error;
        GDataXMLDocument *doc = [[GDataXMLDocument alloc] initWithRootElement:rootXMLElement];
        NSArray *matches = [doc nodesForXPath:xPath error:&error];
        if ([matches count] > 0 && error == nil) {
            return;
        }
    }
    
	for (int i=0; i < rootUIElement.AXChildren.count; i++)
	{
		PFUIElement *childUIElement = [rootUIElement.AXChildren objectAtIndex:i];
        
//        NSLog(@"#++++++++++++++++++++++++++++++++++++++++++++++++++");
//        NSLog(@"%@", [childUIElement path]);
        
        
		GDataXMLElement *childXMLElement = [GDataXMLElement elementWithName:childUIElement.AXRole];
		[self xmlPageSourceHelperFromRootUIElement:childUIElement rootXMLElement:childXMLElement path:[path stringByAppendingFormat:@"/*[%d]", i+1] pathMap:pathMap xPath:xPath];
		[rootXMLElement addChild:childXMLElement];

		// Cheap and dirty performance improvement by checking more often if we are done.
        // 20170404 For some reason this crashes during pageSource. Maybe it's a little too dirty!
        // Commenting out for now. It's a little slower, but no crash.
//        NSError *error;
//        GDataXMLDocument *doc = [[GDataXMLDocument alloc] initWithRootElement:rootXMLElement];
//        NSArray *matches = [doc nodesForXPath:xPath error:&error];
//        if ([matches count] > 0 && error == nil) {
//            return;
//        }        
    } 
}

// Locate UI Element objects with a simple absolute AXPath.
// Absolute XPaths allow a much faster search, because it avoids building the entire UI page source.
// A simple absolute AXPath:
//    is valid XPath syntax
//    uses OSX Accessibility constants, e.g. AXMenuItem or AXTitle
//    begins with /AXApplication
//    does not contain "//", or use a wildcard, or specify multiple paths using |
//    uses predicates with only simple attribute string values
//    does not use predicate strings containing [ or ]
//    uses single quotes, not double quotes for attribute strings
//    does not contain spaces in the predicate, except in the attribute string itself
//    specifies one node with each predicate
// The caller must ensure the AXPath is valid. An invalid AXPath generates undefined results, but in most cases the method will simply return an empty array.
// Good example:
//    @"/AXApplication/AXMenuBar/AXMenuBarItems/AXMenuBarItem[@AXTitle='Window']/AXMenu/AXMenuItem[@Title='Customers']"
// Bad examples:
//    @"//AXMenuBar/AXMenuBarItems/AXMenuBarItem[@AXTitle='Window']/AXMenu/AXMenuItem[@Title='Customers']"
//      (does not begin with /AXApplication, and contains //)
//
//    @"/AXApplication/AXMenuBarItems/AXMenuBarItem[@AXTitle='Window']/AXMenu/AXMenuItem[@Title='Customers']"
//      (not an absolute path: missing AXMenuBar)
//
//    @"/AXApplication/AXMenuBar/AXMenuBarItems/AXMenuBarItem[@AXTitle="Window"]/AXMenu/AXMenuItem[@Title='Customers']"
//      (predicate string uses double quotes)
//
//    @"/AXApplication/AXMenuBar/AXMenuBarItems/AXMenuBarItem[@AXTitle='(Window)']/AXMenu/AXMenuItem[@Title='Customers']"
//      (predicate string contains forbidden characters)
//
//    @"/AXApplication/AXMenuBar/AXMenuBarItems/AXMenuBarItem[@AXTitle ='Window']/AXMenu/AXMenuItem[@Title='Customers']"
//      (predicate contain a space before the =)
//
//    @"/AXApplication/AXMenuBar/AXMenuBarItems/AXMenuBarItem[position()>3]/AXMenu/AXMenuItem[@Title='Customers']"
//      (predicate is not a simple string match, and specifies more than one node)
#define axPathDebugLog NSLog
- (NSArray *)findAllUsingAbsoluteAXPath:(NSString *)axPath
{
    axPathDebugLog(@"\n\n\n************* findAllUsingAbsoluteAXPath: %@\n", axPath);
    
    if (axPath == nil || [axPath rangeOfString:@"/AXApplication"].location != 0 || [axPath rangeOfString:@"//"].location != NSNotFound) {
        return nil;
    }
    
    // Special case: check for the AXApplication element itself.
    // This can be used for commands like moveto. An offset from the AXApplication element specifies a global screen point.
    if ([axPath isEqualToString:@"/AXApplication"]) {
        return @[self.currentApplication];
    }
    
    // Decompose the axPath into separate role-predicate strings.
    NSMutableArray *axPathComponents = [[axPath componentsSeparatedByString:@"/"] mutableCopy];
    
    // The first component is @"" because an absolute path must begin with @"/". Remove it.
    [axPathComponents removeObjectAtIndex:0];
    axPathDebugLog(@"axPathComponents:%@", axPathComponents);
    
    // This time, the first component must be @"AXApplication". It may or may not have a predicate.
    NSString *appNodeString = axPathComponents[0];
    NSDictionary *appNodeParsed = [self parseRoleAndPredicateString:appNodeString];
    if (!appNodeParsed || ![appNodeParsed[kNodeRole] isEqualTo:@"AXApplication"]) {
        axPathDebugLog(@"findAllUsingAXPath: PARSE APP NODE STRING FAILED");
        return @[];
    }
    NSDictionary *appPredicate = appNodeParsed[kNodePredicate];
    if (appPredicate) {
        // Find the app by name in all running applications.
        NSDictionary *operation = appPredicate[kPredicateOperations][0];
        
        NSString *appName = nil;
        if (operation[kPredicateAttributeName]) {
            // We have a predicate with string value. We need to set currentApplication.
            appName = operation[kPredicateAttributeValue];
            PFApplicationUIElement *uiApp = [self applicationForName:appName];
            self.currentApplication = uiApp;
            if ([[uiApp AXTitle] isEqualToString:@"Dock"]) {
                axPathDebugLog(@"Dock Attributes: %@", [uiApp attributes]);
                axPathDebugLog(@"Dock AXChildren %@", [uiApp AXChildren]);
                axPathDebugLog(@"Dock AXFocusedUIElement %@", [uiApp AXFocusedUIElement]);
                axPathDebugLog(@"Dock AXEnhancedUserInterface %@", [uiApp AXEnhancedUserInterface]);
                //if ([uiApp AXFocusedUIElement]) {
                //    NSArray *attributes = [[uiApp AXFocusedUIElement] attributes];
                //    for (NSString *attribute in attributes) {
                //        NSLog(@"AXDock AXFocusedUIElement: %@:%@", attribute, [[uiApp AXFocusedUIElement] valueForKey:attribute]);
                //    }
                //}
            }
        }
    } else {
        // Use the front application.
        PFApplicationUIElement *uiApp = [self applicationForName:nil];
        self.currentApplication = uiApp;
    }
    
    // Now remove the application component because we want to check its child elements.
    // For example, if axPath == @"/AXApplication/AXMenuBar/AXMenuBarItems/AXMenuBarItem[@AXTitle='Window']",
    // then after this call, axPathComponents will be [@"AXMenuBar", "AXMenuBarItems", @"AXMenuBarItem[@AXTitle='Window']"].
    [axPathComponents removeObjectAtIndex:0];
    
    // Deep dive recursively.
    NSArray *matchedNodes = [self findAllUsingAXPathComponents:axPathComponents rootUIElement:self.currentApplication];
    axPathDebugLog(@"\n************* findAllUsingAbsoluteAXPath matchedNodes:%@\n\n\n", matchedNodes);
    for (PFUIElement *uiElement in matchedNodes) {
        if ([[uiElement AXRole] isEqualToString:@"AXDockItem"]) {
            NSArray *attributes = [uiElement attributes];
            for (NSString *attribute in attributes) {
                axPathDebugLog(@"AXDockItem: %@:%@", attribute, [uiElement valueForKey:attribute]);
            }
        }
    }
    
    // See if there is any partially matching XPath
    if ([matchedNodes count] == 0)
    {
        matchedNodes = [self findAllUsingPartialAXPath:axPathComponents rootUIElement:self.currentApplication];
    }
    [self clearCurrentElementCache];
    
    return matchedNodes;
}

// Recursive helper method for findAllUsingAbsoluteAXPath:. 
// Each iteration will pass a smaller pathComponents array and a different rootUIElement.
// The first pathComponent specifies a child element of rootUIElement. 
- (NSArray *)findAllUsingAXPathComponents:(NSArray *)axPathComponents rootUIElement:(PFUIElement *)rootUIElement
{
    // If there aren't any child elements at all, we are done.
    NSArray *childUIElements = rootUIElement.AXChildren;
    if (!childUIElements || [childUIElements count] == 0) {
        return @[];
    }
    
    NSString *firstPathComponent = [axPathComponents objectAtIndex:0];
    NSDictionary *parsedComponent = [self parseRoleAndPredicateString:firstPathComponent];
    if (!parsedComponent || [parsedComponent count] == 0) {
        return @[];
    }
    NSMutableArray *matchedChildren = [NSMutableArray array];
    
    // Scan the child elements to match the role and (if specified) predicate.
    // First make an array of similar child elements with the same AXRole.
    // Search the similar elements for predicate or index match.
    NSArray *similarChildUIElements = [PFUIElement elementsWithRole:parsedComponent[kNodeRole] inArray:childUIElements];
    if (similarChildUIElements.count > 0) {
        for (NSUInteger i = 0; i < similarChildUIElements.count; i++) {
            PFUIElement *element = [similarChildUIElements objectAtIndex:i];
            if ([self element:element doesMatchPredicate:parsedComponent[kNodePredicate] atIndex:i]) {
                [matchedChildren addObject:element];
            }
        }
    } else {
        // This can happen when an element has an attribute with child elements, 
        // but the element doesn't include that attribute in its own list of children.
        // This is a bug because the entire AX structure should go through AXChildren.
        // E.g. For a Dock item contextual menu:
        //      Using normal xPath creation, constructed child to parent (see also Accessibility Inspector):
        //          /AXApplication[@AXTitle='Dock']/AXMenu/AXMenuItem[@AXTitle='Hide']
        //          In this case AXMenu is in the AXChildren array of the AXApplication.
        //      But the actual path from parent down to child is:
        //          /AXApplication[@AXTitle='Dock']/AXFocusedUIElement/AXMenuItem[@AXTitle='Hide']
        //          The AXFocusedUIElement is NOT in the AXChildren array.
        PFUIElement *element = [rootUIElement valueForKey:parsedComponent[kNodeRole]];
        if (element && [element isKindOfClass:[PFUIElement class]]) {
            [matchedChildren addObject:element];
        }
    }
    
    // NOTE: Even if a predicate specifies only one node (e.g. AXMenuBar) there can be multiple nodes that match.
    // For example, an AXApplication could have two children with AXMenuBar role:
    //  1) One contains AXMenuBarItem child nodes like Finder, File, Edit, etc.
    //  2) The other may contain AXMenuBarItem child nodes with AXMenuBarExtra subrole.
    // Therefore, the algorithm must scan each matched child.
    NSUInteger matchedChildrenCount = [matchedChildren count];
    if (matchedChildrenCount == 0) {
        return @[];
    } else if ([axPathComponents count] <= 1) {
        // We have one or more matched child nodes and no more path components. Success!
        return matchedChildren;
    }
    
    // Call recursively with each matched child element, and the remainder of the path components.
    // When recursion returns a non-empty array, stop and return the array.
    NSMutableArray *childAXPathComponents = [axPathComponents mutableCopy];
    [childAXPathComponents removeObjectAtIndex:0];
    for (int i = 0; i < matchedChildrenCount; i++) {
        NSArray *recursivelyMatchedChildren = [self findAllUsingAXPathComponents:childAXPathComponents rootUIElement:[matchedChildren objectAtIndex:i]];
        if ([recursivelyMatchedChildren count] > 0) {
            return recursivelyMatchedChildren;
        }
    }
    // By default there was no match.
    return @[];
}

-(PFUIElement*) findElementRecursively:(PFUIElement *)rootUIElement calls:(int)calls
{
    if ([self checkIfElementFound:rootUIElement])
    {
        return rootUIElement;
    }
    calls++;
    
    NSArray* children = rootUIElement.AXChildren;
    
    PFUIElement* result = nil;
    
    if (children)
    {
        for (PFUIElement* node in children)
        {
            result = [self findElementRecursively:node calls:calls];
            if (result)
            {
                return result;
            }
        }
    }
    
    calls--;
    return nil;
}

-(NSArray*) findAllUsingPartialAXPath:(NSArray *)axPathComponents rootUIElement:(PFUIElement *)rootUIElement
{
    NSArray* childUIElements = rootUIElement.AXChildren;
    
    if (!childUIElements || [childUIElements count] == 0)
    {
        return @[];
    }
    
    [self setRoleAndPredicatesOfElement:[axPathComponents lastObject]];
    NSMutableArray *matchedChildren = [NSMutableArray array];
    PFUIElement* foundElement = [self findElementRecursively:rootUIElement calls:0];
    
    if ([foundElement isNotEqualTo:nil])
    {
        [matchedChildren addObject:foundElement];
    }
    
    return matchedChildren;
}

-(void) setRoleAndPredicatesOfElement:(NSString*) elementToFind
{
    if (self.elementRole && self.predicateDict) {
        return;
    }
    
    NSDictionary *parseElement = [self parseRoleAndPredicateString:elementToFind];
    
    self.elementRole = parseElement[kNodeRole];
    self.predicateDict = [[NSMutableDictionary alloc] init];
    
    NSArray* predicateArray = parseElement[kNodePredicate][kPredicateOperations];
    
    for (int i=0; i < [predicateArray count]; i++)
    {
        id object = [predicateArray objectAtIndex:i];
        NSString* attributeName = object[kPredicateAttributeName];
        NSString* attributeValue = object[kPredicateAttributeValue];
        if (![attributeName isEqualToString:@"AXIdentifier"])
        {
            self.predicateDict[attributeName] = attributeValue;
        }
    }
}


-(void) clearCurrentElementCache
{
    self.elementRole = nil;
    self.predicateDict = nil;
}

-(BOOL) checkIfElementFound:(PFUIElement*) currentElement
{
    
    if ([currentElement.AXRole isNotEqualTo:self.elementRole])
    {
        return NO;
    }
    
    for( NSString* key in self.predicateDict )
    {
        if ( ![currentElement existsAttribute:key] || [self.predicateDict[key] isNotEqualTo:[currentElement valueForAttribute:key]])
        {
            return NO;
        }
    }

    return YES;
   
}

- (BOOL)element:(PFUIElement *)uiElement doesMatchPredicate:(NSDictionary *)predicate atIndex:(NSUInteger)elementIndex
{
    BOOL doesMatch = NO;
    
    //NSLog(@"element:doesMatchPredicate:atIndex:");
    //NSLog(@"element: %@ \ndoesMatchPredicate: %@ \natIndex:%lu", uiElement, predicate, elementIndex);
    
    if (!predicate) {
        // A nil predicate matches everything.
        //NSLog(@"element:doesMatchPredicate:atIndex: RETURNS YES BY DEFAULT");
        return YES;
    }
    
    NSString *booleanOperator = predicate[kPredicateBooleanOperator];
    NSArray *operations = predicate[kPredicateOperations];
    
    for (NSDictionary *operation in operations) {
        //NSLog(@"operation: %@", operation);
        if (operation[kPredicateAttributeName] != nil) {
            // A name-value pair is available. Try to match by string.
            NSString *attributeName = operation[kPredicateAttributeName];
            NSString *attributeValue = operation[kPredicateAttributeValue];
            if ([uiElement existsAttribute:attributeName]) {
                id elementAttributeValue = [uiElement valueForAttribute:attributeName];
                //NSLog(@"attributeName: %@ attributeValue: %@ elementAttributeValue: %@", attributeName, attributeValue, elementAttributeValue);
                
                // In our universe, an empty string is equal to a nil uiElement attribute value.
                if ([operation[kPredicateComparisonOperation] isEqualToString:kPredicateEQUALS]) {
                    doesMatch = (elementAttributeValue != nil && 
                                 [elementAttributeValue isKindOfClass:[NSString class]] && 
                                 [elementAttributeValue isEqualToString:attributeValue]) 
                    || (elementAttributeValue == nil && [@"" isEqualToString:attributeValue]);
                } else if ([operation[kPredicateComparisonOperation] isEqualToString:kPredicateNOTEQUALS]) {
                    doesMatch = (elementAttributeValue != nil && 
                                 [elementAttributeValue isKindOfClass:[NSString class]] && 
                                 ![elementAttributeValue isEqualToString:attributeValue]) 
                    || (elementAttributeValue == nil && ![@"" isEqualToString:attributeValue]);
                }
                
            } else {
                //NSLog(@"attributeName: %@ DOES NOT EXIST on this element", attributeName);
                doesMatch = NO;
            }
            
            if (doesMatch) {
                if ([booleanOperator isEqualToString:kPredicateOR] || [booleanOperator isEqualToString:kPredicateNone]) {
                    // We only need one YES for || or None, and here it is.
                    break;
                } else {
                    // Proceed through the rest of the operations.
                    continue;
                }
            } else {
                if ([booleanOperator isEqualToString:kPredicateAND] || [booleanOperator isEqualToString:kPredicateNone]) {
                    // We only need one NO for && or None, and here it is.
                    break;
                } else {
                    // Proceed through the rest of the operations.
                    continue;
                }
            }
        } else if (operation[kPredicateIndex] != nil) {
            // An index value is available. Try to match by index.
            NSNumber *index = operation[kPredicateIndex];
            if (index && [index isKindOfClass:[NSNumber class]] && [index unsignedIntegerValue] == elementIndex) {
                doesMatch = YES;
            }
            break;
        }
    }
    
    //NSLog(@"element:doesMatchPredicate:atIndex: RETURNS %@", doesMatch?@"YES":@"NO");
    return doesMatch;
}


// Given a string containing an XPath role and predicate, parse it.
// This will handle simple absolute AXPath predicates defined above.
// Returns:  
//  nil: for an invalid string (e.g. bad predicate syntax).
//  NSDictionary: valid parsed predicate
//
// For example: @"AXMenuItem[@AXTitle='Whatever']":
//  key:    @"AXRole"           value:  e.g. @"AXMenuItem"
//  key:    @"predicateKey"     value:  e.g. @"AXTitle"
//  key:    @"predicateValue"   value:  e.g. @"Whatever"
//
// For example: @"AXMenuItem[5]":
//  key:    @"AXRole"           value:  e.g. @"AXMenuItem"
//  key:    @"predicateIndex"   value:  e.g. @(5)
//
// For example: @"AXCloseButton":
//  key:    @"AXRole"           value:  e.g. @"AXCloseButton"
- (NSDictionary *)parseRoleAndPredicateString:(NSString *)rpString
{
    NSMutableDictionary *pathNode = [@{} mutableCopy];
    
    // The first path component specifies a child of rootUIElement.
    // Is this a plain vanilla role, or does it contain a predicate?
    // Predicates are contained in braces, e.g. [@AXTitle='Window'].
    NSArray *roleAndPredicate = [rpString componentsSeparatedByString:@"["];
    switch ([roleAndPredicate count]) {
        case 1:
            // Does not contain "[" so it is plain vanilla AXRole.
            pathNode[kNodeRole] = [roleAndPredicate objectAtIndex:0];
            break;
        case 2:
        {
            // This contains a role and a predicate, so parse the predicate.
            // If parsing fails then it is an error.
            pathNode[kNodeRole] = [roleAndPredicate objectAtIndex:0];
            NSString *predicate = [roleAndPredicate objectAtIndex:1];
            predicate = [predicate stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"]"]];
            NSDictionary *parsedPredicate = [self parsePredicate:predicate];
            if (parsedPredicate) {
                pathNode[kNodePredicate] = parsedPredicate;
            } else {
                pathNode = nil;
            }
        }
            break;
            
        default:
             pathNode = nil;
    }
    return pathNode;
}

// Predicate can have 0 or more &&, or, 0 or more ||. Mixing && and || is not supported.
/*
 @"@AXTitle='Window'"
 @"@AXTitle='Window'"&&@AXFullScreen='YES'
 */
// The predicate must be something like @"@AXTitle='Window'", always with an "=" sign.
- (NSDictionary *)parsePredicate:(NSString *)predicate
{
    //NSLog(@"parsePredicate: %@", predicate);
    
    NSMutableDictionary *predicateDictionary = [@{} mutableCopy];
    
    // Check for && and ||. They are mutually exclusive.
    predicateDictionary[kPredicateBooleanOperator] = kPredicateNone;
    NSArray *operationStrings = [predicate componentsSeparatedByString:kPredicateAND];
    if ([operationStrings count] == 1) {
        //There was no AND operator. Try the OR operator. If neither, then there was only one operation.
        operationStrings = [predicate componentsSeparatedByString:kPredicateOR];
        if ([operationStrings count] > 1) {
            predicateDictionary[kPredicateBooleanOperator] = kPredicateOR;
        }
    } else {
        predicateDictionary[kPredicateBooleanOperator] = kPredicateAND;
    }
    
    if (![operationStrings count]) {
        return @{};
    }
    
    // Parse an array of operation strings for this predicate.
    // Each string must be one of:
    //      1. an equality test, eg "@AXTitle='Calculator'"
    //      2. an inequality test, eg "@AXMenuItemMarkChar!=''"
    //      3. an index, e.g. 4.
    // Our caller will evaluate the array using the specified boolean operator. 
    // Each operation string will be parsed into a dictionary with either one of these examples:
    //  With 1 string in the array it can be either a numeric index or name-value pair:
    //      { kPredicateIndex: 4 }
    //      { kPredicateAttributeName: @"AXTitle", kPredicateAttributeValue: @"Calculator"}
    //  With 2 or more strings, it must be a name-value pair:
    //      { kPredicateAttributeName: @"AXTitle", kPredicateAttributeValue: @"Calculator"}
    // If the operation is an index, then the predicate can not have only one operation.
    NSMutableArray *predicateOperations = [@[] mutableCopy];
    
    for (NSString *operationString in operationStrings) {
        //NSLog(@"operationString: %@", operationString);
        NSMutableDictionary *operationDict = [@{} mutableCopy];

        operationDict[kPredicateComparisonOperation] = kPredicateNone;
        NSArray *terms = [operationString componentsSeparatedByString:kPredicateNOTEQUALS];
        if ([terms count] > 1) {
            operationDict[kPredicateComparisonOperation] = kPredicateNOTEQUALS;
        } else {
            //There was no NOTEQUALS operator. Try the EQUALS operator. If neither, then it might be an index.
            terms = [operationString componentsSeparatedByString:kPredicateEQUALS];
            if ([terms count] > 1) {
                operationDict[kPredicateComparisonOperation] = kPredicateEQUALS;
            }
        }
        switch ([terms count]) {
            case 1: 
            {
                // This is an operation that does not contain an operator, so it could be an index.
                // There must be only one operation in this predicate if it is an index value.
                if ([operationStrings count] == 1) {
                    NSNumberFormatter *f = [[NSNumberFormatter alloc] init];
                    f.numberStyle = NSNumberFormatterDecimalStyle;
                    NSNumber *indexValue = [f numberFromString:predicate];
                    if (indexValue) {
                        // It is an index. The terms will not include a name-value pair. 
                        operationDict[kPredicateIndex] = indexValue;
                    } else {
                        // It wasn't an index. Invalid comparison terms. 
                        operationDict = nil;
                    }
                } else {
                    // If more than one operation, all have to be a name-value pair.
                    operationDict = nil;
                }
            }
                break;
                
            case 2:
            {
                // This is a key-value pair without the enclosing braces, e.g. @"@AXTitle='My Window'".
                // Remove leading "@" in the attribute name.
                // Note: this character is removed as a side effect of using a boolean operator. 
                NSString *attributeName = [[terms objectAtIndex:kPredicateLeftOperand] stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"@"]];
                operationDict[kPredicateAttributeName] = attributeName;
                
                // Remove the surrounding single quotes for the attribute value.
                NSString *attributeValue = [[terms objectAtIndex:kPredicateRightOperand] stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"'"]];
                operationDict[kPredicateAttributeValue] = attributeValue;
            }
                break;
                
            default:
                // Operation can only have a left side a a right side.
                operationDict = nil;
        }
        if (operationDict) {
            [predicateOperations addObject:operationDict];
        } else {
            [predicateOperations removeAllObjects];
            break;
        }
    }
    
    if ([predicateOperations count] == 0) {
        // A predicate must contain at least one comparison. If only one comparison, it can be a name-value pair, or an index.
        //NSLog(@"parsePredicate: RETURNS NIL BECAUSE NO PREDICATE OPERATIONS");
        return nil;
    }
    
    predicateDictionary[kPredicateOperations] = predicateOperations;
    //NSLog(@"parsePredicate: predicateDictionary: %@", predicateDictionary);
    return predicateDictionary;
}


// Sublasses can override this method with more subtle logic.
- (BOOL)isPageLoaded
{
    return YES;
}

#pragma mark - Mouse Control

// Utility method for post_moveto:.

- (AppiumMacHTTPJSONResponse *)moveMouseInsideSandbox:(NSDictionary *)commandParams statusCode:(int *)statusCode
{
    // A sandboxed application can not move the mouse. 
    return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownCommand session:self.sessionId];
}

/*
 Execute a moveto RemoteWebDriver command. Possible commandParams are:
    elementObject: The element object based on the 'element' id. 
        Ignored if commandParams include absolute coordinates.
        Optional if offset is specified (see below).
        Required if offset is not specified. Will move to the center of the element.
    xoffset and yoffset: Optional for absolute coordinates, or if element is specified, otherwise required.
        The offset can have one of several interpretations:
        1. If element is not specified:
            a. Move as an offset from current mouse position.
            b. For the first moveto in a session, move as an offset from {0,0}.
        2. If element is specified:
            a. Move as an offset from the top-left corner of the element.
            b. SPECIAL CASE, NON-STANDARD: if element is the id of the AXApplication, then interpret offsets 
                as global display coordinates, where {0,0} is top-left of the main screen.
 */
- (AppiumMacHTTPJSONResponse *)moveMouseOutsideSandbox:(NSDictionary *)commandParams statusCode:(int *)statusCode
{
    //NSLog(@"moveMouseOutsideSandbox:%@", commandParams);
    // An app running outside the sandbox can move the mouse.
    // However, there may be some other reason not to support native events.
    if (!self.nativeEventSupport) {
        return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownCommand session:self.sessionId];
    }
    
    CGPoint globalEndingLocation = [self getGlobalDisplayPointWithCommandParams:commandParams];
    if (CGPointEqualToPoint(globalEndingLocation, [Utility invalidPoint])) {
        // Bad parameters.
        return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownError session:self.sessionId];
    }
    
    self.lastGlobalMouseLocation = [self moveMouseToScreenGlobalPoint:globalEndingLocation];
    
    return [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:self.sessionId];
}

// Utility method, generic so it can be used in more places than just moveto commands. 
// endingPoint: the global screen ending point for the mouse will stop. 
// With global screen coordinates, (0.0, 0.0) is the upper left corner of the main screen.
// This is different from NSPoint/NSView/NSWindow, which use cartesian coordinates.
// With cartisian coordinates, (0.0, 0.0) is the lower left corner of the view/window.
// IMPORTANT: It is the responsibility of the caller to derive endingPoint from other coordinate system(s).
// Returns the screen global position of the mouse after moving.
- (CGPoint)moveMouseToScreenGlobalPoint:(CGPoint)endingPoint;
{
    //NSLog(@"moveMouseToScreenGlobalPoint begin:{%f, %f}", endingPoint.x, endingPoint.y);
    if (CGPointEqualToPoint(endingPoint, [Utility invalidPoint])) {
        return [Utility invalidPoint];
    }
    
    // Inside this method, all coordinates are screen global.
    CGEventRef nullEvent = CGEventCreate(NULL);
    CGPoint startingPoint = CGEventGetLocation(nullEvent);
    CFRelease(nullEvent);
    
    CGFloat xTotalOffset = endingPoint.x - startingPoint.x;
    CGFloat yTotalOffset = endingPoint.y - startingPoint.y;
    
    // Proper mouse simulation requires multiple partial move events, not just one instant move.
    // Calculate an array of global points, in order to match the desired mouse speed in points per second.
    
    // Figure out how long it will take based on total distance.
    CGFloat totalPoints = hypotf(xTotalOffset, yTotalOffset);
    NSTimeInterval totalTime = totalPoints / self.mouseMoveSpeed;
    
    // Figure out how many times around the loop we will need. Divide total offsets to get per-loop offsets.
    // Start with a minimum loop count to slow down short moves, without affecting longer moves.
    float floatCount = totalTime / kMouseMoveEventDelay;
    
    CGFloat xOffsetPerLoop = xTotalOffset / floatCount;
    CGFloat yOffsetPerLoop = yTotalOffset / floatCount;
    
    // If the left mouse button is down, then drag instead of move.
    CGEventType mouseEventType = kCGEventMouseMoved;
    CGMouseButton mouseButton = kCGMouseButtonLeft;
    if (self.isLeftMouseDown) {
        mouseEventType = kCGEventLeftMouseDragged;
        mouseButton = kCGMouseButtonLeft;
    } else if (self.isRightMouseDown) {
        mouseEventType = kCGEventRightMouseDragged;
        mouseButton = kCGMouseButtonRight;
    } else if (self.isOtherMouseDown) {
        mouseEventType = kCGEventOtherMouseDragged;
        mouseButton = kCGMouseButtonCenter;
    }
    
    // Create an array of CGEvents to move (or drag) the mouse.
    CGPoint newMouseLocation = startingPoint;
    NSUInteger loopCount = floorf(floatCount);
    for (int loopIteration = 1; loopIteration <= loopCount; loopIteration++) {
        newMouseLocation.x = newMouseLocation.x + xOffsetPerLoop;
        newMouseLocation.y = newMouseLocation.y + yOffsetPerLoop;

        CGEventRef mouseEvent = CGEventCreateMouseEvent (NULL, mouseEventType, newMouseLocation, mouseButton);
        NSValue *eventValue = [NSValue valueWithPointer:mouseEvent];
        
        [self.timedEvents addObject:eventValue];
    }
    
    // One more move to the exact ending location.
    CGEventRef mouseEvent = CGEventCreateMouseEvent (NULL, mouseEventType, endingPoint, mouseButton);
    CGEventPost(kCGHIDEventTap, mouseEvent);    
    NSValue *eventValue = [NSValue valueWithPointer:mouseEvent];
    [self.timedEvents addObject:eventValue];
    
    //NSLog(@"moveMouseToScreenGlobalPoint self.timedEvents.count:%li", self.timedEvents.count);

    self.timedEventsCompleted = dispatch_semaphore_create(0);
    
    // Using withObject:nil will automagically create the timer that will perform the selector.
    [self performSelectorOnMainThread:@selector(processTimedMouseEvent:) withObject:nil waitUntilDone:NO];
    
    dispatch_semaphore_wait(self.timedEventsCompleted, DISPATCH_TIME_FOREVER);
    
    nullEvent = CGEventCreate(NULL);
    CGPoint globalFinalLocation = CGEventGetLocation(nullEvent);
    CFRelease(nullEvent);
    
    //NSLog(@"moveMouseToScreenGlobalPoint ended:{%f, %f}", globalFinalLocation.x, globalFinalLocation.y);
    return globalFinalLocation;
}

// This is called on the main thread.
- (void)processTimedMouseEvent:(NSTimer *)timer
{
    if (timer == nil && self.timedEventTimer == nil) {
        // Automagically create the timer that will call this method.
        self.timedEventTimer = [NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:_cmd userInfo:nil repeats:YES];
    }
    
    if (self.timedEventIndex >= [self.timedEvents count]) {
        // All the events have been posted. Stop the timer, and clean up.
        [timer invalidate];
        self.timedEventTimer = nil;
        
        // Allow the thread that created the timer to continue.
        dispatch_semaphore_signal(self.timedEventsCompleted);
        return;
    }
    
    NSValue *eventValue = [self.timedEvents objectAtIndex:self.timedEventIndex];
    CGEventRef mouseEvent = (CGEventRef)[eventValue pointerValue];
    CGEventPost(kCGHIDEventTap, mouseEvent);        
    
    // The eventValue contains this CFEventRef as a void* so it will not automatically release it.
    CFRelease(mouseEvent);
    
    self.timedEventIndex++;
}

- (CGPoint)convertCartisionPointToGlobal:(NSPoint)local
{
    NSScreen *screen = [NSScreen mainScreen];
    CGPoint global = {
        .x = [screen frame].origin.x + local.x,
        .y = [screen frame].origin.y + local.y,
    };
    return global;
}

- (NSPoint)convertGlobalPointToCartesian:(CGPoint)global
{
    NSScreen *screen = [NSScreen mainScreen];
    NSPoint local = {
        .x = global.x - [screen frame].origin.x,
        .y = global.y - [screen frame].origin.y,
    };
    return local;
}


// Utility method for post_click:, post_buttondown:, post_buttonup:, post_doubleclick:.
- (AppiumMacHTTPJSONResponse *)mouseButtonActionInsideSandbox:(NSString *)commandName commandParams:(NSDictionary *)commandParams statusCode:(int *)statusCode
{
    // A sandboxed application can not press mouse buttons.
    return [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownCommand session:self.sessionId];
}

// Utility method for post_click:, post_buttondown:, post_buttonup:, post_doubleclick:.
- (AppiumMacHTTPJSONResponse *)mouseButtonActionOutsideSandbox:(NSString *)commandName commandParams:(NSDictionary *)commandParams statusCode:(int *)statusCode
{
    // An app running outside the sandbox can press and hold mouse buttons.
    
    BOOL result = YES;
    CGEventType mouseDownType = kCGEventLeftMouseDown;
    CGEventType mouseUpType = kCGEventLeftMouseUp;
    CGMouseButton mouseButton = kCGMouseButtonLeft;
    NSNumber *number = [commandParams objectForKey:@"button"];
    if (number) {
        WebDriverMouseButtonType webDriverButton = [number unsignedIntegerValue];
        switch (webDriverButton) {
            case webDriverLeftButton:
                mouseDownType = kCGEventLeftMouseDown;
                mouseUpType = kCGEventLeftMouseUp;
                mouseButton = kCGMouseButtonLeft;
                break;
            case webDriverMiddleButton:
                mouseDownType = kCGEventOtherMouseDown;
                mouseUpType = kCGEventOtherMouseUp;
                mouseButton = kCGMouseButtonCenter;
                break;
            case webDriverRightButton:
                mouseDownType = kCGEventRightMouseDown;
                mouseUpType = kCGEventRightMouseUp;
                mouseButton = kCGMouseButtonRight;
                break;
        }
    }
    
    // get the current mouse location
    CGEventRef mouseLocEvent = CGEventCreate(NULL);
    CGPoint currentMouseLocation = CGEventGetLocation(mouseLocEvent);
    CFRelease(mouseLocEvent);
    
    CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
    
    CGEventRef mouseEvent = nil;
    if ([commandName hasSuffix:@"click"]) {
        // click and doubleclick
        mouseEvent = CGEventCreateMouseEvent (NULL, mouseDownType, currentMouseLocation, mouseButton);        
        int count = 1;
        if ([commandName isEqualToString:@"doubleclick"]) {
            count = 2;
        }
        for (int clickState = 1; clickState <= count; clickState++) {
            CGEventSetIntegerValueField(mouseEvent, kCGMouseEventClickState, clickState);
            CGEventSetType(mouseEvent, mouseDownType);  
            CGEventPost(kCGHIDEventTap, mouseEvent);
            CGEventSetType(mouseEvent, mouseUpType);  
            CGEventPost(kCGHIDEventTap, mouseEvent);  
        }
        CFRelease(mouseEvent);
    } else if ([commandName isEqualToString:@"buttondown"]) {
        mouseEvent = CGEventCreateMouseEvent (NULL, mouseDownType, currentMouseLocation, mouseButton);
        CGEventPost(kCGHIDEventTap, mouseEvent);
        CFRelease(mouseEvent);
        switch (mouseDownType) {
            case kCGEventLeftMouseDown:
                self.isLeftMouseDown = YES;
                break;
            case kCGEventRightMouseDown:
                self.isRightMouseDown = YES;
                break;
            case kCGEventOtherMouseDown:
                self.isOtherMouseDown = YES;
                break;
            default:
                self.isLeftMouseDown = YES;
                break;
        }
    } else if ([commandName isEqualToString:@"buttonup"]) {
        mouseEvent = CGEventCreateMouseEvent (NULL, mouseUpType, currentMouseLocation, mouseButton);
        CGEventPost(kCGHIDEventTap, mouseEvent);
        CFRelease(mouseEvent);
        switch (mouseUpType) {
            case kCGEventLeftMouseUp:
                self.isLeftMouseDown = NO;
                break;
            case kCGEventRightMouseUp:
                self.isRightMouseDown = NO;
                break;
            case kCGEventOtherMouseUp:
                self.isOtherMouseDown = NO;
                break;
            default:
                self.isLeftMouseDown = NO;
                break;
        }
    }
    
    CFRelease(source);
    
    return result ? [AppiumMacHTTPJSONResponse responseWithJson:nil status:kAfMStatusCodeSuccess session:self.sessionId] :
    [AppiumMacHTTPJSONResponse responseWithJsonError:kAfMStatusCodeUnknownError session:self.sessionId];
}

/*
 Returns the calculated globalDisplayPoint using commandParams from a moveto-like command.
 If commandParams are invalid, returns invalidPoint.
 */
- (HIPoint)getGlobalDisplayPointWithCommandParams:(NSDictionary *)commandParams
{
    // NSLog(@"getGlobalDisplayPointWithCommandParams:%@", commandParams);
    
    // The moveMouse methods use a global point, whereas moveto commandParams contain an element and/or offset.
    HIPoint globalDisplayPoint = [Utility invalidPoint];
    
    // Default if there is no element.
    HIPoint globalStartingLocation = self.lastGlobalMouseLocation;
    
    HIPoint elementPoint = [Utility invalidPoint];
    id elementObject = [commandParams objectForKey:@"elementObject"];
    //NSLog(@"getGlobalDisplayPointWithCommandParams elementObject:%@", elementObject);
    
    // The JsonWireProtocol defines 'xoffset' and 'yoffset' from the current mouse location, or (0, 0).
    // However, for flexibility a custom client can send 'xabsolute' and 'yabsolute'.
    // Prefer absolute over offset if both are included.
    if ([commandParams objectForKey:@"xabsolute"] && [commandParams objectForKey:@"yabsolute"]) {
        // Absolute coordinates in Selenium are screeen-based, so {0,0} is the upper left corner of the main screen.
        float xabsolute = [[commandParams objectForKey:@"xabsolute"] floatValue];
        float yAbsolute = [[commandParams objectForKey:@"yabsolute"] floatValue];
        globalDisplayPoint = CGPointMake(xabsolute, yAbsolute);
    } else if ([commandParams objectForKey:@"xoffset"] && [commandParams objectForKey:@"yoffset"]) {
        // Offset coordinates in Selenium are either element-based, where , so {0,0} is the upper left corner of the main screen.
        float xOffsetValue = [[commandParams objectForKey:@"xoffset"] floatValue];
        float yOffsetValue = [[commandParams objectForKey:@"yoffset"] floatValue];
        //NSLog(@"getGlobalDisplayPointWithCommandParams xOffsetValue:%f", xOffsetValue);
        //NSLog(@"getGlobalDisplayPointWithCommandParams yOffsetValue:%f", yOffsetValue);
        
        if (elementObject) {
            elementPoint = [self getGlobalDisplayPoint:elementPointUpperLeft forElement:elementObject];
            if (!CGPointEqualToPoint(elementPoint, [Utility invalidPoint])) {
                // The starting point is the upper left corner of the element.
                globalStartingLocation = elementPoint;
            }
        } else {
            //NSLog(@"getGlobalDisplayPointWithCommandParams NO ELEMENT OBJECT!");
        }
        globalDisplayPoint = CGPointMake(globalStartingLocation.x + xOffsetValue, globalStartingLocation.y + yOffsetValue);
    } else {
        if (elementObject) {
            elementPoint = [self getGlobalDisplayPoint:elementPointCenter forElement:elementObject];
            if (!CGPointEqualToPoint(elementPoint, [Utility invalidPoint])) {
                // The starting point is the upper left corner of the element.
                globalDisplayPoint = elementPoint;
            }
        } else {
            //NSLog(@"getGlobalDisplayPointWithCommandParams NO OFFSET AND NO ELEMENT OBJECT!");
        }
    }
    
    return globalDisplayPoint;
}

// Given a PFUIElement with rect in cartesian coordinates, return a point in global screen coordinates,
// where {0,0} is the upper left corner of the main screen.
- (HIPoint)getGlobalDisplayPoint:(ElementPointLocation)whichPoint forElement:(id)element
{
    HIPoint returnedPoint = [Utility invalidPoint];
    
    if ([element isKindOfClass:[PFUIElement class]]) {
        //NSLog(@"getGlobalDisplayPoint:forElement: returnedPoint element: %@", [element debugDescription]);
        // Special case: the AXApplication represents a global display point of {0,0}, so any "element offset" will equal a global display point. 
        if ([[element AXRole] isEqualToString:@"AXApplication"]) {
            //NSLog(@"getGlobalDisplayPoint:forElement: AXApplication");
            return CGPointMake(0.0, 0.0);
        }
        
        HIPoint elementUpperLeft = [[element AXPosition] pointValue];
        NSSize elementSize = [[element AXSize] sizeValue];
        switch (whichPoint) {
            case elementPointCenter:
                returnedPoint = NSMakePoint(elementUpperLeft.x + 0.5 * elementSize.width, elementUpperLeft.y + 0.5 * elementSize.height);
                break;
                
            case elementPointUpperLeft:
                returnedPoint = elementUpperLeft;
                break;
                
            default:
                break;
        }
        //NSLog(@"getGlobalDisplayPoint:forElement: returnedPoint before convert %@", NSStringFromPoint(returnedPoint));
        
        returnedPoint = [self convertCartisionPointToGlobal:returnedPoint];
        //NSLog(@"getGlobalDisplayPoint:forElement: returnedPoint after  convert %@", NSStringFromPoint(returnedPoint));
    }
    
    return returnedPoint;
}

#pragma mark - Screen Capture Utilities
// Utility method for get_screenshot, and for generic screen shots outside of a WebDriver command handler.
// Screenshot files are created in the diagnosticsDirectory with the given file name.
// Optionally append a date stamp.
- (void)screenCaptureToFile:(NSString *)fileName withDateStamp:(BOOL)appendDateStamp
{
    if (fileName && self.diagnosticsDirectory) {
        if (appendDateStamp) {
            // E.g. fileName__2016-02-23_01_34_22_+0000
            NSString *dateString = [[[NSDate date] description] stringByReplacingOccurrencesOfString:@" " withString:@"_"];
            dateString = [dateString stringByReplacingOccurrencesOfString:@":" withString:@"_"];
            fileName = [NSString stringWithFormat:@"%@__%@", fileName, dateString];
        }

        NSString *filePath = [NSString stringWithFormat:@"%@/%@.png", self.diagnosticsDirectory, fileName];

        NSLog(@"Creating screenshot: %@", filePath);
        system([[NSString stringWithFormat:@"/usr/sbin/screencapture -mx %@", filePath] cStringUsingEncoding:NSASCIIStringEncoding]);
    }
}

// Utility method for get_screenshot, and for generic screen shots outside of a WebDriver command handler.
// If imageDirectory is nil, the image is saved to the clipboard.
// Returns YES if the clipboard contains an NSImage, otherwise NO.
- (BOOL)screenCaptureToClipboard
{
    system([@"/usr/sbin/screencapture -c" UTF8String]);
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    return [pasteboard canReadObjectForClasses:@[[NSImage class]] options:@{}];
}

#pragma mark - Notifications
- (void)fnKeyTrigger:(NSNotification *)notification
{
    NSLog(@"fnKeyTrigger: session isCanceled set YES: %@", self.sessionId);
    self.isCanceled = YES;
}

#pragma mark - Diagnostics

// This method validates the directory, and creates a per-session directory.
// If globalDirectory is nil, or there are errors creating directories, then diagnostic file saving is disabled.
- (void)setGlobalDiagnosticsDirectory:(NSString *)globalDirectoryPath
{
    [self setCookie: @{@"name": kCookieGlobalDiagnosticsDirectory, @"value": @""}];
    
    if (!globalDirectoryPath) {
        // Disable all diagnostic files. This is not the default, but the Selenium client can request this.
        NSLog(@"Diagnostic files are disabled because globalDirectoryPath was nil.");
        self.diagnosticsDirectory = nil;
        return;
    }
        
    // Validate the global directory by trying to create the per-session directory.
    if (![self createDiagnosticsDirectoryRelativeTo:globalDirectoryPath]) {
        NSLog(@"Diagnostic files are disabled because could not create per-session directory.");
    }
}

// Create a directory for _this_ session's diagnostic files, inside a given directory.
- (BOOL)createDiagnosticsDirectoryRelativeTo:(NSString *)directoryPath
{
    BOOL success = NO;
    
    NSString *sessionDirectoryName = [NSString stringWithFormat:@"session-%@", self.sessionId];
    NSString *sessionPath = [directoryPath stringByAppendingString:sessionDirectoryName];
    
    NSError *error = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath:sessionPath withIntermediateDirectories:YES attributes:nil error:&error];
    if (error) {
        NSLog(@"createDirectoryAtPath:%@ error:%@", sessionPath, error);
    } else {
        self.diagnosticsDirectory = sessionPath;
        success = YES;
    }
    return success;
}

- (void)setGlobalDiagnosticsDirectoryCookie:(NSDictionary *)cookie
{
    if ([cookie[@"name"] isEqualToString:kCookieGlobalDiagnosticsDirectory]) {
        // Set the top level directory containing diagnostic files for _all_ sessions.
        NSString *path = cookie[@"value"];
        path = [path stringByExpandingTildeInPath];
        if (path && [path isKindOfClass:[NSString class]] && path.length > 0) {
            if ([path rangeOfString:@"/" options:NSBackwardsSearch].location < path.length - 1) {
                path = [path stringByAppendingString:@"/"];
            }
            if ([path rangeOfString:@"AppiumDiagnostics"].location == NSNotFound) {
                path = [path stringByAppendingString:@"AppiumDiagnostics/"];
            }
            // Capability was requested and a non-empty string. Use this value.
            [self setGlobalDiagnosticsDirectory:path];
        }
    }
}

@end

