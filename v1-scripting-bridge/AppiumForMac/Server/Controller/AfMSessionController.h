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

@interface AfMSessionController : NSObject

@property NSMutableDictionary *elements;
@property int elementIndex;

@property (readonly) NSArray* allProcesses;
@property (readonly) NSArray* allWindowHandles;
@property NSDictionary *capabilities;
@property NSString *currentApplicationName;
@property NSString *currentProcessName;
@property NSString *currentWindowHandle;
@property (readonly) SystemEventsProcess *currentProcess;
@property FinderApplication *finder;
@property (readonly) NSString* frontmostApplicationName;
@property (readonly) NSString* frontmostProcessName;
@property (readonly) NSString* frontmostWindowHandle;
@property SystemEventsApplication *systemEvents;

-(void) activateApplication:(NSString*)applicationName;
-(void) activateWindow:(NSString*)windowHandle forProcessName:(NSString*)processName;
-(NSString*) applicationForProcessName:(NSString*)processName;
-(void) clickElement:(SystemEventsUIElement*)element;
-(SystemEventsUIElement*) elementByName:(NSString*)name baseElement:(SystemEventsUIElement*)baseElement;
-(void) closeWindow:(NSString*)windowHandle forProcessName:(NSString*)processName;
-(NSDictionary*) pageSource;
-(NSInteger) pidForProcessName:(NSString*)processName;
-(SystemEventsProcess*) processForName:(NSString*)processName;
-(NSString*) processNameForApplicationName:(NSString*) applicationName;
-(void) selectElement:(SystemEventsUIElement*)element;
-(void) sendKeys:(NSString*)keys;
-(void) sendKeys:(NSString*)keys toElement:(SystemEventsUIElement*)element;
-(SystemEventsWindow*) windowForHandle:(NSString*)windowHandle forProcess:(NSString*)processName;

@end