//
//  AppiumAppDelegate.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/27/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumForMacAppDelegate.h"

#import "AfMHTTPServer.h"
#import "AfMHTTPConnection.h"
#import "HTTPServer.h"
#import "DDLog.h"
#import "DDTTYLogger.h"
#import "Utility.h"

static const int ddLogLevel = LOG_LEVEL_VERBOSE;

@implementation AppiumForMacAppDelegate

-(void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [DDLog addLogger:[DDTTYLogger sharedInstance]];
    self.server = [[AfMHTTPServer alloc] init];
    [self.server setType:@"_http._tcp."];
    [self.server setPort:4622];
    [self.server setName:[NSString stringWithFormat:@"Appium for Mac (%@)", [Utility bundleVersion]]];
    [self.server setConnectionClass:[AfMHTTPConnection class]];
	NSError *error = nil;
	if(![self.server start:&error])
	{
		DDLogError(@"Error starting HTTP Server: %@", error);
	}
}

@end
