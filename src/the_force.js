var initialized = false;

Pebble.addEventListener('ready', function(e) {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  console.log("sending message to pebble to retreive current locale");

  var transactionId = Pebble.sendAppMessage( { '0': 'de' },
    function(e) {
      console.log('Successfully delivered message with transactionId=' + e.data.transactionId);
    },
    function(e) {
      console.log('Unable to deliver message with transactionId=' + e.data.transactionId + ' Error is: ' + e.error.message);
    }
  );

  console.log("showing configuration");
  Pebble.openURL('http://wwwpub.zih.tu-dresden.de/~s6430795/the_force_settings.html?locale=');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  var options = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(options));
});

/*
<!DOCTYPE html>
<html>
  <head>
    <title>"The Force" Watchface Configuration</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.css" />
    <script src="http://code.jquery.com/jquery-1.9.1.min.js"></script>
    <script src="http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.js"></script>
  </head>
  <body>
    <div data-role="page" id="main">
      <div data-role="header" class="jqm-header">
        <h1>&quot;The Force&quot; Watchface Configuration</h1>
      </div>

      <div data-role="content">

        <div data-role="fieldcontain">
          <label for="name">Name:</label>
          <textarea cols="40" rows="8" name="name" id="name"></textarea>
        </div>

        <div data-role="fieldcontain">
          <label for="special-feature">Activate special feature:</label>
          <select name="special-feature" id="special-feature" data-role="slider">
            <option value="off">Off</option>
            <option value="on">On</option>
          </select>
        </div>

        <div data-role="fieldcontain">
          <fieldset data-role="controlgroup">
            <legend>Choose as many snacks as you'd like:</legend>
            <input type="checkbox" name="checkbox-cheetos" id="checkbox-cheetos" class="custom" />
            <label for="checkbox-cheetos">Cheetos</label>

            <input type="checkbox" name="checkbox-doritos" id="checkbox-doritos" class="custom" />
            <label for="checkbox-doritos">Doritos</label>

            <input type="checkbox" name="checkbox-fritos" id="checkbox-fritos" class="custom" />
            <label for="checkbox-fritos">Fritos</label>

            <input type="checkbox" name="checkbox-sunchips" id="checkbox-sunchips" class="custom" />
            <label for="checkbox-sunchips">Sun Chips</label>
            </fieldset>
          </div>
        </div>

        <div class="ui-body ui-body-b">
          <fieldset class="ui-grid-a">
              <div class="ui-block-a"><button type="submit" data-theme="d" id="b-cancel">Cancel</button></div>
              <div class="ui-block-b"><button type="submit" data-theme="a" id="b-submit">Submit</button></div>
            </fieldset>
          </div>
        </div>
      </div>
    </div>
    <script>
      function saveOptions() {
        var options = {
          'name': $("#name").val(),
          'special-feature': $("#special-feature").val(),
          'checkbox-cheetos': $("#checkbox-cheetos").is(':checked'),
          'checkbox-doritos': $("#checkbox-doritos").is(':checked'),
          'checkbox-fritos': $("#checkbox-fritos").is(':checked'),
          'checkbox-sunchips': $("#checkbox-sunchips").is(':checked')
        }
        return options;
      }

      $().ready(function() {
        $("#b-cancel").click(function() {
          console.log("Cancel");
          document.location = "pebblejs://close";
        });

        $("#b-submit").click(function() {
          console.log("Submit");

          var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
          console.log("Warping to: " + location);
          console.log(location);
          document.location = location;
        });

      });
    </script>
  </body>
</html>
*/