//
//  AppiumMacElementLocator.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/3/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacElementLocator.h"

@implementation AppiumMacElementLocator

-(id) initWithSession:(AppiumMacSessionController*)session strategy:(AppiumMacLocatoryStrategy)strategy value:(NSString*)value isRecursive:(BOOL)isRecursive{
    self = [super init];
    if (self) {
		self.session = session;
		self.strategy = strategy;
		self.value = value;
		self.isRecursive = isRecursive;
    }
    return self;
}

+(AppiumMacElementLocator*) locatorWithSession:(AppiumMacSessionController*)session using:(NSString*)using value:(NSString*)value
{
	if ([using isEqualToString:@"name"])
	{
		return [[AppiumMacElementLocator alloc] initWithSession:session strategy:AppiumMacLocatoryStrategyName value:value isRecursive:YES];
	}
	else if ([using isEqualToString:@"tag name"])
	{
		return [[AppiumMacElementLocator alloc] initWithSession:session strategy:AppiumMacLocatoryStrategyTagName value:value isRecursive:YES];
	}
	return nil;
}

-(BOOL) matchesElement:(SystemEventsUIElement*)element
{
	switch(self.strategy)
	{
		case AppiumMacLocatoryStrategyName:
			return element != nil && [self.value isEqualToString:element.title];
		case AppiumMacLocatoryStrategyTagName:
			return element != nil && [self.value isEqualToString:element.role];
		default:
			return NO;
	}
}

-(SystemEventsUIElement*) findUsingBaseElement:(SystemEventsUIElement*)baseElement
{
    // check if this the element we are looking for
    if ([self matchesElement:baseElement])
    {
        // return the element if it matches
        return baseElement;
    }

    // search the children
    NSArray *elementsToSearch;
	if (!self.isRecursive)
	{
		// don't search children if recursion is not requested
		elementsToSearch = nil;
	}
	else if (baseElement != nil)
    {
        // search the children if this is an element
        elementsToSearch = baseElement.UIElements;
    }
    else
    {
		// get elements from the current window of the process if no base element is supplied
        if (self.session.currentProcess != nil)
        {
            elementsToSearch = [self.session getWindowForHandle:self.session.currentWindowHandle forProcess:self.session.currentProcessName].UIElements;
        }
    }

	// check the children
    if (elementsToSearch != nil)
    {
        for(SystemEventsUIElement* childElement in elementsToSearch)
        {
            // check the child
            SystemEventsUIElement *childResult = [self findUsingBaseElement:childElement];

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

-(void)findAllUsingBaseElement:(SystemEventsUIElement*)baseElement results:(NSMutableArray*)results
{
    // check if this the element we are looking for
    if ([self matchesElement:baseElement])
    {
        // return the element if it matches
        [results addObject: baseElement];
    }

    // search the children
    NSArray *elementsToSearch;
	if (!self.isRecursive)
	{
		// don't search children if recursion is not requested
		elementsToSearch = nil;
	}
	else if (baseElement != nil)
    {
        // search the children if this is an element
        elementsToSearch = baseElement.UIElements;
    }
    else
    {
		// get elements from the current window of the process if no base element is supplied
        if (self.session.currentProcess != nil)
        {
            elementsToSearch = [self.session getWindowForHandle:self.session.currentWindowHandle forProcess:self.session.currentProcessName].UIElements;
        }
    }

	// check the children
    if (elementsToSearch != nil)
    {
        for(SystemEventsUIElement* childElement in elementsToSearch)
        {
            // check the child
            [self findAllUsingBaseElement:childElement results:results];
        }
    }
}

@end
