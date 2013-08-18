//
//  AppiumMacElementLocator.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/3/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMElementLocator.h"

@implementation AfMElementLocator

-(id) initWithSession:(AfMSessionController*)session strategy:(AppiumMacLocatoryStrategy)strategy value:(NSString*)value
{
    self = [super init];
    if (self) {
		self.session = session;
		self.strategy = strategy;
		self.value = value;
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

-(PFUIElement*) findUsingBaseElement:(PFUIElement*)baseElement
{
	// use different method for xpath
	if (self.strategy == AppiumMacLocatoryStrategyXPath)
	{
		NSMutableDictionary *pathMap = [NSMutableDictionary new];
		GDataXMLDocument *doc = [self.session xmlPageSourceFromElement:baseElement pathMap:pathMap];
		NSError *error;
		NSArray *matches = [doc nodesForXPath:self.value error:&error];
		if (matches.count < 1)
		{
			return nil;
		}
		NSString *matchedPath = [(GDataXMLElement*)[matches objectAtIndex:0] attributeForName:@"path"].stringValue;
		PFUIElement *matchedElement = [pathMap objectForKey:matchedPath];
		return matchedElement;
	}

    // check if this the element we are looking for
    if ([self matchesElement:baseElement])
    {
        // return the element if it matches
        return baseElement;
    }

    // search the children
    NSArray *elementsToSearch;
	if (baseElement != nil)
    {
        // search the children if this is an element
        elementsToSearch = baseElement.AXChildren;
    }
    else
    {
		// get elements from the current window of the process if no base element is supplied
        if (self.session.currentProcess != nil)
        {
            elementsToSearch = self.session.currentWindow != nil ? self.session.currentWindow.AXChildren : [NSArray new];
			// TODO: Fix crash that occurs when there are no windows
        }
    }

	// check the children
    if (elementsToSearch != nil)
    {
        for(PFUIElement* childElement in elementsToSearch)
        {
            // check the child
            PFUIElement *childResult = [self findUsingBaseElement:childElement];

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

-(void)findAllUsingBaseElement:(PFUIElement*)baseElement results:(NSMutableArray*)results
{
	// use different method for xpath
	if (self.strategy == AppiumMacLocatoryStrategyXPath)
	{
		NSMutableDictionary *pathMap = [NSMutableDictionary new];
		GDataXMLDocument *doc = [self.session xmlPageSourceFromElement:baseElement pathMap:pathMap];
		NSError *error;
		NSArray *matches = [doc nodesForXPath:self.value error:&error];
		for(GDataXMLElement *match in matches)
		{
			PFUIElement *matchedElement = [pathMap objectForKey:[match attributeForName:@"path"].stringValue];
			[results addObject:matchedElement];
			return;
		}
	}

    // check if this the element we are looking for
    if ([self matchesElement:baseElement])
    {
        // return the element if it matches
        [results addObject: baseElement];
    }

    // search the children
    NSArray *elementsToSearch;
	if (baseElement != nil)
    {
        // search the children if this is an element
        elementsToSearch = baseElement.AXChildren;
    }
    else
    {
		// get elements from the current window of the process if no base element is supplied
        if (self.session.currentProcess != nil)
        {
            elementsToSearch = self.session.currentWindow.AXChildren;
        }
    }

	// check the children
    if (elementsToSearch != nil)
    {
        for(PFUIElement* childElement in elementsToSearch)
        {
            // check the child
            [self findAllUsingBaseElement:childElement results:results];
        }
    }
}

@end
