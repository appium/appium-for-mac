# Appium For Mac

Appium for Mac can control the native user interface of Mac applications using Selenium / WebDriver and the OS X Accessibility API. Check out the example which will control the calculator app.

The WebDriver server is on port 4622. If you build it yourself, you can change this value in AppiumForMacAppDelegate.m.

## Requirements
* Running: Mac OS X 10.7 or later.
* Building: Xcode 7.2.1 or later. 

## Installation
Mac OS X does not allow an application to use the Accessibility API without permission, so you have to enable it manually.

### 10.7
1. Open System Preferences > Universal Access.
1. Check the "Enable access for assistive devices" checkbox.

### 10.8
1. Open System Preferences > Accessibility.
1. Check the "Enable access for assistive devices" checkbox.

### 10.9, 10.10, 10.11, 10.12
1. Open System Preferences > Security & Privacy.
1. Click the Privacy tab.
1. Click Accessibility in the left hand table.
1. If needed, click the lock to make changes.
1. If you do not see AppiumForMac.app in the list of apps, then drag it to the list from Finder.
1. Check the checkmark next to AppiumForMac.app.

## New Features For Scripters (as of November 2016)

You can see the new features in the calculator.py example script.

### Native Events
AppiumForMac will generate OS X native keyboard and mouse events using Action syntax. Several examples are in /examples/calculator.py.

#### click
    ActionChains(driver).click(element).perform()
    
    ActionChains(driver).click().perform()

#### move to
    ActionChains(driver).move_to_element(element).perform()

#### send keys
    ActionChains(driver).send_keys('A string to type.').perform()


### Absolute AXPath
This is a huge performance improvement, although it requires cooperation from the scripter. Instead of using a partial XPath, use a simple absolute AXPath. This is based on the OS X accessibility structure, which you can view in Accessibility Inspector. The paths are "simple" because only a limited subset of XPath syntax is supported. Locating objects by XPath is highly optimized when using absolute AXPaths.

A simple absolute AXPath:

    * is a string with valid XPath syntax
    * uses OS X Accessibility constants, e.g. AXMenuItem or AXTitle
    * can specify any accessible element on the screen, in any application or window, any time
    * begins with /AXApplication, and contains at least one other node
    * does not contain "//", or use a wildcard, or specify multiple paths using |
    * uses predicates with a single integer for an index, or, one or more string comparisons using = and !=
    * may use boolean operators "and" or "or" (mutually exclusive) in between multiple comparisons
    *       there must be a leading and trailing space, e.g. " and "
    * does not use predicate strings containing braces [] or parens ()
    * uses single quotes, not double quotes for attribute strings
    * does not contain spaces, except within attribute string values (optional), and, surrounding boolean operators (required)
    
Notes:

    * The node "/AXApplication" without a predicate denotes the frontmost application.
    * Attribute values nil and '' (empty string) are defined to be equivalent.
    *   For example, if a menu item does not have a mark character (i.e. the value is nil) then @AXMenuItemMarkChar=='' is true.
    * Index predicates are valid for nodes with the same AXRole. For example, AXGroup[0] refers to the first AXGroup, not simply the first child element.

Good examples:

    "/AXApplication[@AXTitle='Calculator']/AXWindow[0]/AXGroup[1]/AXButton[@AXDescription='clear']"
    
    "/AXApplication[@AXTitle='Calculator']/AXMenuBarItems/AXMenuBarItem[@AXTitle='View']/AXMenu/AXMenuItem[@AXTitle='Scientific']"
    
    "/AXApplication/AXMenuBarItems/AXMenuBarItem[@AXTitle='View']/AXMenu/AXMenuItem[@AXTitle='Basic' and @AXMenuItemMarkChar!='']"
    

