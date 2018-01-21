# coding=utf-8

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.keys import Keys
from random import randint
import time
import utilities

kLineHeight = 33 # Monaco Regular 24
kLineStartInsetX = 3 # inset from the textView left border, for selecting lines
welcomeLineY = 20
welcomeX = 300
messageLineY = 80
global headerWindow

defaultLoopDelay_sec = 1.00
defaultCommandDelay_sec = 0.100
defaultImplicitTimeout_sec = 5.000
defaultMouseSpeed = 40
defaultScreenShotOnError = True
defaultGlobalDiagnosticsDirectory = '~/Desktop/'

defaultCookies = [
    {'name': 'loop_delay', 'value': defaultLoopDelay_sec},
    {'name': 'command_delay', 'value':defaultCommandDelay_sec },
    {'name': 'implicit_timeout', 'value': defaultImplicitTimeout_sec},
    {'name': 'mouse_speed', 'value': defaultMouseSpeed},
    {'name': 'screen_shot_on_error', 'value': True},
    {'name': 'global_diagnostics_directory', 'value': defaultGlobalDiagnosticsDirectory}
]
driver = utilities.startWebDriverSessionCookies('http://localhost:4622/wd/hub', defaultCookies)

# Set up a new window with Rich Text and monospaced font.
# Returns: True if the window was created and set up correctly.
def createWindow(x, y, width, height):
    #    ActionChains(driver).key_down(Keys.COMMAND).send_keys("n").key_up(Keys.COMMAND).perform()
    
    window = utilities.findElementByXPath(driver, "/AXApplication[@AXTitle='TextEdit']/AXWindow[0]")
    if window == None:
        print "No front window!"
        return False
    driver.switch_to.window
    
    driver.set_window_position(x, y, "current")
    driver.set_window_size(width, height, "current")
    
    return True

def dragWordInTextView(view, viewOffsetX, viewOffsetY, dragOffsetX, dragOffsetY):
    ActionChains(driver).move_to_element_with_offset(view, viewOffsetX, viewOffsetY).double_click().perform()
    time.sleep(0.5) # avoid unwanted triple-click
    
    # Save current mouse speed
    print driver.get_cookies()
    savedMouseCookie = driver.get_cookie('mouse_speed')
    print savedMouseCookie
    driver.add_cookie({'name': 'mouse_speed', 'value': 20})
    print driver.get_cookies()
    
    ActionChains(driver).click_and_hold().perform()
    time.sleep(1.5) # give time for the dragging session to start (cursor == arrow)
    ActionChains(driver).move_by_offset(dragOffsetX, dragOffsetY).release().perform()

    # Restore mouse speed
    driver.add_cookie(savedMouseCookie)

# Replace a message line. 
def replaceLineInTextView(view, newText, lineY):
    driver.get("TextEdit")
    # drag-select the line (Selenium can't triple-click, dang it!)
    ActionChains(driver).move_to_element_with_offset(view, kLineStartInsetX, lineY).click_and_hold().move_by_offset(0, kLineHeight).release().perform()
    ActionChains(driver).send_keys(newText).perform()

def createHeaderWindow():
    global headerWindow
    # Create a header window
    if createWindow(200, 200, 600, 300) == False:
        quit()
    headerWindow = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]")

# Given a window, clicks in its text view, clears the text view.
# Returns: the text view
def clickAndClearWindowTextView(window):
    #print "clickAndClearWindowTextView: " + window.get_attribute("AXTitle")
    action = ActionChains(driver).click(window)
    action.key_down(Keys.COMMAND).send_keys("a").key_up(Keys.COMMAND).send_keys(Keys.BACKSPACE).perform()
    textView = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXScrollArea[0]/AXTextArea[@AXIdentifier='First Text View']")
    return textView

def showWelcomeText(window):
    textView = clickAndClearWindowTextView(window)
    
    # Type an intro with an improperly-ordered word, then drag the word to its proper location.
    ActionChains(driver).send_keys("to AppiumForMac Welcome!!" + Keys.RETURN).perform()
    time.sleep(1)
    ActionChains(driver).send_keys(Keys.RETURN + "oops, sorry, hang on...").perform()
    time.sleep(1)

    # Drag the word "Welcome" to the beginning of the title line.
    dragWordInTextView(textView, welcomeX, welcomeLineY, kLineStartInsetX - welcomeX, 0)
    time.sleep(1)
    replaceLineInTextView(textView, "Much better.", messageLineY)
    time.sleep(1)

            
# The demo starts here.
driver.get("TextEdit")
time.sleep(1)
createHeaderWindow()
driver.find_element_by_xpath("/ASDF")

# quit the webdriver instance
print 'Quitting the WebDriver session'
driver.quit()

