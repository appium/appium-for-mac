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
#import "GDataXMLNode.h"

#import <PFAssistive/PFAssistive.h>

@interface AfMSessionController : NSObject

@property NSMutableDictionary *elements;
@property int elementIndex;

@property (readonly) NSArray* allProcesses;
@property (readonly) NSArray* allWindows;
@property (readonly) NSArray* allWindowHandles;
@property NSDictionary *capabilities;
@property PFApplicationUIElement *currentApplication;
@property NSString *currentApplicationName;
@property NSString *currentProcessName;
@property (readonly) PFUIElement *currentWindow;
@property NSString *currentWindowHandle;
@property (readonly) SystemEventsProcess *currentProcess;
@property FinderApplication *finder;
@property SystemEventsApplication *systemEvents;

-(void) activateApplication;
-(void) activateWindow;
-(NSString*) applicationNameForProcessName:(NSString*)processName;
-(void) clickElement:(PFUIElement*)element;
-(void) closeWindow:(NSString*)windowHandle;
-(NSDictionary*) pageSource;
-(NSInteger) pidForProcessName:(NSString*)processName;
-(SystemEventsProcess*) processForName:(NSString*)processName;
-(NSString*) processNameForApplicationName:(NSString*) applicationName;
-(void) sendKeys:(NSString*)keys;
-(void) sendKeys:(NSString*)keys toElement:(PFUIElement*)element;
-(PFUIElement*) windowForHandle:(NSString*)windowHandle;
-(GDataXMLDocument*)xmlPageSource;
-(GDataXMLDocument*)xmlPageSourceFromElement:(PFUIElement*)rootUIElement pathMap:(NSMutableDictionary*)pathMap;

@end