::
:: Web Scraping and ScreenShooting in Headless Chrome (CLI)
:: http://blog.malcom.pl/2020/web-scraping-i-screenshooting-w-headless-chrome-cli.html
::
:: Copyright (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>
:: Released under the MIT License
::
:: Helper to run a dedicated Chrome version in headless mode
::

set PROFILE="TestProfile"
set CHROME="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"

%CHROME% --profile-directory=%PROFILE_DIR% --headless --remote-debugging-port=9222
