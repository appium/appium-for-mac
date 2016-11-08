//
//  AfMElementLocator.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/3/13.
//  Improvements by Stuart Russell at Intuit, Inc.
//  Copyright (c) 2013-2016 Appium. All rights reserved.
//

#import <PFAssistive/PFUIElement.h>
#import "AfMElementLocator.h"
#import "AfMStatusCodes.h"

@interface AfMElementLocator()
// Use a private property so this class and subclasses can have a different session class.
@property AfMSessionController *session;
@end

@implementation AfMElementLocator

-(id) initWithSession:(AfMSessionController*)session strategy:(AppiumMacLocatoryStrategy)strategy value:(NSString*)value
{
    self = [super init];
    if (self) {
		self.session = session;
		self.strategy = strategy;
		self.value = value;
        if ([self.value rangeOfString:@"/AXApplication"].location != 0) {
            NSLog(@"AfMElementLocator initWithSession self.value is not AbsoluteAXPath: %@", self.value);
        }
    }
    return self;
}

+(AfMElementLocator*) locatorWithSession:(AfMSessionController*)session using:(NSString*)using value:(NSString*)value
{
	if ([using isEqualToString:@"id"])
	{
		return [[AfMElementLocator alloc] initWithSession:session strategy:AppiumMacLocatoryStrategyID value:value];
	}
	else if ([using isEqualToString:@"name"])
	{
		return [[AfMElementLocator alloc] initWithSession:session strategy:AppiumMacLocatoryStrategyName value:value];
	}
	else if ([using isEqualToString:@"tag name"])
	{
		return [[AfMElementLocator alloc] initWithSession:session strategy:AppiumMacLocatoryStrategyTagName value:value];
	}
	else if ([using isEqualToString:@"xpath"])
	{
		return [[AfMElementLocator alloc] initWithSession:session strategy:AppiumMacLocatoryStrategyXPath value:value];
	}
	return nil;
}

-(BOOL) matchesElement:(PFUIElement*)element
{
	switch(self.strategy)
	{
		case AppiumMacLocatoryStrategyID:
		{
			if (element == nil)
			{
				return NO;
			}
			NSString *identifier = [element valueForAttribute:@"AXIdentifier"];
			return identifier != nil && [self.value isEqualToString:identifier];
		}
		case AppiumMacLocatoryStrategyName:
			return element != nil && [self.value isEqualToString:element.AXTitle];
		case AppiumMacLocatoryStrategyTagName:
			return element != nil && [self.value isEqualToString:element.AXRole];
		default:
			return NO;
	}
}

