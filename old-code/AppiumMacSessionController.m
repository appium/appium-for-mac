//
//  AppiumMacAppleScriptLibrary.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacSessionController.h"

#import "NSString+Extensions.h"

@interface _DomElement : NSObject

@property NSString *type;
@property NSString *identifier;
@property _DomElement *parent;
@property NSMutableArray *children;

-(BOOL) isEqualTo:(_DomElement*)otherElement;
-(NSString*) stringValue;
-(NSString*) jsonify;

@end

@implementation AppiumMacSessionController

- (id)init
{
    self = [super init];
    if (self) {
        [self setElementIndex:0];
        [self setElements:[NSMutableDictionary new]];
        [self setCurrentApplicationName:nil];
        [self setCurrentProcessName:nil];
        [self setFinder:[SBApplication applicationWithBundleIdentifier:@"com.apple.finder"]];
        [self setSystemEvents:[SBApplication applicationWithBundleIdentifier:@"com.apple.systemevents"]];
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
        SystemEventsWindow *window = [self getWindowForHandle:windowHandle forProcess:processName];
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

-(SystemEventsWindow*) getWindowForHandle:(NSString*)windowHandle forProcess:(NSString*)processName
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

-(NSDictionary*) pageSource
{	/*
	// Slow Technique
    NSMutableArray *children = [NSMutableArray new];
    NSDictionary *source = [NSDictionary dictionaryWithObject:children forKey:@"source"];
    SystemEventsProcess *process = [self processForName:self.currentProcessName];
    if (process.name != nil)
    {
        for(SystemEventsUIElement *element in process.entireContents)
        {
            [children addObject:[NSString stringWithFormat:@"%@ %@", element.class, element.name]];
        }
    }
    return source;
	*/

	// Fast Technique (BADLY PORTED FROM PYTHON)
	NSDictionary *errorDict;
    NSAppleScript *getDomScript = [[NSAppleScript alloc] initWithSource:[NSString stringWithFormat:@"tell application \"System Events\" to tell process \"%@\" to return entire contents", self.currentProcessName]];
    NSString *content = [[getDomScript executeAndReturnError:&errorDict] stringValue];

	NSError *error;
	NSRegularExpression *domRegex = [NSRegularExpression regularExpressionWithPattern:@"([^\"]+\\s)+(\"[^\"]*\"|\\d+)" options:0 error:&error];
	NSMutableArray *ocDom = [NSMutableArray new];
	NSMutableArray *asDom = [[content componentsSeparatedByString:@","] mutableCopy];
	NSInteger asDomSize = asDom.count;
	NSInteger index = 0;
	while (index < asDomSize)
	{
		NSString *asDomItem = [(NSString*)[asDom objectAtIndex:index] copy];
		while ([asDomItem countOccurencesOfSubString:@"\""] % 2 == 1) // there's a comma inside the string
		{
			// check for malformed dump
			if (index + 1 > asDomSize)
			{
				[NSException raise:@"Malformed AppleScript Dump" format:@"%@", asDomItem];
			}

			// append the next item because it's part of the same element
			index++;
			asDomItem = [asDomItem stringByAppendingString:[asDom objectAtIndex:index]];
		}

		// split the elements in the dom item
		NSArray *asDomElements = [asDomItem componentsSeparatedByString:@" of "];

		
		_DomElement *parent = nil;
		NSMutableArray *nextDomLevel = ocDom;

		for (NSString *asDomElement in [asDomElements reverseObjectEnumerator])
		{
			NSArray* matches = [domRegex matchesInString:asDomElement options:0 range:NSMakeRange(0, [asDomElement length])];
			if (matches.count > 0)
			{
				// read the values
				NSTextCheckingResult *match = [matches objectAtIndex:0];
				NSString *type = [asDomElement substringWithRange:[match rangeAtIndex:1]];
				NSString *identifier = [asDomElement substringWithRange:[match rangeAtIndex:2]];

				// check if the object exists in the dom
				_DomElement *ocDomObject = nil;
				if (parent != nil)
				{
					for (_DomElement *ocDomItem in parent.children)
					{
						if ([ocDomItem.type isEqualToString:type] && [ocDomItem.identifier isEqualToString:identifier])
						{
							ocDomObject = ocDomItem;
						}
					}
				}

				// create a new object if its new to the dom
				if (ocDomObject == nil)
				{
					ocDomObject = [_DomElement new];
					ocDomObject.type = type;
					ocDomObject.identifier = identifier;
				}

				// check if it already exists
				BOOL alreadyExists = NO;
				for(_DomElement *ocDomItem in nextDomLevel)
				{
					if ([ocDomItem.type isEqualToString:type] && [ocDomItem.identifier isEqualToString:identifier])
					{
						alreadyExists = YES;
						ocDomObject = ocDomItem;
						break;
					}
				}

				// add it to the dom if it already exists
				if (!alreadyExists)
				{
					[nextDomLevel addObject:ocDomObject];
				}

				// set this element to be the parent of the next element
				parent = ocDomObject;
				nextDomLevel = parent.children;
			}

			//for ( NSTextCheckingResult* match in matches )
			//{
			//	NSString* matchText = [searchedString substringWithRange:[match range]];
			//	NSLog(@"match: %@", matchText);
			//}
		}

		// increment and continue the loop
		index++;
	}
	return [NSDictionary dictionaryWithObject:ocDom forKey:@"source"];
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

@end

@implementation _DomElement

-(id) init
{
    self = [super init];
    if (self) {
		self.parent = nil;
        self.children = [NSMutableArray new];
    }
    return self;
}

-(BOOL) isEqualTo:(_DomElement*)otherElement
{
	if (otherElement == nil)
		return NO;
	if (![self.type isEqualToString:otherElement.type])
		return NO;
	if (![self.identifier isEqualToString:otherElement.identifier])
		return NO;
	if (self.parent != nil && otherElement.parent != nil && ![self.parent.stringValue isEqualToString:otherElement.parent.stringValue])
		return NO;
	if (self.children.count < otherElement.children.count)
		return NO;
	for (int i=0; i < self.children.count; i++)
	{
		_DomElement *myChild = [self.children objectAtIndex:i];
		_DomElement *otherChild = [otherElement.children objectAtIndex:i];
		if (![myChild isEqualTo:otherChild])
		{
			return NO;
		}
	}
	return YES;
}

-(NSString*) stringValue
{
	if (self.parent != nil)
		return [NSString stringWithFormat:@"%@ %@ of %@", self.type, self.identifier, [self.parent stringValue]];
	else
		return [NSString stringWithFormat:@"%@ %@", self.type, self.identifier];
}

-(NSString*) jsonify
{
	NSString *childJson = @"[";
	for (_DomElement *child in self.children)
	{
		childJson = [childJson stringByAppendingFormat:@"%@,", [child jsonify]];
	}
	if ([childJson hasSuffix:@","])
	{
		childJson = [childJson substringToIndex:childJson.length-2];
	}
	childJson = [childJson stringByAppendingString:@"]"];
	return [NSString stringWithFormat:@"{\"type\":\"%@\",\"id\":\"%@\",\"children\":\"%@\"}",[self.type stringByReplacingOccurrencesOfString:@"\"" withString:@"\\\""], [self.identifier stringByReplacingOccurrencesOfString:@"\"" withString:@"\\\""], childJson];
}

@end
