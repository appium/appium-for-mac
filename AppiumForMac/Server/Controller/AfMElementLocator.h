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
	AppiumMacLocatoryStrategyID,
	AppiumMacLocatoryStrategyName,
	AppiumMacLocatoryStrategyTagName,
	AppiumMacLocatoryStrategyXPath

} AppiumMacLocatoryStrategy;

@interface AfMElementLocator : NSObject

@property AfMSessionController *session;
@property AppiumMacLocatoryStrategy strategy;
@property NSString *value;

-(id) initWithSession:(AfMSessionController*)session strategy:(AppiumMacLocatoryStrategy)strategy value:(NSString*)value;

+(AfMElementLocator*) locatorWithSession:(AfMSessionController*)session using:(NSString*)using value:(NSString*)value;

-(PFUIElement*) findUsingBaseElement:(PFUIElement*)baseElement statusCode:(int*)statusCode;

-(void) findAllUsingBaseElement:(PFUIElement*)baseElement results:(NSMutableArray*)results statusCode:(int*)statusCode;

@end
