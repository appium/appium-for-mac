//
//  AfMStatusCodes.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/18/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMStatusCodes.h"

NSString * const kAfMStatusCodeMessages[] =
{
	/*00*/ @"Success	 The command executed successfully.",
	/*01*/ @"",
	/*02*/ @"",
	/*03*/ @"",
	/*04*/ @"",
	/*05*/ @"",
	/*06*/ @"A session is either terminated or not started",
	/*07*/ @"An element could not be located on the page using the given search parameters.",
	/*08*/ @"A request to switch to a frame could not be satisfied because the frame could not be found.",
	/*09*/ @"The requested resource could not be found, or a request was received using an HTTP method that is not supported by the mapped resource.",
	/*10*/ @"An element command failed because the referenced element is no longer attached to the DOM.",
	/*11*/ @"An element command could not be completed because the element is not visible on the page.",
	/*12*/ @"An element command could not be completed because the element is in an invalid state (e.g. attempting to click a disabled element).",
	/*13*/ @"An unknown server-side error occurred while processing the command.",
	/*14*/ @"",
	/*15*/ @"An attempt was made to select an element that cannot be selected.",
	/*16*/ @"",
	/*17*/ @"An error occurred while executing user supplied JavaScript.",
	/*18*/ @"",
	/*19*/ @"An error occurred while searching for an element by XPath.",
	/*20*/ @"",
	/*21*/ @"An operation did not complete before its timeout expired.",
	/*22*/ @"",
	/*23*/ @"A request to switch to a different window could not be satisfied because the window could not be found.",
	/*24*/ @"An illegal attempt was made to set a cookie under a different domain than the current page.",
	/*25*/ @"A request to set a cookie's value could not be satisfied.",
	/*26*/ @"A modal dialog was open, blocking this operation",
	/*27*/ @"An attempt was made to operate on a modal dialog when one was not open.",
	/*28*/ @"A script did not complete before its timeout expired.",
	/*29*/ @"The coordinates provided to an interactions operation are invalid.",
	/*30*/ @"IME was not available.",
	/*31*/ @"An IME engine could not be started.",
	/*32*/ @"Argument was an invalid selector (e.g. XPath/CSS).",
	/*33*/ @"A new session could not be created.",
	/*34*/ @"Target provided for a move action is out of bounds."
};