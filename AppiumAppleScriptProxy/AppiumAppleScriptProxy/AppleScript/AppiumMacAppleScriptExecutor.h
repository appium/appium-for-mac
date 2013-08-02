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

@property (readonly) NSArray* allProcesses;
@property NSString *currentApplicationName;
@property NSString *currentProcessName;
@property (readonly) SystemEventsProcess *currentProcess;
@property FinderApplication *finder;
@property (readonly) NSString* frontmostApplicationName;
@property (readonly) NSString* frontmostProcessName;
@property SystemEventsApplication *systemEvents;

-(void) activateApplication:(NSString*)applicationName;
-(NSString*) applicationForProcessName:(NSString*)processName;
-(void) clickElement:(SystemEventsUIElement*)element;
-(SystemEventsUIElement*) elementByName:(NSString*)name baseElement:(SystemEventsUIElement*)baseElement;
-(NSDictionary*) pageSource;
-(int) pidForProcessName:(NSString*)processName;
-(SystemEventsProcess*) processForName:(NSString*)processName;
-(NSString*) processNameForApplicationName:(NSString*) applicationName;
-(void) selectElement:(SystemEventsUIElement*)element;
-(void) sendKeys:(NSString*)keys;
-(void) sendKeys:(NSString*)keys toElement:(SystemEventsUIElement*)element;

@end