-(PFUIElement*) findUsingBaseUIElement:(PFUIElement*)baseUIElement statusCode:(int *)statusCode
{
	// use different method for xpath
	if (self.strategy == AppiumMacLocatoryStrategyXPath)
	{
        // Try an absolute path first (starts with @"/AXApplication"). It will either succeed or fail, but quickly.
        NSArray *foundElements = [self.session findAllUsingAbsoluteAXPath:self.value];
        // At this point, foundElements is nil or an array. Nil means it was not an absolute path.
        // An array means it was an absolute path, and can have 0 or more items.
        if (foundElements && [foundElements isKindOfClass:[NSArray class]]) {
            // Since foundElement is an array, our path was absolute.
            if ([foundElements count] > 0) {
                // Success! Found one or more elements.
                return [foundElements firstObject];
            } else {
                // Did not find element(s). Even so, do not continue to build the XMLPageSource for an absolute path.
                // This ensures absolute paths are fast.
                *statusCode = kAfMStatusCodeNoSuchElement;
                return nil;
            }
        } else  {
            NSLog(@"findUsingBaseUIElement foundElements was nil or not an array: %@", self.value);
        }
        
        // VERY RESTRICTIVE - Remove me to handle partial XPaths (which can be very, very slow).
        if (YES) {
            NSLog(@"findUsingBaseUIElement XPath was not AbsoluteAXPath: %@", self.value);
            return nil;
        }
        
        // The path was not absolute, so use the slower method that can match partial paths.
        // WARNING: Creating the XMLPage from the UI is prohibitively slow when accessing an application from a different app.
		NSMutableDictionary *pathMap = [NSMutableDictionary new];
		GDataXMLDocument *doc = [self.session xmlPageSourceFromRootUIElement:baseUIElement pathMap:pathMap xPath:self.value];
		NSError *error;
		NSArray *matches = [doc nodesForXPath:self.value error:&error];
		if (error != nil)
		{
			*statusCode = kAfMStatusCodeXPathLookupError;
			return nil;
		}
		if (matches.count < 1)
		{
			*statusCode = kAfMStatusCodeNoSuchElement;
			return nil;
		}
		NSString *matchedPath = [(GDataXMLElement*)[matches objectAtIndex:0] attributeForName:@"path"].stringValue;
		PFUIElement *matchedElement = [pathMap objectForKey:matchedPath];
		*statusCode = kAfMStatusCodeSuccess;
		return matchedElement;
	}

    // check if this the element we are looking for
    if ([self matchesElement:baseUIElement])
    {
        // return the element if it matches
		*statusCode = kAfMStatusCodeSuccess;
        return baseUIElement;
    }

    // search the children
    NSArray *elementsToSearch;
	if (baseUIElement != nil)
    {
        // search the children if this is an element
        elementsToSearch = baseUIElement.AXChildren;
    }
    else
    {
		@try
		{
			// get elements from the current window if no base element is supplied
			if (self.session.currentWindow == nil)
			{
				*statusCode = kAfMStatusCodeNoSuchWindow;
				return nil;
			}

			elementsToSearch = self.session.currentWindow.AXChildren;
		}
		@catch (NSException *exception)
		{
			*statusCode = kAfMStatusCodeNoSuchWindow;
			return nil;
		}
    }

	// check the children
    if (elementsToSearch != nil)
    {
        for(PFUIElement* childElement in elementsToSearch)
        {
            // check the child
            PFUIElement *childResult = [self findUsingBaseUIElement:childElement statusCode:statusCode];

            //return the child if it matches
            if (childResult != nil)
            {
				*statusCode = kAfMStatusCodeSuccess;
                return childResult;
            }
        }
    }
    // return nil because there was no match
	*statusCode = kAfMStatusCodeNoSuchElement;
    return nil;
}

-(void)findAllUsingBaseUIElement:(PFUIElement*)baseUIElement results:(NSMutableArray*)results statusCode:(int *)statusCode
{
	// use different method for xpath
	if (self.strategy == AppiumMacLocatoryStrategyXPath)
	{
        // Try an absolute path first. It will either succeed or fail, but quickly.
        NSArray *foundElements = [self.session findAllUsingAbsoluteAXPath:self.value];
        if ([foundElements count] > 0) {
            [results addObjectsFromArray:foundElements];
            return;
        }
        
        // Absolute path did not match, so use the slower, guaranteed method.
		NSMutableDictionary *pathMap = [NSMutableDictionary new];
		GDataXMLDocument *doc = [self.session xmlPageSourceFromRootUIElement:baseUIElement pathMap:pathMap xPath:self.value];
		NSError *error;
		NSArray *matches = [doc nodesForXPath:self.value error:&error];
		if (error != nil)
		{
			*statusCode = kAfMStatusCodeXPathLookupError;
			return;
		}
		if (matches.count < 1)
		{
			*statusCode = kAfMStatusCodeNoSuchElement;
			return;
		}

		*statusCode = kAfMStatusCodeSuccess;
		for(GDataXMLElement *match in matches)
		{
			PFUIElement *matchedElement = [pathMap objectForKey:[match attributeForName:@"path"].stringValue];
			[results addObject:matchedElement];
		}
        return;
	}

    // check if this the element we are looking for
    if ([self matchesElement:baseUIElement])
    {
        // return the element if it matches
		*statusCode = kAfMStatusCodeSuccess;
        [results addObject: baseUIElement];
    }

    // search the children
    NSArray *elementsToSearch;
	if (baseUIElement != nil)
    {
        // search the children if this is an element
        elementsToSearch = baseUIElement.AXChildren;
    }
    else
    {
		// get elements from the current window of the process if no base element is supplied
        if (self.session.currentWindow == nil)
        {
			*statusCode = kAfMStatusCodeNoSuchWindow;
			return;
        }
		elementsToSearch = self.session.currentWindow.AXChildren;
    }

	// check the children
    if (elementsToSearch != nil)
    {
        for(PFUIElement* childElement in elementsToSearch)
        {
            // check the child
            [self findAllUsingBaseUIElement:childElement results:results statusCode:statusCode];
        }
    }
}

@end

