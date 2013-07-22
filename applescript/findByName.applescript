on findByName(theElement, theName)
	set theTitle to missing value
	tell application "System Events"
		try
			set theTitle to title of theElement
			if theTitle is equal to theName then
				return theElement
			end if
		end try
		repeat with e in UI elements of theElement
			set theTitle to my findByName(e, theName)
			if theTitle is not missing value then
				return theTitle
			end if
		end repeat
		return missing value
	end tell
end findByName

on run argv
	tell application "System Events"
		return my findByName(process "Appium", "Launch")
	end tell
end run