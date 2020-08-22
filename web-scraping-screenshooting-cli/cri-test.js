//
// Web Scraping and ScreenShooting in Headless Chrome (CLI)
// http://blog.malcom.pl/2020/web-scraping-i-screenshooting-w-headless-chrome-cli.html
//
// Copyright (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>
// Released under the MIT License
//
// Example script using the 'chrome-remote-interface' node module
//

const CDP = require('chrome-remote-interface');
const fs = require('fs');

const url = "https://www.google.com/";
const filename = "screenshot.png";

(async () => {

	const client = await CDP();
	const { Page, Runtime } = client;
	await Page.enable();
	await Runtime.enable();

	await Page.navigate({ url });
	await Page.loadEventFired();

	const { result } = await Runtime.evaluate({
		expression: "document.querySelector('title').textContent"
	});
	console.log('Title: ' + result.value);

	// for full page screenshots
	{
		const { Emulation } = client;
		const metrics = await Page.getLayoutMetrics();

		const width = Math.ceil(metrics.contentSize.width);
		const height = Math.ceil(metrics.contentSize.height);

		await Emulation.setDeviceMetricsOverride({
			width,
			height,
			deviceScaleFactor: 1,
			mobile: false,
		});
		await Emulation.setVisibleSize({ width, height });
	}

	const { data } = await Page.captureScreenshot();
	fs.writeFileSync(filename, Buffer.from(data, 'base64'));
	console.log('Screenshot: ' + filename);

	client.close();

})();