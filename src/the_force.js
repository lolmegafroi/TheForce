var initialized = false;
var currentLocale = 'de';

Pebble.addEventListener('ready', function(e) {
	console.log("ready called!");
	Pebble.sendAppMessage( { 'MSGKEY_LOCALE_GET': '' },
		function(e) {
			console.log('Successfully requested current locale');
		},
		function(e) {
			console.log('Unable to request current locale. Error is: ' + e.error.message);
		}
	);
	initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
	console.log("showing configuration with locale=" + currentLocale);
	Pebble.openURL('http://wwwpub.zih.tu-dresden.de/~s6430795/the_force_settings.html?locale=locale-' + encodeURIComponent(currentLocale));
});

Pebble.addEventListener('appmessage', function(e) {
	console.log('Received message: ' + JSON.stringify(e));
	if (e.payload.MSGKEY_LOCALE_GET) {
		currentLocale = e.payload.MSGKEY_LOCALE_GET;
		console.log("Changed current locale to " + currentLocale);
	}
});

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("configuration closed");
	var options = JSON.parse(decodeURIComponent(e.response));
	console.log("Options = " + JSON.stringify(options));
	if (options.locale) {
		currentLocale = options.locale;
		// request update and immediately get the actual result back from the watch
		Pebble.sendAppMessage( { 'MSGKEY_LOCALE_SET': currentLocale, 'MSGKEY_LOCALE_GET': '' },
			function(e) {
				console.log('Successfully sent request to update current locale to ' + currentLocale);
			},
			function(e) {
				console.log('failed to send request to update current locale to ' + currentLocale + '. Error is: ' + e.error.message);
			}
		);
	}
});
