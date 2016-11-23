from selenium import webdriver
from selenium.webdriver import ActionChains
from random import randint
import utilities

useNativeEvents = 1

windowPath = "/AXApplication[@AXTitle='Calculator']/AXWindow[0]"
resultGroupPath = windowPath + "/AXGroup[0]"
basicGroupPath = windowPath + "/AXGroup[1]"
scientificGroupPath = windowPath + "/AXGroup[2]"
programmerGroupPath = windowPath + "/AXGroup[1]"

print 'Starting the WebDriver session'
desiredCapabilities = {'platform':'Mac', 'commandDelay':50, 'loopDelay':1000, 'implicitTimeout':utilities.defaultImplicitTimeout_sec * 1000, 'mouseMoveSpeed':50, "screenShotOnError":1}
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities=desiredCapabilities)

print 'Opening the "Calculator" app'
driver.get("Calculator")

#print 'Selecting the 1st Window'
#driver.switch_to_window("1")

def numToAXPath(num):
    if num == 0:
        return basicGroupPath + "/AXButton[@AXDescription='zero']"
    elif num == 1:
        return basicGroupPath + "/AXButton[@AXDescription='one']"
    elif num == 2:
        return basicGroupPath + "/AXButton[@AXDescription='two']"
    elif num == 3:
        return basicGroupPath + "/AXButton[@AXDescription='three']"
    elif num == 4:
        return basicGroupPath + "/AXButton[@AXDescription='four']"
    elif num == 5:
        return basicGroupPath + "/AXButton[@AXDescription='five']"
    elif num == 6:
        return basicGroupPath + "/AXButton[@AXDescription='six']"
    elif num == 7:
        return basicGroupPath + "/AXButton[@AXDescription='seven']"
    elif num == 8:
        return basicGroupPath + "/AXButton[@AXDescription='eight']"
    elif num == 9:
        return basicGroupPath + "/AXButton[@AXDescription='nine']"
    else:
        return ""

menuBarAXPath = "/AXApplication[@AXTitle='Calculator']/AXMenuBar"


## Provide a list of menu item titles in the menu bar.
def selectMenuItem(menuItemPath):
    # Start with the menu bar item which has its own role.
    absoluteAXPath = menuBarAXPath + "/AXMenuBarItem[@AXTitle='" + menuItemPath[0] + "']"
    parentElement = driver.find_element_by_xpath(absoluteAXPath)
    parentSize = parentElement.size
    parentLocation = parentElement.location
    ActionChains(driver).move_to_element_with_offset(parentElement, 20, 10).perform()
    ActionChains(driver).click().perform()

    # Now iterate through the menuItems and subMenuItems.
    for i in range(1, len(menuItemPath)):
        absoluteAXPath = absoluteAXPath + "/AXMenu/AXMenuItem[@AXTitle='" + menuItemPath[i] + "']"
        element = driver.find_element_by_xpath(absoluteAXPath)
        eSize = element.size
        eLocation = element.location
        if element.location['x'] <= parentElement.location['x']:
            # The (sub)menu item is either to the left or under the parent menu item. 
            # Move the mouse to the left of the parent menu item first.
            ActionChains(driver).move_to_element_with_offset(parentElement, 1, 10).perform()
            ActionChains(driver).move_to_element_with_offset(element, 20, 10).perform()
        else:
            # The (sub)menu item is to the right of the parent menu item. 
            # Move the mouse to the right of the parent menu item first.
            ActionChains(driver).move_to_element_with_offset(parentElement, parentElement.size['width'] - 1, 10).perform()
            ActionChains(driver).move_to_element_with_offset(element, 20, 10).perform()
        ActionChains(driver).click().perform()
        parentElement = element

#print 'Selecting menu items...'
#selectMenuItem(["Calculator", "About Calculator"])
#selectMenuItem(["View", "Basic"])
#selectMenuItem(["View", "Scientific"])
#selectMenuItem(["View", "Programmer"])
#selectMenuItem(["View", "Basic"])
#selectMenuItem(["View", "Decimal Places", "5"])

#ePI = driver.find_element_by_xpath("/AXApplication[@AXTitle='Calculator']/AXWindow[0]/AXGroup[2]/AXButton[@AXDescription='pi']")
#ePI.click()

def clickElement(element):
    if useNativeEvents > 0:
        # move and click the mouse like a user
        actions = ActionChains(driver)
        actions.click(element)
#        actions.click(element)
#        actions.click(element)
#        actions.click(element)
#        actions.click(element)
        actions.perform()
    else:
        # use the traditional accessibility action
        element.click()

#print 'Moving the mouse around to show off'
#if useNativeEvents > 0:
#    actions = ActionChains(driver)
#    actions.move_to_element(button_clear)
#    actions.perform()

def do_some_calculations_with_clicks():
    print 'Clearing the calculator'
    clickElement(button_clear)

    rand1 = randint(0,1000)
    rand2 = randint(0,1000)

    print 'Entering the first number'
    for num in str(rand1):
        n = numToAXPath(int(num))
        print str(num) + ' --> ' + str(n)
        clickElement(driver.find_element_by_xpath(n))

    print 'Clicking the "+" button'
    clickElement(button_plus)

    print 'Entering the second number'
    for num in str(rand2):
        n = numToAXPath(int(num))
        print str(num) + ' --> ' + str(n)
        clickElement(driver.find_element_by_xpath(n))

    print 'Clicking the "=" button'
    clickElement(button_equals)

    print 'Reading result from screen'
    ActionChains(driver).move_to_element(text_result).perform()
    answer = text_result.text

    if int(answer) == (rand1 + rand2):
      print 'Correct Result: ' + answer
    else:
      print 'Incorect Result: ' + answer

def do_some_calculations_with_keystrokes():
    print 'Clearing the calculator'
    clickElement(button_clear)
    
    rand1 = randint(0,1000)
    rand2 = randint(0,1000)
    
    print 'Typing the first number'
    ActionChains(driver).send_keys(str(rand1)).perform()
    
    print 'Typing the "+" button'
    ActionChains(driver).send_keys("+").perform()
    
    print 'Typing the second number'
    ActionChains(driver).send_keys(str(rand2)).perform()

    print 'Typing the "=" button'
    ActionChains(driver).send_keys("=").perform()
    
    print 'Reading result from screen'
    ActionChains(driver).move_to_element(text_result).perform()
    answer = text_result.text
    
    if int(answer) == (rand1 + rand2):
        print 'Correct Result: ' + answer
    else:
        print 'Incorect Result: ' + answer

print 'Finding Some Elements...'
button_clear = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='clear']")
button_plus = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='add']")
button_equals = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='equals']")
text_result = driver.find_element_by_xpath(resultGroupPath + "/AXStaticText[@AXDescription='main display']")

print 'Doing calculations with native mouse events...'
useNativeEvents = 1
do_some_calculations_with_clicks()

print 'Doing calculations with accessibility actions...'
useNativeEvents = 0
do_some_calculations_with_clicks()

useNativeEvents = 1
print 'Doing calculations with native keystrokes...'
do_some_calculations_with_keystrokes();

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
