//
//  AppiumMacAppleScriptLibrary.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumMacHTTPJSONResponse.h"
#import "Finder.h"
#import "SystemEvents.h"

@interface AppiumMacAppleScriptExecutor : AppiumMacHTTPJSONResponse

@property NSString *currentApplication;
@property NSString *currentProcess;
@property FinderApplication *finder;
@property SystemEventsApplication *systemEvents;

-(void) activateApplication:(NSString*)applicationName;
-(NSArray*) allProcesses;
-(NSString*) applicationForProcessName:(NSString*)processName;
-(SystemEventsUIElement*) elementByName:(NSString*)name baseElement:(SystemEventsUIElement*)baseElement;
-(NSString*) frontmostApplication;
-(NSString*) frontmostProcess;
-(NSDictionary*) pageSource;
-(int) pidForProcess:(NSString*)processName;
-(NSString*) processForApplication:(NSString*) applicationName;

@end