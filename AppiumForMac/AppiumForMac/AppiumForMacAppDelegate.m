//
//  AppiumAppDelegate.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/27/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumForMacAppDelegate.h"

#import "AppiumMacHTTPServer.h"
#import "AppiumMacHTTPConnection.h"
#import "HTTPServer.h"
#import "DDLog.h"
#import "DDTTYLogger.h"
#import "Utility.h"

static const int ddLogLevel = LOG_LEVEL_VERBOSE;

@implementation AppiumForMacAppDelegate

-(void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [DDLog addLogger:[DDTTYLogger sharedInstance]];
    self.server = [[AppiumMacHTTPServer alloc] init];
    [self.server setType:@"_http._tcp."];
    [self.server setPort:8080];
    [self.server setName:[NSString stringWithFormat:@"Appium for Mac (%@)", [Utility bundleVersion]]];
    [self.server setConnectionClass:[AppiumMacHTTPConnection class]];
	NSError *error = nil;
	if(![self.server start:&error])
	{
		DDLogError(@"Error starting HTTP Server: %@", error);
	}
}

@end
