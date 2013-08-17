/*
 PFAssistiveError.h
 PFAssistive Framework 3.0.0
 Copyright 2003-2010 Bill Cheeseman. Used by permission.
 All rights reserved.
*/

/* IMPORTANT COPYRIGHT AND LICENSING INFORMATION:
 
 This is a summary of the PreForm Assistive Technology, LLC licenses. For the legally binding terms consult the licenses themselves. The licenses are embedded in the frameworks’ bundles, and they are also available for download at pfiddlesoft.com. 
 
 FREE FOR PERSONAL USE AND FOR DISTRIBUTION AND USE WITH FREE PRODUCTS
 
 The PFiddlesoft Frameworks may be licensed free of charge for personal use, including use during development of any client application or other software. They may also be licensed free of charge for distribution and use with any client application or other software that you distribute to the public free of charge (including freeware as well as free beta or trial versions of a product for which you intend to request or require payment in the future). You are required only to give notice to PreForm Assistive Technologies, LLC, to provide attribution to PreForm Assistive Technologies, LLC in your client application or other software, and to include the copyright notice and license in your client application or other software.
 
 ONE-TIME LICENSING FEE FOR DISTRIBUTION AND USE WITH PAID PRODUCTS
 
 If you distribute the PFiddlesoft Frameworks with or in a client application or other software product for which you request or require payment, or if you distribute a client application or other software product that makes use of the PFiddlesoft Frameworks for which you request or require payment, such as donationware, shareware and commercial applications, or for internal use within a for-profit organization, you must within thirty days of initial distribution of your product pay PreForm Assistive Technologies, LLC a flat one-time license fee of $250 U.S. for each framework that you distribution or use, regardless of the number of units of your product you distribute or use. This fee covers all present and future versions of your product, but any separate and distinct product requires you to pay PreForm Assistive Technologies, LLC an additional licensing fee of $250 U.S. for each framework that you distribute or use, as described above.
 
 An executed license is required both for free distribution or use, and for distribution or use subject to a flat one-time license fee with a product for which you request or require payment. Print the PFAssistive Framework distribution license or the PFEventTaps Framework distribution license or both of them, depending on which of the PFiddlesoft Frameworks you distribute or use, in duplicate. Then fill in the blanks, sign them, and mail them to:
 
 PreForm Assistive Technologies, LLC
 P.O. Box 326
 Quechee, VT 05059-0326
 
 DIFFERENT TERMS APPLY TO LARGE OR ESTABLISHED COMMERCIAL SOFTWARE DEVELOPERS. The source code is available for an additional fee. Contact PreForm Assistive Technologies, LLC for details.
 
 DISCLAIMERS
 
 This Framework is provided on an "AS IS" basis.
 
 PREFORM ASSISTIVE TECHNOLOGIES, LLC MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE FRAMEWORK OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH OTHER PRODUCTS. THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY DISCLAIMED.
 
 IN NO EVENT SHALL PREFORM ASSISTIVE TECHNOLOGIES, LLC BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF THE FRAMEWORK, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF PREFORM ASSISTIVE TECHNOLOGIES, LLC HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. YOUR REMEDY FOR ANY DEFECT OR FAULT IN THE FRAMEWORK IS LIMITED TO REFUND OF THE LICENSE FEE YOU PAID.
*/

#import <Cocoa/Cocoa.h>

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5

#define PFAssistiveErrorDomain		@"com.pfiddlesoft.PFAssistive.ErrorDomain"

// PFAssistiveError userInfo dictionary keys.
#define PFAssistiveDescriptionPrefixErrorKey	@"PFAssistiveDescriptionPrefixErrorKey"
#define PFAssistiveFailureReasonSuffixErrorKey	@"PFAssistiveFailureReasonSuffixErrorKey"
#define PFAssistiveElementErrorKey				@"PFAssistiveElementErrorKey"
#define PFAssistiveAttributeErrorKey			@"PFAssistiveAttributeErrorKey"
#define PFAssistiveActionErrorKey				@"PFAssistiveActionErrorKey"
#define PFAssistiveIsExpectedErrorKey			@"PFAssistiveIsExpectedErrorKey"

#pragma mark -
@interface PFAssistiveError : NSError {
}

@end

#endif