Bad examples:

    "//AXButton[@AXDescription='clear']"
    (does not begin with /AXApplication, and contains //)
    
    "/AXApplication[@AXTitle='Calculator']/AXWindow[0]/AXButton[@AXDescription='clear']"
    (not an absolute path: missing AXGroup)
    
    "/AXApplication[@AXTitle="Calculator"]/AXWindow[0]"
    (a predicate string uses double quotes)

    "/AXApplication[@AXTitle='Calculator']"
    (path does not contain at least two nodes)

    "/AXApplication[@AXTitle='Calculator']/AXMenuBar/AXMenuBarItems/AXMenuBarItem[@AXTitle='(Window)']"
    (a predicate string contains forbidden characters)

    "/AXApplication[@AXTitle='Calculator']/AXWindow[0]/AXGroup[1]/AXButton[@AXDescription ='clear']"
    (a predicate contain a space before the =)

    "/AXApplication[@AXTitle='Calculator']/AXWindow[position()>3]/AXGroup[1]/AXButton[@AXDescription='clear']"
    (a predicate is not a simple string or integer, and specifies more than one node)

    "/AXApplication/AXMenuBarItems/AXMenuBarItem[@AXTitle='View']/AXMenu/AXMenuItem[@AXTitle='Basic' and@AXMenuItemMarkChar!='']"
    (leading and trailing spaces required for the boolean operator)

    "/AXApplication[@AXTitle="Calculator"]/AXWindow[0]/AXButton[@AXDescription='clear' and @AXEnabled='YES' or @AXDescription='clear all']"
    (predicate uses multiple kinds of boolean operators; use one or more 'and', or, use one or more 'or', but not both)


### Implicit Timeouts
AppumForMac supports standard implicit timeouts when finding an element. Pass a milliseconds value as a desired capability. The default is 0.0 seconds, that is, there will be only one attempt.

    For example, 'implicitTimeout':20000 will retry for 20 seconds before returning an error. 

### Loop Delay
During an implicit timeout to find an element, you can specify how long to wait between attempts. Pass a milliseconds value as a desired capability. The default is 1.0 seconds. The loop delay occurs _after_ each attempt, so if the element is found on the first attempt, there will be no delay. The total waiting time will not exceed implicitTimeout, regardless of the loopDelay value.

    For example, 'loopDelay':1000 will wait 1 second between each attempt. 

### Command Delay
This provides a simple way to slow down a script if needed, e.g. for debugging. It specifies an arbitrary delay _before_ each command. Pass a milliseconds value as a desired capability. The default is 0.0 seconds so your script runs at full speed!

    For example, 'commandDelay':2500 will wait 2.5 seconds before executing each command. 

### Mouse Move Speed
For native mouse events, this determines how fast the mouse moves across the screen, in pixels per second. Several examples are in /examples/calculator.py.

    For example, 'mouseMoveSpeed':100 will move about 2x faster than 'mouseMoveSpeed':50.
    
    Be careful setting mouse speed too high, or your application may not detect the mouse-over for some elements.

### Session Diagnostics Directories
AppiumForMac can create a directory for all diagnostics, named "AppiumDiagnostics". This will contain other items such as per-session diagnostic directories. The per-session directory stores items like screen shots. Pass a string file path in as a desiredCapability. If you do not pass any path, then there will be no diagnostic records, such as screen shots.

    For example, "diagnosticsDirectoryLocation":"~/Documents/" will add a directory named AppiumDiagnostics to your Documents folder. 

### Screen Shots
AppiumForMac can take a screen shot automatically whenever an element is not found before the implicit timeout. These can be used to analyze errors. The screen shots are stored in the per-session diagnostic directory. Pass in a non-zero number as a desiredCapability to enable the feature. If you pass in zero, or do not provide this desiredCapability, then there will be no screen shots.

    For example, "screenShotOnError":1 will enable automatic screen shots when find element fails.
    
    For example, "screenShotOnError":0 will disable screen shots.



## New Features for Developers (as of November 2016)

### Greatly Simplified Code Pattern for Command Handlers.
The code pattern for handler methods has changed substantially. A new method in AfMSessionController, executeWebDriverCommandWithPath:data:onMainThread:commandBlock:, performs the common tasks, such as looking up the session based on the path id, extracting json parameters, validating elements, and observing timeouts and delays. Each handler becomes a simple code block containing only the unique functional code for that handler.

There is no more need to update the giant httpResponseForMethod:URI: method. The name of each handler is dynamically constructed from the http request. All you have to do is name your handler correctly and it will get executed.

#### Creating a new command handler

1. Go to AfMHandlers.m.
1. Find an existing handler method. You might want to find one that is similar to the new command you want to add. 
1. Copy the handler method to the clipboard.
1. Find the commented out stub for the new handler method and paste at that location.
1. Rename your new handler, following the naming pattern for the other handlers.
1. Replace the handler block with your new handler code.
1. Update the header file (AfMHandlers.h) so others can easily see which commands are implemented.
1. Update the onMainThread: argument of the executeWebDriverCommandWithPath: method. Most often, your commands do not have to execute on the main thread, so use NO.


#### Runtime for your new command handler

1. Your handler is automatically called whenever AppiumForMac receives its matching wire protocol request.
1. Your handler code block receives three arguments: session, commandParams, and status.

    * session: The AfMSessionController instance created when your script created a new web driver. This provides access to the machine and other applications.
    * commandParams: A dictionary containing http request parameters. It includes elements referenced from the URL path, and all POST json objects. The "elementObject" dictionary key retrieves a validated element.
    * status: A pointer to an int to write back a status code to your caller. This is not a substitute for your handler return value.
    
1. Your code block should return an object created with [AppiumMacHTTPJSONResponse responseWithJson:status:session:] or [AppiumMacHTTPJSONResponse responseWithJsonError:session:].
1. If your code block returns responseWithJsonError:kAfMStatusCodeNoSuchElement, then the block (not the entire handler method) will be automatically called again, within the constraints of implicitTimeout, and loopDelay.
1. However, if your block returns nil, then a kAfMStatusCodeNoSuchElement error is _immediately_ returned to the scripter, even if implicitTimeout has not yet been reached.
1. If you expect thrown exceptions, add them to your own code block. Execution exceptions which you do not catch are logged but not re-thrown.
1. AppiumForMac automatically returns common errors to the scripter so you don't need to worry about them:

    * There is no session with the sessionId specified in the request path.
    * The specified element is invalid.
    * Your handler returned kAfMStatusCodeNoSuchElement, but the implicitTimeout period was exceeded.
    * Your handler doesn't exist (or you named it incorrectly).
    * Your handler returned a nil value.

