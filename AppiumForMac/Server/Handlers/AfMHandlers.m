//
//  AppiumMacController.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMHandlers.h"

#import "AfMElementLocator.h"
#import "AfMStatusCodes.h"
#import "AppiumMacHTTP303JSONResponse.h"
#import "NSData+Base64.h"
#import "Utility.h"
#import <PFAssistive/PFAssistive.h>

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

-(AppiumMacHTTPJSONResponse*) respondWithJsonError:(int)statusCode session:(NSString*)sessionId
{
	NSDictionary *errorJson = [NSDictionary dictionaryWithObjectsAndKeys:kAfMStatusCodeMessages[statusCode],@"message" , nil];
	return [self respondWithJson:errorJson status:statusCode session:sessionId];
}

-(int) checkElement:(PFUIElement*)element forSession:(AfMSessionController*)session
{
	if (element == nil)
	{
		return kAfMStatusCodeNoSuchElement;
	}
	if (![session.currentWindow exists])
	{
		return kAfMStatusCodeNoSuchWindow;
	}
	if (![element exists])
	{
		return kAfMStatusCodeStaleElementReference;
	}
	NSPoint middlePoint = [[element AXPosition] pointValue];
	NSSize elementSize = [[element AXSize] sizeValue];
	middlePoint.x += elementSize.width/2.0;
	middlePoint.y += elementSize.height/2.0;
	if (![element isVisibleAtPoint:middlePoint])
	{
		return kAfMStatusCodeElementNotVisible;
	}
	return kAfMStatusCodeSuccess;
}

// GET /status
-(AppiumMacHTTPJSONResponse*) getStatus:(NSString*)path
{
    NSDictionary *buildJson = [NSDictionary dictionaryWithObjectsAndKeys:[Utility bundleVersion], @"version", [Utility bundleRevision], @"revision", [NSString stringWithFormat:@"%d", [Utility unixTimestamp]], @"time", nil];
    NSDictionary *osJson = [NSDictionary dictionaryWithObjectsAndKeys:[Utility arch], @"arch", @"Mac OS X", @"name", [Utility version], @"version", nil];
    NSDictionary *json = [NSDictionary dictionaryWithObjectsAndKeys:buildJson, @"build", osJson, @"os", nil];
    return [self respondWithJson:json status:kAfMStatusCodeSuccess session:nil];
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

	AfMSessionController *session = [AfMSessionController new];
    [self.sessions setValue:session forKey:newSession];

    return [self respondWithJson:session.capabilities status:kAfMStatusCodeSuccess session: newSession];
}

// GET /sessions
-(AppiumMacHTTPJSONResponse*) getSessions:(NSString*)path
{
    // respond with the session
    // TODO: implement this correctly
    NSMutableArray *json = [NSMutableArray new];
    for(id key in self.sessions)
    {
        [json addObject:[NSDictionary dictionaryWithObjectsAndKeys:key, @"id", [self.sessions objectForKey:key], @"capabilities", nil]];
    }

    return [self respondWithJson:json status:kAfMStatusCodeSuccess session: nil];
}

// GET /session/:sessionId
-(AppiumMacHTTPJSONResponse*) getSession:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    // TODO: show error if session does not exist
    return [self respondWithJson:[self.sessions objectForKey:sessionId] status:kAfMStatusCodeSuccess session:sessionId];
}

// DELETE /session/:sessionId
-(AppiumMacHTTPJSONResponse*) deleteSession:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    if ([self.sessions objectForKey:sessionId] != nil)
    {
        [self.sessions removeObjectForKey:sessionId];
    }
    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// /session/:sessionId/timeouts
// /session/:sessionId/timeouts/async_script
// /session/:sessionId/timeouts/implicit_wait

