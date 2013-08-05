//
//  AfMDomElement.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/4/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMDomElement.h"
#import "NSString+Extensions.h"

@implementation AfMDomElement

-(id) init
{
    self = [super init];
    if (self) {
		self.parent = nil;
        self.children = [NSMutableArray new];
    }
    return self;
}

-(BOOL) isEqualTo:(AfMDomElement*)otherElement
{
	if (otherElement == nil)
		return NO;
	if (![self.type isEqualToString:otherElement.type])
		return NO;
	if (![self.identifier isEqualToString:otherElement.identifier])
		return NO;
	if (self.parent != nil && otherElement.parent != nil && ![self.parent.stringValue isEqualToString:otherElement.parent.stringValue])
		return NO;
	if (self.children.count < otherElement.children.count)
		return NO;
	for (int i=0; i < self.children.count; i++)
	{
		AfMDomElement *myChild = [self.children objectAtIndex:i];
		AfMDomElement *otherChild = [otherElement.children objectAtIndex:i];
		if (![myChild isEqualTo:otherChild])
		{
			return NO;
		}
	}
	return YES;
}

-(NSString*) stringValue
{
	if (self.parent != nil)
		return [NSString stringWithFormat:@"%@ %@ of %@", self.type, self.identifier, [self.parent stringValue]];
	else
		return [NSString stringWithFormat:@"%@ %@", self.type, self.identifier];
}

-(NSString*) jsonify
{
	NSString *childJson = @"[";
	for (AfMDomElement *child in self.children)
	{
		childJson = [childJson stringByAppendingFormat:@"%@,", [child jsonify]];
	}
	if ([childJson hasSuffix:@","])
	{
		childJson = [childJson substringToIndex:childJson.length-2];
	}
	childJson = [childJson stringByAppendingString:@"]"];
	return [NSString stringWithFormat:@"{\"type\":\"%@\",\"id\":\"%@\",\"children\":%@}",[self.type stringByReplacingOccurrencesOfString:@"\"" withString:@"\\\""], [self.identifier stringByReplacingOccurrencesOfString:@"\"" withString:@"\\\""], childJson];
}

+(NSDictionary*) pageSource:(NSString*)content
{
	NSError *error;
	NSRegularExpression *domRegex = [NSRegularExpression regularExpressionWithPattern:@"([^\":\\s]+\\s)+(\"[^\"]*\"|\\d+)" options:0 error:&error];
	NSMutableArray *ocDom = [NSMutableArray new];
	NSMutableArray *asDom = [[content componentsSeparatedByString:@"<<<APPIUM_DIVIDER>>>"] mutableCopy];
	NSInteger asDomSize = asDom.count;
	NSInteger index = 0;
	while (index < asDomSize)
	{
		NSString *asDomItem = [(NSString*)[asDom objectAtIndex:index] copy];
		//while ([asDomItem countOccurencesOfSubString:@"\""] % 2 == 1) // there's a comma inside the string
		//{
		//	// check for malformed dump
		//	if (index + 1 > asDomSize)
		//	{
		//		[NSException raise:@"Malformed AppleScript Dump" format:@"%@", asDomItem];
		//	}

		//	// append the next item because it's part of the same element
		//	index++;
		//	asDomItem = [asDomItem stringByAppendingString:[asDom objectAtIndex:index]];
		//}

		// split the elements in the dom item
		NSArray *asDomElements = [asDomItem componentsSeparatedByString:@" of "];


		AfMDomElement *parent = nil;
		NSMutableArray *nextDomLevel = ocDom;

		for (NSString *asDomElement in [asDomElements reverseObjectEnumerator])
		{
			NSArray* matches = [domRegex matchesInString:asDomElement options:0 range:NSMakeRange(0, [asDomElement length])];
			if (matches.count > 0)
			{
				// read the values
				NSTextCheckingResult *match = [matches objectAtIndex:0];
				NSString *type = [[asDomElement substringWithRange:[match rangeAtIndex:1]] stringByReplacingOccurrencesOfString:@" " withString:@""];
				NSString *identifier = [asDomElement substringWithRange:[match rangeAtIndex:2]];

				// check if the object exists in the dom
				AfMDomElement *ocDomObject = nil;
				if (parent != nil)
				{
					for (AfMDomElement *ocDomItem in parent.children)
					{
						if ([ocDomItem.type isEqualToString:type] && [ocDomItem.identifier isEqualToString:identifier])
						{
							ocDomObject = ocDomItem;
						}
					}
				}

				// create a new object if its new to the dom
				if (ocDomObject == nil)
				{
					ocDomObject = [AfMDomElement new];
					ocDomObject.type = type;
					ocDomObject.identifier = identifier;
				}

				// check if it already exists
				BOOL alreadyExists = NO;
				for(AfMDomElement *ocDomItem in nextDomLevel)
				{
					if ([ocDomItem.type isEqualToString:type] && [ocDomItem.identifier isEqualToString:identifier])
					{
						alreadyExists = YES;
						ocDomObject = ocDomItem;
						break;
					}
				}

				// add it to the dom if it already exists
				if (!alreadyExists)
				{
					[nextDomLevel addObject:ocDomObject];
				}

				// set this element to be the parent of the next element
				parent = ocDomObject;
				nextDomLevel = parent.children;
			}

			//for ( NSTextCheckingResult* match in matches )
			//{
			//	NSString* matchText = [searchedString substringWithRange:[match range]];
			//	NSLog(@"match: %@", matchText);
			//}
		}

		// increment and continue the loop
		index++;
	}
	return [NSDictionary dictionaryWithObject:[ocDom objectAtIndex:0] forKey:@"source"];
}


@end
