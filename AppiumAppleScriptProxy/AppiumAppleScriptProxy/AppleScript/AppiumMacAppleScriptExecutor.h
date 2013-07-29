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

@property NSString *currentProcess;
@property FinderApplication *finder;
@property SystemEventsApplication *systemEvents;

-(NSString*) frontmostApplication;
-(NSDictionary*) pageSource;

@end