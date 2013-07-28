//
//  Utility.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GCDWebServerRequest.h"

@interface Utility : NSObject

+(NSString*) arch;
+(NSString*) bundleRevision;
+(NSString*) bundleVersion;
+(NSString*) getSessionFromRequest:(GCDWebServerRequest*)request;
+(NSString*) randomStringOfLength:(int)length;
+(int) unixTimestamp;
+(NSString*) version;

@end
