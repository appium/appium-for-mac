//
//  Utility.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Utility : NSObject

+(NSString*) arch;
+(NSString*) bundleRevision;
+(NSString*) bundleVersion;
+(NSString*) getSessionFromPath:(NSString*)path;
+(NSString*) randomStringOfLength:(int)length;
+(int) unixTimestamp;
+(NSString*) version;

@end
