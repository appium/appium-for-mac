//
//  AfMString.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/4/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "NSString+Extensions.h"

@implementation NSString (countOccurencesOfSubString)

-(NSInteger) countOccurencesOfSubString:(NSString*)subString
{
	return (self.length - [[self stringByReplacingOccurrencesOfString:subString withString:@""] length]) / subString.length;
}

@end
