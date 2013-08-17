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
#import <PFAssistive/PFAssistive.h>

@interface AfMSessionController : NSObject

@property NSMutableDictionary *elements;
@property int elementIndex;

@property (readonly) NSArray* allProcesses;
@property (readonly) NSArray* allWindows;
@property (readonly) NSArray* allWindowHandles;
@property NSDictionary *capabilities;
@property PFUIElement *currentApplication;
@property NSString *currentApplicationName;
@property NSString *currentProcessName;
@property (readonly) PFUIElement *currentWindow;
@property NSString *currentWindowHandle;
@property (readonly) SystemEventsProcess *currentProcess;
@property FinderApplication *finder;
@property (readonly) NSString* frontmostApplicationName;
@property (readonly) NSString* frontmostProcessName;
@property (readonly) NSString* frontmostWindowHandle;
@property SystemEventsApplication *systemEvents;

-(void) activateApplication;
-(void) activateWindow:(NSString*)windowHandle;
-(NSString*) applicationNameForProcessName:(NSString*)processName;
-(void) clickElement:(PFUIElement*)element;
-(PFUIElement*) elementByName:(NSString*)name baseElement:(PFUIElement*)baseElement;
-(void) closeWindow:(NSString*)windowHandle;
-(NSDictionary*) pageSource;
-(NSInteger) pidForProcessName:(NSString*)processName;
-(SystemEventsProcess*) processForName:(NSString*)processName;
-(NSString*) processNameForApplicationName:(NSString*) applicationName;
-(void) sendKeys:(NSString*)keys;
-(void) sendKeys:(NSString*)keys toElement:(PFUIElement*)element;
-(PFUIElement*) windowForHandle:(NSString*)windowHandle;

@end