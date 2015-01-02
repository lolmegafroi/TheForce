var initialized = false;
var currentLocale = 'de';

Pebble.addEventListener('ready', function(e) {
	console.log("ready called!");
	Pebble.sendAppMessage( { '0': 'de' },
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
	console.log("showing configuration");
	Pebble.openURL('http://wwwpub.zih.tu-dresden.de/~s6430795/the_force_settings.html?locale=' + encodeURIComponent(currentLocale));
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
});
