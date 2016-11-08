//
//  AfMHTTPConnection.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Improvements by Stuart Russell at Intuit, Inc.
//  Copyright (c) 2013-2016 Appium. All rights reserved.
//

#import "AfMHTTPConnection.h"
#import "AfMStatusCodes.h"
#import "AppiumForMacAppDelegate.h"
#import "AppiumMacHTTPJSONResponse.h"
#import "HTTPDataResponse.h"
#import "HTTPLogging.h"
#import "HTTPMessage.h"
#import "Utility.h"


// Log levels: off, error, warn, info, verbose
// Other flags: trace
static const int httpLogLevel = HTTP_LOG_LEVEL_VERBOSE;

@implementation AfMHTTPConnection

- (BOOL)supportsMethod:(NSString *)method atPath:(NSString *)path
{
	HTTPLogTrace();

	// Add support for POST
	if ([method isEqualToString:@"POST"])
	{
		return YES;
	}
    if ([method isEqualToString:@"DELETE"])
	{
		return YES;
	}

	return [super supportsMethod:method atPath:path];
}

- (BOOL)expectsRequestBodyFromMethod:(NSString *)method atPath:(NSString *)path
{
	HTTPLogTrace();

	// Inform HTTP server that we expect a body to accompany a POST request
	if([method isEqualToString:@"POST"])
		return YES;

	return [super expectsRequestBodyFromMethod:method atPath:path];
}

- (NSObject<HTTPResponse> *)httpResponseForMethod:(NSString *)method URI:(NSString *)path
{
    // Let the AFM_SERVER.handlers try to handle the request.
    NSData *postData = [method isEqualToString:@"POST"] ? [request body] : nil;
    AppiumMacHTTPJSONResponse *handlerReturnValue = [AFM_SERVER.handlers httpResponseForMethod:method URI:path data:postData];
    if (handlerReturnValue) {
        return handlerReturnValue;
    }
    
    // AFM_SERVER.handlers did not handle it, so do the server default.
    return [super httpResponseForMethod:method URI:path];
}

- (void)prepareForBodyWithSize:(UInt64)contentLength
{
	HTTPLogTrace();

	// If we supported large uploads,
	// we might use this method to create/open files, allocate memory, etc.
}

- (void)processBodyData:(NSData *)postDataChunk
{
	HTTPLogTrace();

	// Remember: In order to support LARGE POST uploads, the data is read in chunks.
	// This prevents a 50 MB upload from being stored in RAM.
	// The size of the chunks are limited by the POST_CHUNKSIZE definition.
	// Therefore, this method may be called multiple times for the same POST request.

	BOOL result = [request appendData:postDataChunk];
	if (!result)
	{
		HTTPLogError(@"%@[%p]: %@ - Couldn't append bytes!", THIS_FILE, self, THIS_METHOD);
	}
}

@end

