//
//  AppiumMacElementLocator.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/3/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AfMSessionController.h"
#import <PFAssistive/PFUIElement.h>

typedef enum appiumMacLocatoryStrategy
{
	AppiumMacLocatoryStrategyName,
	AppiumMacLocatoryStrategyTagName,
	AppiumMacLocatoryStrategyXPath

} AppiumMacLocatoryStrategy;

@interface AfMElementLocator : NSObject

@property AfMSessionController *session;
@property AppiumMacLocatoryStrategy strategy;
@property NSString *value;
@property BOOL isRecursive;
@property BOOL isXPath;
@property AfMElementLocator *nextLocator;

-(id) initWithSession:(AfMSessionController*)session strategy:(AppiumMacLocatoryStrategy)strategy value:(NSString*)value isRecursive:(BOOL)isRecursive
;

+(AfMElementLocator*) locatorWithSession:(AfMSessionController*)session using:(NSString*)using value:(NSString*)value;

-(PFUIElement*) findUsingBaseElement:(PFUIElement*)baseElement;
-(void)findAllUsingBaseElement:(PFUIElement*)baseElement results:(NSMutableArray*)results;

@end
