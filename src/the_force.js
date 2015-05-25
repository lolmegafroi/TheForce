var initialized = false;
var options = { 
	locale: 'de',
	font: 'sys'
};

function getOptionsFromPebble() {
	Pebble.sendAppMessage( { 'MSGKEY_LOCALE_GET': '', 'MSGKEY_FONT_GET': '' },
		function(e) {
			console.log('Successfully requested current locale and font from Pebble watch');
		},
		function(e) {
			console.log('Unable to request current locale and/or font. Error is: ' + JSON.stringify(e));
		}
	);
}

Pebble.addEventListener('ready', function(e) {
	console.log('ready called!');
	getOptionsFromPebble();
	initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
	console.log('showing configuration with options=' + JSON.stringify(options));
	Pebble.openURL('http://wwwpub.zih.tu-dresden.de/~s6430795/the_force_settings.html?locale=' +
		encodeURIComponent(options.locale) + '&font=' + encodeURIComponent(options.font));
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
	console.log('Options = ' + JSON.stringify(newOps));
	if (newOps.locale && newOps.font) {
		options.locale = newOps.locale;
		options.font = newOps.font;
		// request update and immediately get the actual result back from the watch
		Pebble.sendAppMessage( { 'MSGKEY_LOCALE_SET': options.locale, 'MSGKEY_FONT_SET': options.font },
			function(e) {
				console.log('Successfully sent request to Pebble watch to update options to ' + JSON.stringify(options));
				getOptionsFromPebble();
			},
			function(e) {
				console.log('failed to send request to update options to ' + JSON.stringify(options) + '. Error is: ' + JSON.stringify(e));
			}
		);
	}
});