// GET /session/:sessionId/window_handle
-(AppiumMacHTTPJSONResponse*) getWindowHandle:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
	if ([session.currentWindowHandle intValue] >= session.allWindows.count)
	{
		return [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
	}

	return [self respondWithJson:session.currentWindowHandle status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/window_handles
-(AppiumMacHTTPJSONResponse*) getWindowHandles:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    // TODO: add error handling
    return [self respondWithJson:session.allWindowHandles status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/url
-(AppiumMacHTTPJSONResponse*) getUrl:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    return [self respondWithJson:session.currentApplicationName status:kAfMStatusCodeSuccess session: sessionId];
}

// POST /session/:sessionId/url
-(AppiumMacHTTPJSONResponse*) postUrl:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    // activate supplied application
    NSString *url = (NSString*)[postParams objectForKey:@"url"];
	[session setCurrentApplicationName:url];
	[session activateApplication];

    // TODO: error handling
    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// /session/:sessionId/forward
// /session/:sessionId/back
// /session/:sessionId/refresh

// POST /session/:sessionId/execute
-(HTTPDataResponse*) postExecute:(NSString*)path data:(NSData*)postData
{
	NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    // run the script
    NSString *script = (NSString*)[postParams objectForKey:@"script"];
	id result = [session executeScript:script];

	// send back the result
	if ([result isKindOfClass:[NSAppleEventDescriptor class]])
	{
		NSString *resultText = [(NSAppleEventDescriptor*)result stringValue];
		if (resultText == nil)
		{
			resultText = @"";
		}
		return [self respondWithJson:[NSDictionary dictionaryWithObject:resultText forKey:@"result"] status:kAfMStatusCodeSuccess session: sessionId];
	}
	else
	{
		return [self respondWithJson:(NSDictionary*)result status:kAfMStatusCodeJavascriptError session: sessionId];
	}
}

// /session/:sessionId/execute_async

// GET /session/:sessionId/screenshot
-(HTTPDataResponse*) getScreenshot:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *frontmostAppWindow = [session frontmostAppWindowInfo];
    CGWindowID windowID = [[frontmostAppWindow objectForKey:@"kCGWindowNumber"] unsignedIntValue];
	CGImageRef screenShot = CGWindowListCreateImage(CGRectNull, kCGWindowListOptionIncludingWindow, windowID, kCGWindowImageBoundsIgnoreFraming);
    if (screenShot) {
        NSSize size = NSMakeSize(CGImageGetWidth(screenShot), CGImageGetHeight(screenShot));
        NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:screenShot];
        [bitmapRep setSize:size];
        NSData *pngData = [bitmapRep representationUsingType:NSPNGFileType properties:nil];
        NSString *base64Image = [pngData base64EncodedString];
        return [self respondWithJson:base64Image status:kAfMStatusCodeSuccess session:[Utility getSessionIDFromPath:path]];
    }else{
        return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: [Utility getSessionIDFromPath:path]];
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
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    // activate application for supplied process
    NSString *windowHandle = (NSString*)[postParams objectForKey:@"name"];
	if ([windowHandle intValue] >= session.allWindows.count)
	{
		return [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
	}
	
	[session setCurrentWindowHandle:windowHandle];
    [session activateWindow];

    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// DELETE /session/:sessionId/window
-(AppiumMacHTTPJSONResponse*) deleteWindow:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];

	// check that there is at least one window
	NSUInteger originalWindowCount = session.allWindows.count;
	if (originalWindowCount < 1)
	{
		return [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
	}

	// close the window
    [session closeWindow];
	return (session.allWindows.count < originalWindowCount) ? [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId] : [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
}

// POST /session/:sessionId/window/:windowHandle/size
-(AppiumMacHTTPJSONResponse*) postWindowSize:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
	NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
	PFUIElement *window = [session windowForHandle:windowHandle];

	if (windowHandle == nil)
	{
		return [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
	}

	NSDictionary *postParams = [self dictionaryFromPostData:postData];
    CGFloat width = [(NSNumber*)[postParams objectForKey:@"width"] floatValue];
	CGFloat height = [(NSNumber*)[postParams objectForKey:@"height"] floatValue];

	NSSize size = [window.AXSize sizeValue];
	size.width = width;
	size.height = height;

	[window setAXSize:[NSValue valueWithSize:size]];

    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/window/:windowHandle/size
-(AppiumMacHTTPJSONResponse*) getWindowSize:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
	NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
	PFUIElement *window = [session windowForHandle:windowHandle];

	if (windowHandle == nil)
	{
		return [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
	}

	NSSize size = [window.AXSize sizeValue];

    return [self respondWithJson:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:size.width], "width", [NSNumber numberWithFloat:size.height], @"height", nil] status:kAfMStatusCodeSuccess session: sessionId];
}

// POST /session/:sessionId/window/:windowHandle/position
-(AppiumMacHTTPJSONResponse*) postWindowPosition:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
	NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
	PFUIElement *window = [session windowForHandle:windowHandle];

	if (windowHandle == nil)
	{
		return [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
	}

	NSDictionary *postParams = [self dictionaryFromPostData:postData];
    CGFloat x = [(NSNumber*)[postParams objectForKey:@"x"] floatValue];
	CGFloat y = [(NSNumber*)[postParams objectForKey:@"y"] floatValue];

	NSPoint position = [window.AXPosition pointValue];
	position.x = x;
	position.y = y;

	[window setAXPosition:[NSValue valueWithPoint:position]];
	
    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/window/:windowHandle/position
-(AppiumMacHTTPJSONResponse*) getWindowPosition:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
	NSString *windowHandle = [Utility getItemFromPath:path withSeparator:@"window"];
	PFUIElement *window = [session windowForHandle:windowHandle];

	if (windowHandle == nil)
	{
		return [self respondWithJsonError:kAfMStatusCodeNoSuchWindow session:sessionId];
	}

	NSPoint position = [[window AXPosition] pointValue];

    return [self respondWithJson:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:position.x], "x", [NSNumber numberWithFloat:position.y], @"y", nil] status:kAfMStatusCodeSuccess session: sessionId];
}

