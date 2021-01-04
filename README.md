**Please consider switching to [appium-mac2-driver](https://github.com/appium/appium-mac2-driver) for over macOS 10.15**

**The Appium team will not contribute to this driver**

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

### 10.9 and later
1. Open System Preferences > Security & Privacy.
1. Click the Privacy tab.
1. Click Accessibility in the left hand table.
1. If needed, click the lock to make changes.
1. If you do not see AppiumForMac.app in the list of apps, then drag it to the list from Finder.
1. Check the checkmark next to AppiumForMac.app.

## Build

Newer macOS requires 64 bit binary.
You can build this repository manually to match for your environment with Xcode.
Then, please copy `AppiumForMac.app` in  `/Users/<username>/Library/Developer/Xcode/DerivedData/AppiumForMac-xxxxxxxx/Build/Products/Debug/AppiumForMac.app` to `/Applications`.

## Release

Please push a tag with `v` prefix like `v0.5.0`.
Then, a GitHub Actions will push a new `AppiumForMac.app` as the version as a draft on [Release page](https://github.com/appium/appium-for-mac/releases).
Once you convert the draft to publish, the tag will appear on the release page.

### Download binaries built on CI

[Release page](https://github.com/appium/appium-for-mac/releases) provides `AppiumForMac.app` for each tag.
You also can get the latest `AppiumForMac.app` on each PR.

If you could not start `AppiumForMac.app` by security limitation by macOS,
please try out `xattr -cr /path/to/AppiumForMac.app` command once to remove extende attributes on macOS.
Then, you probably open the `AppiumForMac.app` without error.

## New Features For Scripters (as of April 2017)

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


### Automatic AXPath Recording
Generating an AXPath for an element is easy: while AppiumForMac is running, just use the mouse to point to an element, then press the fn (function) key for a couple of seconds. AppiumForMac will build the AXPath of the element on screen, and copy it to the clipboard. You can then paste it into your script.

### Session Kill Switch
If your script has gone off the rails, you need to stop it. But if a script is running, it can keep typing and clicking as you try to cancel it. Now, while a session is in progress, you can press the fn (function) key for a couple of seconds. AppiumForMac will cancel any open sessions. Your script will receive a "no such session" error.

### New Session Properties
AppiumForMac sessions have several new properties. See below for a description of each. You can read and write the property values while your session is in progress, using cookies! Yes it is an odd way to use cookies, but it is the only way I could think of to support getting and setting generic session properties. There are a couple of existing commands in Selenium, but those are limited to a small set of properties, for example timeouts.

#### Getting and Setting Properties with a Cookie
Each cookie is a dictionary with at least two keys: 'name' and 'value'. The Selenium commands are: get_cookies, get_cookie(), and addCookie().

1. To get all cookies, use driver.get_cookies(). This will return an array of cookie dictionaries.
1. To get one cookie, use driver.get_cookie('cookieName'). This will return the cookie with the name 'cookieName'.
1. To set a cookie, use driver.add_cookie({'name': 'cookieName', 'value': 'cookieValue'}).

#### Implicit Timeouts
AppiumForMac supports standard implicit timeouts when finding an element. The default is 0.0 seconds, that is, there will be only one attempt.

    For example, to set a 20.5 second timeout, use driver.add_cookie({'name': 'implicit_timeout', 'value': 20.5}).

#### Loop Delay
During an implicit timeout to find an element, you can specify how long to wait between attempts. The default is 1.0 seconds. The loop delay occurs _after_ each attempt, so if the element is found on the first attempt, there will be no delay. The total waiting time will not exceed implicit_timeout, regardless of the loop_delay value.

    For example, to set a 1.7 second loop delay, use driver.add_cookie({'name': 'loop_delay', 'value': 1.7}).

#### Command Delay
This provides a simple way to slow down a script if needed, e.g. for debugging. It specifies an arbitrary delay _before_ each command. The default is 0.0 seconds so your script runs at full speed!

    For example, to wait 2.5 seconds before executing each command, use driver.add_cookie({'name': 'command_delay', 'value': 2.5}).

#### Mouse Speed
For native mouse events, this determines how fast the mouse moves across the screen, approximately in points per second. The default is 100. A value of 50 will move the mouse about half as fast as 100.

    For example, to set a very slow speed, use driver.add_cookie({'name': 'mouse_speed', 'value': 5}).

Be careful setting mouse speed too high, or your application may not detect the mouse-over for some elements.

#### Session Diagnostics Directories
AppiumForMac can create a global directory for all diagnostics, named "AppiumDiagnostics". This will contain other items such as per-session diagnostic directories. The per-session directory stores items like screen shots. Set a string file path for the global directory. The default is not to use a diagnostics directory. If you do not set a path, or if you set an empty string for the path, then there will be no diagnostic records, such as screen shots.

    For example, driver.add_cookie({'name': 'global_diagnostics_directory', 'value': '~/Desktop'}).

#### Screen Shots for Errors
AppiumForMac can take a screen shot automatically whenever an element is not found before the implicit timeout. These can be used to analyze errors. The screen shots are stored in the per-session diagnostic directory. The default is not to create screen shots for errors. If you do not set the global_diagnostics_directory cookie, then there will be no screen shots even if you enable them.

    For example, to enable the feature, use driver.add_cookie({'name': 'screen_shot_on_error', 'value': True}).

    For example, to enable the feature, use driver.add_cookie({'name': 'screen_shot_on_error', 'value': False}).


## New Features for Developers (as of April 2017)

### Greatly Simplified Code Pattern for Command Handlers.
The code pattern for handler methods has changed substantially. A new method in AfMSessionController, executeWebDriverCommandWithPath:data:onMainThread:commandBlock:, performs the common tasks, such as looking up the session based on the path id, extracting json parameters, validating elements, and observing timeouts and delays. Your handler becomes a simple code block containing only the unique functional code for that handler.

The name of each handler is dynamically constructed from the http request. All you have to do is name your handler correctly and it will get executed.

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

1. Your code block should return an object and a status value created with either [AppiumMacHTTPJSONResponse responseWithJson:status:session:] or [AppiumMacHTTPJSONResponse responseWithJsonError:session:].
1. If your code block returns responseWithJsonError:kAfMStatusCodeNoSuchElement, then the block (not the entire handler method) will be automatically called again, within the constraints of implicitTimeout, and loopDelay.
1. However, if your block returns nil, then a kAfMStatusCodeNoSuchElement error is _immediately_ returned to the scripter, even if implicitTimeout has not yet been reached.
1. If you want to process thrown exceptions, put a try block in your own code block. Uncaught execution exceptions are logged but not re-thrown.
1. AppiumForMac automatically returns common errors to the scripter so you don't need to worry about them:

    * kAfMStatusCodeNoSuchDriver - there is no session with the sessionId specified in the request path
    * kAfMStatusCodeUnknownCommand - your handler method may have been named improperly
    * kAfMStatusCodeStaleElementReference - a previously located element no longer exists
    * kAfMStatusCodeNoSuchWindow - the current window no longer exists
    * kAfMStatusCodeSessionNotCreatedException - the maximum number of sessions are open

