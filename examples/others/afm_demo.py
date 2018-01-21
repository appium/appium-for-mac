# coding=utf-8

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.keys import Keys
from random import randint
import time
import utilities
import dock_demo

print 'Starting the WebDriver session'

#cookies = [
#           {'name': 'loop_delay', 'value': 1.000},
#           {'name': 'command_delay', 'value':0.100 },
#           {'name': 'implicit_timeout', 'value': utilities.defaultImplicitTimeout_sec},
#           {'name': 'mouse_speed', 'value': 50},
#           {'name': 'screen_shot_on_error', 'value': False},
#           {'name': 'global_diagnostics_directory', 'value': '~/Desktop/'}
#]

httpHost = 'http://localhost:4622/wd/hub'
#httpHost = 'http://devadminsmac.local:4622/wd/hub'
#httpHost = 'http://192.168.209.128:4622/wd/hub'

global driver
driver = utilities.startWebDriverSession(httpHost, None)
print driver


def prepareForDemo():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("Here are some features..." + Keys.RETURN).perform()

def demoMouseSpeed():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoMouseSpeed" + Keys.RETURN).perform()

def demoMenuBar():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoMenuBar" + Keys.RETURN).perform()

def demoPopupMenu():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoPopupMenu" + Keys.RETURN).perform()

def demoNativeModifiersInMenus():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoNativeModifiersInMenus" + Keys.RETURN).perform()

def demoDock():
#    clickAndClearWindowTextView(commentsWindow)
#    ActionChains(driver).send_keys("Dock demo" + Keys.RETURN).perform()
    dock_demo.driver = driver
    dock_demo.performDemo()

def demoAXPathRecording():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoAXPathRecording" + Keys.RETURN).perform()

def demoCalculatorWithNativeMouse():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoCalculatorWithNativeMouse" + Keys.RETURN).perform()

def demoCalculatorWithAXElements():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoCalculatorWithAXElements" + Keys.RETURN).perform()

def demoCalculatorWithNativeKeys():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoCalculatorWithNativeKeys" + Keys.RETURN).perform()

def demoDiagnosticsDirectory():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoDiagnosticsDirectory" + Keys.RETURN).perform()

def demoScreenShotOnError():
    clickAndClearWindowTextView(commentsWindow)
    ActionChains(driver).send_keys("demoScreenShotOnError" + Keys.RETURN).perform()

try:
# The demo starts here.    
#    textedit.setupTextEdit()
    
    # working
#    showWelcomeText(headerWindow)
#    advertiseScripterFeatures()
#    advertiseDeveloperFeatures()

    # not yet working
#    prepareForDemo()
    
#    demoMouseSpeed()
#    demoMenuBar()
#    demoPopupMenu()
#    demoNativeModifiersInMenus()
    demoDock()
#    demoAXPathRecording()
#    demoCalculatorWithNativeMouse()
#    demoCalculatorWithAXElements()
#    demoCalculatorWithNativeKeys()
#    demoDiagnosticsDirectory()
#    demoScreenShotOnError()
except Exception as e:
    print 'Exception in afm_demo: ', e
finally:
    driver.get("TextEdit")
    closeDemoWindows()
    closeWithoutSaving("empty.py")


# quit the webdriver instance
print 'Quitting the WebDriver session'
driver.quit()