// /session/:sessionId/window/:windowHandle/maximize
// /session/:sessionId/cookie
// /session/:sessionId/cookie/:name

// GET /session/:sessionId/source
-(AppiumMacHTTPJSONResponse*) getSource:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];

	// xml page source
	return [self respondWithJson:[[NSString alloc]initWithData:[session xmlPageSource].XMLData encoding:NSUTF8StringEncoding] status:kAfMStatusCodeSuccess session:sessionId];

	// json page source
	//return [self respondWithJson:[session pageSource] status:0 session: sessionId];
}

// /session/:sessionId/title

// POST /session/:sessionId/element
-(AppiumMacHTTPJSONResponse*) postElement:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    NSString *using = (NSString*)[postParams objectForKey:@"using"];
    NSString *value = (NSString*)[postParams objectForKey:@"value"];

	AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];

	// initialize status as though no element were found
	int statusCode = kAfMStatusCodeNoSuchElement;

	if (locator != nil)
	{
		PFUIElement *element = [locator findUsingBaseElement:nil statusCode:&statusCode];
        if (element != nil)
        {
            session.elementIndex++;
            NSString *myKey = [NSString stringWithFormat:@"%d", session.elementIndex];
            [session.elements setValue:element forKey:myKey];
            return [self respondWithJson:[NSDictionary dictionaryWithObject:myKey forKey:@"ELEMENT"] status:kAfMStatusCodeSuccess session:sessionId];
        }
	}
	
	return [self respondWithJsonError:statusCode session:sessionId];
}

// POST /session/:sessionId/elements
-(AppiumMacHTTPJSONResponse*) postElements:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    NSString *using = (NSString*)[postParams objectForKey:@"using"];
    NSString *value = (NSString*)[postParams objectForKey:@"value"];

	AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];

	// initialize status as though no element were found
	int statusCode = kAfMStatusCodeNoSuchElement;
	
	if (locator != nil)
	{
		NSMutableArray *matches = [NSMutableArray new];
		[locator findAllUsingBaseElement:nil results:matches statusCode:&statusCode];
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
			
			return [self respondWithJson:nil status:kAfMStatusCodeNoSuchElement session: sessionId];
        }
	}

    return [self respondWithJsonError:statusCode session:sessionId];
}

