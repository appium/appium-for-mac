from selenium import webdriver

driver = webdriver.Remote( command_executor='http://localhost:8080/wd/hub', desired_capabilities={'platform':'Mac'})

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
