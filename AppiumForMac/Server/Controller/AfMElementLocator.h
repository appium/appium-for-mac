//
//  AfMElementLocator.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/3/13.
//  Improvements by Stuart Russell at Intuit, Inc.
//  Copyright (c) 2013-2016 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AfMSessionController.h"

typedef enum appiumMacLocatoryStrategy
{
	AppiumMacLocatoryStrategyClassName,
	AppiumMacLocatoryStrategyCSSSelector,
	AppiumMacLocatoryStrategyID,
    AppiumMacLocatoryStrategyLinkText,
	AppiumMacLocatoryStrategyName,
    AppiumMacLocatoryStrategyPartialLinkText,
	AppiumMacLocatoryStrategyTagName,
	AppiumMacLocatoryStrategyXPath
} AppiumMacLocatoryStrategy;

@class PFUIElement;

@interface AfMElementLocator : NSObject

@property AppiumMacLocatoryStrategy strategy;
@property NSString *value;

-(id) initWithSession:(AfMSessionController*)session strategy:(AppiumMacLocatoryStrategy)strategy value:(NSString*)value;

+(AfMElementLocator*) locatorWithSession:(AfMSessionController*)session using:(NSString*)using value:(NSString*)value;

-(PFUIElement*) findUsingBaseUIElement:(PFUIElement*)baseUIElement statusCode:(int*)statusCode;

-(void) findAllUsingBaseUIElement:(PFUIElement*)baseUIElement results:(NSMutableArray*)results statusCode:(int*)statusCode;

@end