// /session/:sessionId/element/active
// /session/:sessionId/element/:id

// POST /session/:sessionId/element/:id/element
-(AppiumMacHTTPJSONResponse*) postElementInElement:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];
    NSString *elementId = [Utility getElementIDFromPath:path];
    PFUIElement *rootElement = [session.elements objectForKey:elementId];
    NSString *using = (NSString*)[postParams objectForKey:@"using"];
    NSString *value = (NSString*)[postParams objectForKey:@"value"];

	AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];

	// initialize status as though no element were found
	int statusCode = kAfMStatusCodeNoSuchElement;
	
	if (locator != nil)
	{
		PFUIElement *element = [locator findUsingBaseElement:rootElement statusCode:&statusCode];
        if (element != nil)
        {
            session.elementIndex++;
            NSString *myKey = [NSString stringWithFormat:@"%d", session.elementIndex];
            [session.elements setValue:element forKey:myKey];
            return [self respondWithJson:[NSDictionary dictionaryWithObject:myKey forKey:@"ELEMENT"] status:kAfMStatusCodeSuccess session:sessionId];
        }
	}

    return [self respondWithJsonError:statusCode session:sessionId];
}

// POST /session/:sessionId/element/:id/elements
-(AppiumMacHTTPJSONResponse*) postElementsInElement:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];
    NSString *elementId = [Utility getElementIDFromPath:path];
    PFUIElement *rootElement = [session.elements objectForKey:elementId];
    NSString *using = (NSString*)[postParams objectForKey:@"using"];
    NSString *value = (NSString*)[postParams objectForKey:@"value"];

	AfMElementLocator *locator = [AfMElementLocator locatorWithSession:session using:using value:value];

	// initialize status as though no element were found
	int statusCode = kAfMStatusCodeNoSuchElement;

	if (locator != nil)
	{
		NSMutableArray *matches = [NSMutableArray new];
		[locator findAllUsingBaseElement:rootElement results:matches statusCode:&statusCode];
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
			return [self respondWithJson:elements status:kAfMStatusCodeSuccess session:sessionId];
        }
	}

    return [self respondWithJsonError:statusCode session:sessionId];
}


// POST /session/:sessionId/element/:id/click
-(AppiumMacHTTPJSONResponse*) postElementClick:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	BOOL result = [session clickElement:element];

    return result ? [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId] :
	[self respondWithJsonError:kAfMStatusCodeUnknownError session:sessionId];
}

// /session/:sessionId/element/:id/submit

// GET /session/:sessionId/element/:id/text
-(AppiumMacHTTPJSONResponse*) getElementText:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}
	
	id valueAttribute = element.AXValue;
	if (valueAttribute != nil)
	{
		NSString *text = [NSString stringWithFormat:@"%@", valueAttribute];
			return [self respondWithJson:text status:kAfMStatusCodeSuccess session: sessionId];
	}

    return [self respondWithJson:nil status:kAfMStatusCodeUnknownError session: sessionId];
}

// POST /session/:sessionId/element/:id/value
-(AppiumMacHTTPJSONResponse*) postElementValue:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    NSArray *value = [postParams objectForKey:@"value"];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	// set the value
    [element setAXValue:[value componentsJoinedByString:@""]];

    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// POST /session/:sessionId/keys
-(AppiumMacHTTPJSONResponse*) postKeys:(NSString*)path data:(NSData*)postData
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSDictionary *postParams = [self dictionaryFromPostData:postData];

    NSArray *value = [postParams objectForKey:@"value"];
    [session sendKeys:[value componentsJoinedByString:@""] toElement:nil];

    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/element/:id/name
