from selenium import webdriver

driver = webdriver.Remote( command_executor='http://localhost:8080/wd/hub', desired_capabilities={'platform':'Mac'})

driver.get("Calculator")

e2 = driver.find_element_by_name('2')
eplus = driver.find_element_by_name('+')
eequals = driver.find_element_by_name('=')

e2.click()
eplus.click()
e2.click()
eequals.click()

source = driver.page_source
print source

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
