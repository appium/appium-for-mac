from selenium import webdriver

print 'Starting the WebDriver session'
driver = webdriver.Remote( command_executor='http://localhost:8080/wd/hub', desired_capabilities={'platform':'Mac'})

print 'Opening the "Calculator" app'
driver.get("Calculator")

print 'Finding Elements...'
e2 = driver.find_element_by_name('2')
eplus = driver.find_element_by_name('+')
eequals = driver.find_element_by_name('=')

print 'Clicking the "2" button'
e2.click()
print 'Clicking the "+" button'
eplus.click()
print 'Clicking the "2" button'
e2.click()
print 'Clicking the "=" button'
eequals.click()

#print 'Page Source'
#source = driver.page_source
#print source

print 'Getting all elements of name "4"'
els = driver.find_elements_by_name('4')
for el in els:
  print el.text

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
