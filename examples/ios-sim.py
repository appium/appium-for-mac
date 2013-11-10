from selenium import webdriver
from random import randint
from time import sleep

print 'Starting the WebDriver session'
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities={'platform':'Mac'})

print 'Opening the "iPhone Simulator" app'
driver.get("iPhone Simulator")

print 'Selecting the 1st Window'
driver.switch_to_window("1")

print 'Launching the Contacts App'
contactsIcon = driver.find_element_by_name('Contacts')
contactsIcon.click()
sleep(1)

print 'Pressing the "Add Contact" button'
addContactButton = driver.find_element_by_name('Add')
addContactButton.click()
sleep(.5)

print 'Entering a first name'
fnameField = driver.find_element_by_xpath("//*[@AXValue='First']")
fnameField.click()
driver.execute_script('tell application "System Events" to tell process "iOS Simulator" to keystroke "Dan"')

print 'Entering a last name'
lnameField = driver.find_element_by_xpath("//*[@AXValue='Last']")
lnameField.click()
driver.execute_script('tell application "System Events" to tell process "iOS Simulator" to keystroke "Cuellar"')

print 'Entering a company'
companyField = driver.find_element_by_xpath("//*[@AXValue='Company']")
companyField.click()
driver.execute_script('tell application "System Events" to tell process "iOS Simulator" to keystroke "Zoosk"')

print 'Entering a mobile number'
mobileField = driver.find_element_by_xpath("//*[@AXValue='Phone']")
mobileField.click()
driver.execute_script('tell application "System Events" to tell process "iOS Simulator" to keystroke "5551234567"')

print 'Pressing Done'
doneButton = driver.find_element_by_name('Done')
doneButton.click()

print 'Exiting the app'
# press the home button to exit the app
driver.execute_script('tell application "System Events" to tell process "iOS Simulator" to keystroke "H" using {command down, shift down}')

print 'Quitting webdriver\n'
driver.quit()
