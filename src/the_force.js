var initialized = false;
var options = { 
	locale: 'de',
	font: 'sys'
};

Pebble.addEventListener('ready', function(e) {
	console.log('ready called!');
	Pebble.sendAppMessage( { 'MSGKEY_LOCALE_GET': '', 'MSGKEY_FONT_GET': '' },
		function(e) {
			console.log('Successfully requested current locale and font');
		},
		function(e) {
			console.log('Unable to request current locale and/or font. Error is: ' + e.error.message);
		}
	);
	initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
	console.log('showing configuration with options=' + options);
	Pebble.openURL('http://wwwpub.zih.tu-dresden.de/~s6430795/the_force_settings.html?locale=' + encodeURIComponent(options.locale) + '&font=' + options.font);
});

Pebble.addEventListener('appmessage', function(e) {
	console.log('Received message: ' + JSON.stringify(e));
	if (e.payload.MSGKEY_LOCALE_GET) {
		options.locale = e.payload.MSGKEY_LOCALE_GET;
		console.log('Changed current locale to ' + options.locale);
	}
	if (e.payload.MSGKEY_FONT_GET) {
		options.font = e.payload.MSGKEY_FONT_GET;
		console.log('Changed current font to ' + options.font);
	}
});

Pebble.addEventListener('webviewclosed', function(e) {
	console.log('configuration closed');
	var newOps = JSON.parse(decodeURIComponent(e.response));
	console.log('Options = ' + JSON.stringify(options));
	if (options.locale && options.font) {
		options.locale = newOps.locale;
		options.font = newOps.font;
		// request update and immediately get the actual result back from the watch
		Pebble.sendAppMessage( { 'MSGKEY_LOCALE_SET': options.locale, 'MSGKEY_FONT_SET': options.font, 'MSGKEY_LOCALE_GET': '', 'MSGKEY_FONT_GET': '' },
			function(e) {
				console.log('Successfully sent request to update options to ' + options);
			},
			function(e) {
				console.log('failed to send request to update options to ' + options + '. Error is: ' + e.error.message);
			}
		);
	}
});
