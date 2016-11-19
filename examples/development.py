from selenium import webdriver
from selenium.webdriver import ActionChains
from random import randint
import utilities

print 'Starting the WebDriver session'
desiredCapabilities = {'platform':'Mac', 'commandDelay':50, 'loopDelay':1000, 'implicitTimeout':utilities.defaultImplicitTimeout_sec * 1000, 'mouseMoveSpeed':75}
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities=desiredCapabilities)

#print 'Opening the "Calculator" app'
#driver.get("Calculator")
##'\xe2'
#
##print driver.desired_capabilities
##utilities.selectMenuItem(driver, "Calculator", ["Calculator", "About Calculator"])
##utilities.selectMenuItem(driver, "Calculator", ["View", "Show Thousands Separators"], "")
#utilities.selectMenuItem(driver, "Calculator", ["View", "Show Thousands Separators"], "&&@AXMenuItemMarkChar!=''")
#utilities.selectMenuItem(driver, "Calculator", ["File", "Quit Calculator"], "||@AXTitle='Quit and Close All Windows'")

driver.get("QuickBooks")
utilities.selectMenuItem(driver, "QuickBooks", ["Debug", "Configuration", "Do Not Load Remote Config File"], "&&@AXMenuItemMarkChar=''")
utilities.selectMenuItem(driver, "QuickBooks", ["Debug", "Configuration", "Do Not Load Remote Config File"], "&&@AXMenuItemMarkChar!=''")
utilities.selectMenuItem(driver, "QuickBooks", ["Debug", "QuickView", "Services", "Use Mock Email Feed"], "&&@AXMenuItemMarkChar=''")
utilities.selectMenuItem(driver, "QuickBooks", ["Debug", "QuickView", "Services", "Use Mock Email Feed"], "&&@AXMenuItemMarkChar!=''")

#selectedPath = utilities.selectMenuItem(driver, "Calculator", ["View", "Decimal Places", "15"], "@AXMenuItemMark='(null)'")
#print selectedPath

# Open the View menu and leave it open
#viewMenuPath = utilities.selectMenuItem(driver, "Calculator", ["View", "Basic"], "[@AXMenuItemMark='']")
#driver.implicitly_wait(0)
#try:
#    menuItem = driver.find_element_by_xpath(viewMenuPath + "/AXMenu/AXMenuItem[@AXTitle='Basic']")
#    # select the item if it has no checkmark
#    markChar = menuItem.get_attribute("AXMenuItemMarkChar")
##    if type(markChar) is str:
#    ActionChains(driver).click(menuItem).perform()
#except:
#    print "didn't find"

### Provide a list of menu item titles in the menu bar.
#def selectMenuItem(menuItemPath):
#    # Start with the menu bar item which has its own role.
#    absoluteAXPath = menuBarAXPath + "/AXMenuBarItem[@AXTitle='" + menuItemPath[0] + "']"
#    parentElement = driver.find_element_by_xpath(absoluteAXPath)
#    parentSize = parentElement.size
#    parentLocation = parentElement.location
#    ActionChains(driver).move_to_element_with_offset(parentElement, 20, 10).perform()
#    ActionChains(driver).click().perform()
#
#    # Now iterate through the menuItems and subMenuItems.
#    for i in range(1, len(menuItemPath)):
#        absoluteAXPath = absoluteAXPath + "/AXMenu/AXMenuItem[@AXTitle='" + menuItemPath[i] + "']"
#        element = driver.find_element_by_xpath(absoluteAXPath)
#        eSize = element.size
#        eLocation = element.location
#        if element.location['x'] <= parentElement.location['x']:
#            # The (sub)menu item is either to the left or under the parent menu item. 
#            # Move the mouse to the left of the parent menu item first.
#            # This avoids passing over other menu items, which can change the highlighted menu item.
#            ActionChains(driver).move_to_element_with_offset(parentElement, 1, 10).perform()
#            ActionChains(driver).move_to_element_with_offset(element, 20, 10).perform()
#        else:
#            # The (sub)menu item is to the right of the parent menu item. 
#            # Move the mouse to the right of the parent menu item first.
#            # This avoids passing over other menu items, which can change the highlighted menu item.
#            ActionChains(driver).move_to_element_with_offset(parentElement, parentElement.size['width'] - 1, 10).perform()
#            ActionChains(driver).move_to_element_with_offset(element, 20, 10).perform()
#        ActionChains(driver).click().perform()
#        parentElement = element

#print 'Selecting menu items...'
#selectMenuItem(["Calculator", "About Calculator"])
#selectMenuItem(["View", "Basic"])
#selectMenuItem(["View", "Scientific"])
#selectMenuItem(["View", "Programmer"])
#selectMenuItem(["View", "Basic"])
#selectMenuItem(["View", "Decimal Places", "5"])

def clickElement(element):
    if useNativeEvents > 0:
        # move and click the mouse like a user
        actions = ActionChains(driver)
        actions.click(element)
        actions.perform()
    else:
        # use the traditional accessibility action
        element.click()


# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
