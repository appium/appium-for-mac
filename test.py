from selenium import webdriver

driver = webdriver.Remote( command_executor='http://localhost:8080/wd/hub', desired_capabilities={'platform':'Mac'})

source = driver.page_source
print source

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
