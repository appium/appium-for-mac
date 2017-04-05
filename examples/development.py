# coding=utf-8

#
# This script is not a demo or example. Use it when trying out AppiumForMac features, python algorithms, and debugging.
#

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.keys import Keys
from random import randint
import time
import utilities



print 'Starting the WebDriver session'
desiredCapabilities = {'platform':'Mac', 'commandDelay':50, 'loopDelay':0, 'implicitTimeout':utilities.defaultImplicitTimeout_sec * 1000, 'mouseMoveSpeed':60, 'diagnosticsDirectoryLocation':'~/Desktop/', 'screenShotOnError':1}
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities=desiredCapabilities)

print driver.page_source()

#time.sleep(5)
#driver.get("Calculator")
#utilities.selectMenuItemFromMenuBar(driver, "Convert", ["Area…"], "")
# utilities.selectMenuItemFromMenuBar(driver, "Speech", ["Speak Button Pressed"], " and @AXMenuItemMarkChar!=''")
# utilities.selectMenuItemFromMenuBar(driver, "Speech", ["Speak Result"], " and @AXMenuItemMarkChar!=''")


# 
# 
# 
# driver.get("TextEdit")
# 
# #utilities.selectMenuItemFromMenuBar(driver, "Format", ["Text", "Writing Direction", "	Default"], "")
# 
# #utilities.selectMenuItemFromMenuBar(driver, "Format", ["Font", "Hide Fonts"], "")
# 
# # Close all floating windows.
# driver.implicitly_wait(0.5)
# while True:
#     floatWindowCloseButton = utilities.findElementByXPath(driver, "/AXApplication[@AXTitle='TextEdit']/AXWindow[@AXSubrole='AXFloatingWindow']/AXButton[@AXSubrole='AXCloseButton']");
#     if floatWindowCloseButton != None:
#         floatWindowCloseButton.click()
#     else:
#         break
# 
# driver.implicitly_wait(30)
# 
# # Set up a new window with Rich Text and monospaced font.
# if True:
#     ActionChains(driver).key_down(Keys.COMMAND).send_keys("n").key_up(Keys.COMMAND).perform()
#     time.sleep(0.5)
# 
#     window = utilities.findElementByXPath(driver, "/AXApplication[@AXTitle='TextEdit']/AXWindow[0]")
#     if window == None:
#         print "No front window!"
#         quit()
#     driver.switch_to.window
# 
#     driver.set_window_position(50, 50, "current")
#     driver.set_window_size(600, 400, "current")
#     
#     utilities.selectMenuItemFromMenuBar(driver, "Format", ["Make Rich Text"], "")
#     utilities.selectMenuItemFromMenuBar(driver, "Format", ["Font", "Show Fonts"], "")
#     
#     window = utilities.findElementByXPath(driver, "/AXApplication[@AXTitle='TextEdit']/AXWindow[@AXTitle='Fonts']")
#     if window == None:
#         print "No fonts window!"
#         quit()
#     driver.switch_to.window
#     driver.set_window_position(700, 50, "current")
#     driver.set_window_size(600, 400, "current")
#     
#     # Bring Fonts to the front and make it the key window. We need to click in each table and type-select rows.
#     element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXStaticText[@AXValue='Fonts']")
#     ActionChains(driver).click(element).perform()
#     
#     element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXScrollArea[0]/AXTable[0]/AXRow[0]")
#     ActionChains(driver).click(element).send_keys("Fixed Width").perform()
#     
#     element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXScrollArea[2]/AXTable[0]/AXRow[0]")
#     ActionChains(driver).click(element).send_keys("Monaco").perform()
#     
#     element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXScrollArea[1]/AXTable[0]/AXRow[0]")
#     ActionChains(driver).click(element).send_keys("Regular").perform()
#     
#     element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXGroup[0]/AXScrollArea[0]/AXTable[0]/AXRow[0]")
#     ActionChains(driver).click(element).send_keys("24").perform()
#     
#     element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXButton[@AXSubrole='AXCloseButton']")
#     ActionChains(driver).click(element).perform()
# 
# ActionChains(driver).key_down(Keys.COMMAND).send_keys("a").key_up(Keys.COMMAND).send_keys("to AppiumForMac Welcome!!").perform()
# time.sleep(1)
# ActionChains(driver).send_keys(Keys.RETURN + Keys.RETURN + "oops, sorry, hang on...").perform()
# 
# 
# textView = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXScrollArea[0]/AXTextArea[@AXIdentifier='First Text View']")
# if textView == None:
#     quit()
# # Drag the word "Welcome" to the beginning of the line.
# lineStartX = 3
# welcomeLineY = 20
# messageLineY = 80
# welcomeX = 300
# 
# ActionChains(driver).move_to_element_with_offset(textView, welcomeX, welcomeLineY).double_click().click_and_hold().move_by_offset(lineStartX - welcomeX, 0).release().perform()
# 
# utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXButton[@AXSubrole='AXCloseButton']").click()
# utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXSheet[0]/AXButton[@AXTitle='Don’t Save']").click()




#"/AXApplication[@AXTitle='TextEdit']/AXMenuBar[0]/AXMenuBarItem[@AXTitle='Format']/AXMenu[0]/AXMenuItem[@AXTitle='Text']/AXMenu[0]/AXMenuItem[@AXTitle='Writing Direction']/AXMenu[0]/AXMenuItem[@AXTitle='	Default']"
#"/AXApplication/AXMenuBar[0]/AXMenuBarItem[4]/AXMenu[0]/AXMenuItem[1]/AXMenu[0]/AXMenuItem[5]/AXMenu[0]/AXMenuItem[1]"



#utilities.selectMenuItemFromDock(driver, "System Preferences", ["Options", "Show in Finder"], "")
#time.sleep(1)
#ActionChains(driver).key_down(Keys.COMMAND).send_keys("i").key_up(Keys.COMMAND).perform()
#time.sleep(1)
#ActionChains(driver).key_down(Keys.COMMAND).send_keys("w").key_up(Keys.COMMAND).perform()
#ActionChains(driver).key_down(Keys.COMMAND).send_keys("w").key_up(Keys.COMMAND).perform()

#"/AXApplication[@AXTitle='Calculator']/AXMenuBar[0]/AXMenuBarItem[@AXTitle='Apple']/AXMenu[0]/AXMenuItem[@AXTitle='Recent Items']/AXMenu[0]/AXMenuItem[@AXTitle='VMs']"


##'\xe2'
#
##print driver.desired_capabilities

#driver.get("eclipse")
#utilities.selectMenuItemFromMenuBar(driver, "Eclipse", ["Quit Eclipse"], "")



# quit the webdriver instance
print 'Quitting the WebDriver session'
driver.quit()
