//
// Web Scraping and ScreenShooting in Headless Chrome (CLI)
// http://blog.malcom.pl/2020/web-scraping-i-screenshooting-w-headless-chrome-cli.html
//
// Copyright (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>
// Released under the MIT License
//
// Example script using the 'puppeteer' node module
//

const puppeteer = require('puppeteer');

const url = "https://www.google.com/";
const filename = "screenshot.png";

(async () => {

	// hack to work on windows with oth
	const browser = await puppeteer.launch({
		headless: true,
		//executablePath: 'C:/Program Files (x86)/Google/Chrome/Application/chrome.exe',
		// hack to work on Win
		args: [
			'--no-sandbox',
		]
	});

	const page = await browser.newPage();

	await page.goto(url, { waitUntil: 'load' });
	console.log('Title: ' + await page.title());

	await page.screenshot({ path: filename, fullPage: true });
	console.log('Screenshot: ' + filename);

	await browser.close();

})();
