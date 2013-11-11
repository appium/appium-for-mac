//
//  AppiumMacAppleScriptLibrary.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMSessionController.h"
#import "Utility.h"


@interface AfMSessionController()
@property NSString *_currentApplicationName;
@end

@implementation AfMSessionController

- (id)init
{

    self = [super init];
    if (self) {
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

-(NSArray*) allWindows
{
    return [[NSArray arrayWithObject:self.currentApplication] arrayByAddingObjectsFromArray:[self.currentApplication AXWindows]];
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

-(PFApplicationUIElement*) applicationForName:(NSString*)applicationName
{
	NSDictionary *errorDict;
    NSAppleScript *fronstMostProcessScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"get POSIX path of (path to application \"%@\")", applicationName]];
    NSString *statusString = [[fronstMostProcessScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return [PFApplicationUIElement applicationUIElementWithURL:[NSURL fileURLWithPath:statusString] delegate:nil];
	return nil;
}

-(NSString*) applicationNameForProcessName:(NSString*)processName
{
    NSDictionary *errorDict;
    NSAppleScript *appForProcNameScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset process_bid to get the bundle identifier of process \"%@\"\nset application_name to file of (application processes where bundle identifier is process_bid)\nend tell\nreturn application_name", processName]];
    NSString *statusString = [[appForProcNameScript executeAndReturnError:&errorDict] stringValue];
    return errorDict.count > 0 ? nil : statusString;
}


-(BOOL) clickElement:(PFUIElement*)element
{
	if (element == nil)
	{
		return NO;
	}
    return [element performAction:(NSString*)kAXPressAction];
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
    return [self windowForHandle:self.currentWindowHandle];
}

-(id) executeScript:(NSString*)scriptText
{
	NSDictionary *errorDict;
	NSAppleScript *script = [[NSAppleScript alloc] initWithSource:scriptText];
	NSAppleEventDescriptor *result = [script executeAndReturnError:&errorDict];
    return errorDict.count > 0 ? errorDict : result;
}

-(NSString*) pageSource
{
	NSMutableDictionary *dom = [NSMutableDictionary new];
	[self pageSourceHelperFromElement:self.currentApplication dictionary:dom];

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

-(void)pageSourceHelperFromElement:(PFUIElement*)root dictionary:(NSMutableDictionary*) dict
{
	[dict setValue:root.AXRole forKey:@"AXRole"];
	[dict setValue:root.AXTitle forKey:@"AXTitle"];
	[dict setValue:root.AXDescription forKey:@"AXDescription"];
	NSMutableArray *children = [NSMutableArray new];
	[dict setValue:children forKey:@"AXChildren"];
	for (PFUIElement *child in root.AXChildren)
	{
		NSMutableDictionary *childDict = [NSMutableDictionary new];
		[self pageSourceHelperFromElement:child dictionary:childDict];
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
    NSAppleScript *fronstMostProcessScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\"\nset application_id to (get the id of application \"%@\" as string)\nset process_name to name of (application processes where bundle identifier is application_id)\nend tell\nreturn item 1 of process_name as text", applicationName]];
    NSString *statusString = [[fronstMostProcessScript executeAndReturnError:&errorDict] stringValue];
    // TODO: Add error handling
    return errorDict.count > 0 ? nil : statusString;
}

-(void) sendKeys:(NSString*)keys
{
    [self sendKeys:keys toElement:nil];
}

-(void) sendKeys:(NSString*)keys toElement:(PFUIElement*)element
{
	if (element != nil)
	{
		[element setAXFocused:[NSNumber numberWithBool:YES]];
	}
	[self.systemEvents keystroke:keys using:0];
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
	return [self xmlPageSourceFromElement:nil pathMap:nil];
}

-(GDataXMLDocument*)xmlPageSourceFromElement:(PFUIElement*)rootUIElement pathMap:(NSMutableDictionary*)pathMap
{
	if (rootUIElement == nil)
	{
		rootUIElement = self.currentApplication;
	}

	GDataXMLElement *root = [GDataXMLNode elementWithName:rootUIElement.AXRole];
	[self xmlPageSourceHelperFromElement:rootUIElement element:root path:@"/*[1]" pathMap:pathMap];
	GDataXMLDocument *doc = [[GDataXMLDocument alloc] initWithRootElement:root];
	return doc;
}

-(void)xmlPageSourceHelperFromElement:(PFUIElement*)root element:(GDataXMLElement*) element path:(NSString*)path pathMap:(NSMutableDictionary*)pathMap
{
	[element addAttribute:[GDataXMLElement attributeWithName:@"AXRole" stringValue:root.AXRole]];
	[element addAttribute:[GDataXMLElement attributeWithName:@"AXTitle" stringValue:root.AXTitle]];
	[element addAttribute:[GDataXMLElement attributeWithName:@"AXDescription" stringValue:root.AXDescription]];
	[element addAttribute:[GDataXMLElement attributeWithName:@"AXRoleDescription" stringValue:root.AXRoleDescription]];

	if (root.AXValue != nil)
	{
		[element addAttribute:[GDataXMLElement attributeWithName:@"AXValue" stringValue:[NSString stringWithFormat:@"%@",root.AXValue]]];
	 }

	NSString *identifier = [root valueForAttribute:@"AXIdenitifer"];
	if (identifier != nil)
	{
		[element addAttribute:[GDataXMLElement attributeWithName:@"AXIdentifier" stringValue:[NSString stringWithFormat:@"%@",[root valueForAttribute:@"AXIdentifier"]]]];
	}

	if (pathMap != nil)
	{
		[pathMap setValue:root forKey:path];
		[element addAttribute:[GDataXMLElement attributeWithName:@"path" stringValue:path]];
	}

	for (int i=0; i < root.AXChildren.count; i++)
	{
		PFUIElement *child = [root.AXChildren objectAtIndex:i];
		GDataXMLElement *childElement = [GDataXMLElement elementWithName:child.AXRole];
		[self xmlPageSourceHelperFromElement:child element:childElement path:[path stringByAppendingFormat:@"/*[%d]", i+1] pathMap:pathMap];
		[element addChild:childElement];
	}
}

@end
