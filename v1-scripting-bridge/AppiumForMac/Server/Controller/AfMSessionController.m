//
//  AppiumMacAppleScriptLibrary.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMSessionController.h"

#import "AfMDomElement.h"
#import "Utility.h"

@implementation AfMSessionController

- (id)init
{
    self = [super init];
    if (self) {
        [self setElementIndex:0];
        [self setElements:[NSMutableDictionary new]];
        [self setCurrentApplicationName:nil];
        [self setCurrentProcessName:nil];
		[self setCurrentWindowHandle:nil];
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
			nil]];
    }
    return self;
}

-(NSArray*) allProcessNames
{
    NSMutableArray *processes = [NSMutableArray new];
    for(SystemEventsProcess *process in [self.systemEvents processes])
    {
        [processes addObject:[process name]];
    }
    return processes;
}

-(NSArray*) allWindowHandles
{
    NSMutableArray *windowHandles = [NSMutableArray new];
    SBElementArray *allWindows = self.currentProcess.windows;
    for(int i=0; i < allWindows.count; i++)
    {
        SystemEventsWindow *currentWindow = [allWindows objectAtIndex:i];
        BOOL duplicateOfName = NO;
        for (int j = i+1; j < allWindows.count; j++)
        {
            if ( [[(SystemEventsWindow*)[allWindows objectAtIndex:j] name] isEqualToString:[currentWindow name]])
            {
                duplicateOfName = YES;
                break;
            }
        }
        NSString *currentWindowHandle = duplicateOfName ? [NSString stringWithFormat:@"%d", i+1] : [currentWindow name];
        [windowHandles addObject:currentWindowHandle];
    }
    return windowHandles;
}

-(void) activateApplication:(NSString*)applicationName
{
    if (![self.currentApplicationName isEqualToString:applicationName])
    {
        [self setCurrentWindowHandle:@"1"];
    }

    NSDictionary *errorDict;
    NSAppleScript *activateScript = [[NSAppleScript alloc] initWithSource:
									 [NSString stringWithFormat:@"tell application \"%@\" to activate", applicationName]];
    [activateScript executeAndReturnError:&errorDict];

    // TODO: Add error handling
    // TODO: convert to scripting bridge
}

-(void) activateWindow:(NSString*)windowHandle forProcessName:(NSString*)processName
{
    // activate application for supplied process
    NSDictionary *errorDict;
    NSAppleScript *activateWindowScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\" to tell process \"%@\" to perform action \"AXRaise\" of window %@", processName, windowHandle]];
    [activateWindowScript executeAndReturnError:&errorDict];
    [self setCurrentWindowHandle:windowHandle];
	
    // TODO: error handling
    // TODO: convert to scripting bridge
}


-(NSString*) applicationForProcessName:(NSString*)processName
{
    NSDictionary *errorDict;
    NSAppleScript *appForProcNameScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset process_bid to get the bundle identifier of process \"%@\"\nset application_name to file of (application processes where bundle identifier is process_bid)\nend tell\nreturn application_name", processName]];
    NSString *statusString = [[appForProcNameScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return statusString;
}


-(void) clickElement:(SystemEventsUIElement*)element
{
    [element clickAt:nil];
    // TODO: error handling
    // TODO: check if element is enabled (clickable)
}

-(void) closeWindow:(NSString*)windowHandle forProcessName:(NSString*)processName
{
    for (SystemEventsWindow *window in [[self processForName:processName] windows])
    {
		SystemEventsWindow *window = [self windowForHandle:windowHandle forProcess:processName];
        if (window != nil)
        {
            [window closeSaving:SystemEventsSavoNo savingIn:nil];
            [self setCurrentWindowHandle:@"1"];
            return;
        }
    }
}

-(SystemEventsProcess*) currentProcess
{
    return [self processForName:self.currentProcessName];
}

-(SystemEventsUIElement*) elementByName:(NSString*)name baseElement:(SystemEventsUIElement*)baseElement
{
    // check if this the element
    if (baseElement != nil && [baseElement.title isEqualToString:name])
    {
        // return the element if it matches
        return baseElement;
    }

    // search the children
    NSArray *elementsToSearch;
    if (baseElement != nil)
    {
        // search the children if this is an element
        elementsToSearch = baseElement.UIElements;
    }
    else
    {
        SystemEventsProcess *process = self.currentProcess;
        if (process != nil)
        {
            elementsToSearch = process.UIElements;
        }
    }

    if (elementsToSearch != nil)
    {
        for(SystemEventsUIElement* childElement in elementsToSearch)
        {
            // check the child
            SystemEventsUIElement *childResult = [self elementByName:name baseElement:childElement];

            //return the child if it matches
            if (childResult != nil)
            {
                return childResult;
            }
        }
    }

    // return nil because there was no match
    return nil;
}

-(NSString*) frontmostApplicationName
{
    NSDictionary *errorDict;
    NSAppleScript *frontMostApplicationScript = [[NSAppleScript alloc] initWithSource:
												 @"tell application \"Finder\"\nset appPath to the path to the frontmost application\nset appName to the name of file appPath\nset appName to text 1 thru ((offset of \".\" in appName) - 1) of appName\nend tell"];
    NSString *statusString = [[frontMostApplicationScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return statusString;
}

-(NSString*) frontmostProcessName
{
    return [self processNameForApplicationName:self.frontmostApplicationName];
}

-(NSString*) pageSource
{
	NSMutableArray *source = [NSMutableArray new];
    SystemEventsProcess *process = [self processForName:self.currentProcessName];
	for (SystemEventsUIElement* element in [process entireContents])
	{
		[source addObject:element.description];
	}
	NSDictionary *dom = [AfMDomElement pageSource:[source componentsJoinedByString:@"<<<APPIUM_DIVIDER>>>"]];

	NSError *error;
	NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dom
													   options:NSJSONWritingPrettyPrinted
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
    NSAppleScript *fronstMostProcessScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset application_id to (get the id of application \"%@\" as string)\nset process_name to name of (application processes where bundle identifier is application_id)\nend tell\nreturn item 1 of process_name as text", applicationName]];
    NSString *statusString = [[fronstMostProcessScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return statusString;
}

-(void) selectElement:(SystemEventsUIElement*)element
{
    [element select];
}

-(void) sendKeys:(NSString*)keys
{
    [self sendKeys:keys toElement:nil];
}

-(void) sendKeys:(NSString*)keys toElement:(SystemEventsUIElement*)element
{
    [self activateApplication:self.currentApplicationName];
    if (element != nil)
    {
        [self selectElement:element];
    }
    [self.systemEvents keystroke:keys using:0];
}

-(SystemEventsWindow*) windowForHandle:(NSString*)windowHandle forProcess:(NSString*)processName
{
    if ([windowHandle hasPrefix:@"\""])
    {
        // string handle
        windowHandle = [[windowHandle substringToIndex:windowHandle.length-2] substringFromIndex:1];
        for(SystemEventsWindow *window in [[self processForName:processName] windows])
        {
            if ([[window name] isEqualToString:windowHandle])
            {
                return window;
            }
        }
    }
    else
    {
        //TODO: actually parse the number from the handle
        SBElementArray *windows = [[self processForName:processName] windows];
        for (int i=0 ; i < windows.count; i++)
        {
            if ([[NSString stringWithFormat:@"%d", i+1] isEqualToString:windowHandle])
            {
                return [windows objectAtIndex:i];
            }
        }
    }
    return nil;
}

@end
