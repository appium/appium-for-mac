//
//  AppiumMacElementLocator.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/3/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AppiumMacSessionController.h"
#import "SystemEvents.h"

typedef enum appiumMacLocatoryStrategy
{
	AppiumMacLocatoryStrategyName,
	AppiumMacLocatoryStrategyTagName

} AppiumMacLocatoryStrategy;

@interface AppiumMacElementLocator : NSObject

@property AppiumMacSessionController *session;
@property AppiumMacLocatoryStrategy strategy;
@property NSString *value;
@property BOOL isRecursive;
@property BOOL isXPath;
@property AppiumMacElementLocator *nextLocator;

-(id) initWithSession:(AppiumMacSessionController*)session strategy:(AppiumMacLocatoryStrategy)strategy value:(NSString*)value isRecursive:(BOOL)isRecursive
;

+(AppiumMacElementLocator*) locatorWithSession:(AppiumMacSessionController*)session using:(NSString*)using value:(NSString*)value;

-(SystemEventsUIElement*) findUsingBaseElement:(SystemEventsUIElement*)baseElement;
-(void)findAllUsingBaseElement:(SystemEventsUIElement*)baseElement results:(NSMutableArray*)results;

@end