-(AppiumMacHTTPJSONResponse*) getElementName:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	return [self respondWithJson:element.AXTitle status:kAfMStatusCodeSuccess session: sessionId];
}

// POST /session/:sessionId/element/:id/clear
-(AppiumMacHTTPJSONResponse*) postElementClear:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	//id value = [element value];
    //if (value != nil && [value isKindOfClass:[NSString class]])
    //{
    //    [element setValue:@""];
    //}

    // TODO: Add error handling
    return [self respondWithJson:nil status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/element/:id/selected
-(AppiumMacHTTPJSONResponse*) getElementIsSelected:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	return [self respondWithJson:element.AXFocused status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/element/:id/enabled
-(AppiumMacHTTPJSONResponse*) getElementIsEnabled:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	return [self respondWithJson:element.AXEnabled status:kAfMStatusCodeSuccess session: sessionId];

}

// GET /session/:sessionId/element/:id/attribute/:name
-(AppiumMacHTTPJSONResponse*) getElementAttribute:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];
    NSString *attributeName = [Utility getItemFromPath:path withSeparator:@"/attribute/"];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}
	
	id attributeValue = [element valueForAttribute:attributeName];
		return [self respondWithJson:[NSString stringWithFormat:@"%@", attributeValue] status:0 session: sessionId];
}

// GET /session/:sessionId/element/:id/equals/:other
-(AppiumMacHTTPJSONResponse*) getElementIsEqual:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];
    NSString *otherElementId = [Utility getItemFromPath:path withSeparator:@"/equals/"];

	// get the first element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check that the first element is valid
	int status1 = [self checkElement:element forSession:session];
	if (status1 != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status1 session:sessionId];
	}

	// get the second element
    PFUIElement *otherElement = [session.elements objectForKey:otherElementId];

	// check that the second element is valid
	int status2 = [self checkElement:otherElement forSession:session];
	if (status2 != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status2 session:sessionId];
	}

    return [self respondWithJson:[NSNumber numberWithBool:[element isEqualToElement:otherElement]] status:kAfMStatusCodeSuccess session:sessionId];
}

// GET /session/:sessionId/element/:id/displayed
-(AppiumMacHTTPJSONResponse*) getElementDisplayed:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess && status != kAfMStatusCodeElementNotVisible)
	{
		[self respondWithJsonError:status session:sessionId];
	}
	BOOL visible = status != kAfMStatusCodeElementNotVisible;
	return [self respondWithJson:[NSNumber numberWithBool:visible] status:kAfMStatusCodeSuccess session: sessionId];
}

// GET /session/:sessionId/element/:id/location
-(AppiumMacHTTPJSONResponse*) getElementLocation:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	NSPoint point = [element.AXPosition pointValue];
	NSDictionary *position = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:point.x], @"x", [NSNumber numberWithFloat:point.y], @"y", nil];
	return [self respondWithJson:position status:kAfMStatusCodeSuccess session: sessionId];
}

// /session/:sessionId/element/:id/location_in_view


// GET /session/:sessionId/element/:id/size
-(AppiumMacHTTPJSONResponse*) getElementSize:(NSString*)path
{
    NSString *sessionId = [Utility getSessionIDFromPath:path];
    AfMSessionController *session = [self controllerForSession:sessionId];
    NSString *elementId = [Utility getElementIDFromPath:path];

	// get the element
    PFUIElement *element = [session.elements objectForKey:elementId];

	// check the element is valid
	int status = [self checkElement:element forSession:session];
	if (status != kAfMStatusCodeSuccess)
	{
		[self respondWithJsonError:status session:sessionId];
	}

	NSSize size = [element.AXSize sizeValue];
	NSDictionary *sizeDict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:size.width], @"width", [NSNumber numberWithFloat:size.height], @"height", nil];
	return [self respondWithJson:sizeDict status:kAfMStatusCodeSuccess session: sessionId];
}

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