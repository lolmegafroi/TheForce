#include <pebble.h>

// pebble display size is 144 * 168 (width * height)

Window *window;
// TextLayer *text_layer;
TextLayer *tl_time = 0;
TextLayer *tl_date = 0;
TextLayer *tl_bt = 0;
TextLayer *tl_charge = 0;
TextLayer *tl_msg = 0;
GBitmap *bitmap = 0;
BitmapLayer *bitmap_layer = 0;
GFont font_rebellion_28;
GFont font_rebellion_14;
const unsigned int NUM_BITMAPS = 7;
char str_time[] = "00:00";
unsigned int currentImage = 0;
uint32_t bitmap_IDs[] = {RESOURCE_ID_star_wars, RESOURCE_ID_darth_vader, RESOURCE_ID_han_solo, RESOURCE_ID_boba_fett, RESOURCE_ID_millenium_falcon, RESOURCE_ID_death_star, RESOURCE_ID_jabba};
// weekdays: double-array map from locale to weekday name
const char weekdays[3][7][3] = {{"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"},\
{"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"},\
{"Di", "Lu", "Ma", "Me", "Je", "Ve", "Sa"}};
char* current_locale = "12"; // ONLY STORE 2-letter LOCALES!
int current_lang = 0;
char tolower_table[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

// persistence constants
const uint32_t PERSISTENCE_ID_LOCALE = 0;

// message constants
const uint32_t MSG_KEY_LOCALE = 0x0;
const uint8_t MSG_KEY_COUNT = 1;

/*
int n_window_loads = 0;
char str_main[] = "12345678901234567890123456789012"; // buffer of 32+1 chars
char fmt_main[] = "This is the %2d%s update";
char str_st[] = "st";
char str_nd[] = "nd";
char str_rd[] = "rd";
char str_th[] = "th";
*/

void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  if(clock_is_24h_style() == true) { // Use 24 hour format
    strftime(str_time, sizeof(str_time), "%H:%M", tick_time);
  } else { // Use 12 hour format
    strftime(str_time, sizeof(str_time), "%I:%M", tick_time);
  }
  // test code
  // text_layer_set_text(tl_time, "56:78");
  text_layer_set_text(tl_time, str_time);
  /*
  static char logBuf[] = "12345678901234567890123456789012";
  strftime(logBuf, 33, "[TheForce] ticked at %H:%M", tick_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, logBuf);
  */
}

void update_locale() {
  const char* loc;
  if (persist_exists(PERSISTENCE_ID_LOCALE)) {
    char locale[10];
    persist_read_string(PERSISTENCE_ID_LOCALE, locale, sizeof(locale));
    loc = locale;
  } else {
    loc = i18n_get_system_locale();
  }
  char* pos = 0;
  if ((pos = strchr(loc, '_')) != 0) {
    strncpy(current_locale, loc, (pos - loc));
  } else {
    strcpy(current_locale, loc);
  }
  char* p = current_locale;
  for ( ; *p; ++p) {
    *p = tolower_table[(uint8_t)*p];
  }
  current_lang = 0;
  if(strcmp(current_locale, "de") == 0) {
    current_lang = 1;
  } else if (strcmp(current_locale, "fr") == 0) {
    current_lang = 2;
  }
}

void update_date() {
  update_locale();
  static char* str_date = "12345678901234567890"; // 20 chars should be fairly enough
  time_t t = time(NULL); 
  struct tm *lt = localtime(&t);
  const char* wd = weekdays[current_lang][lt->tm_wday];
  char* fmt_date = 0;
  switch (current_lang) {
    case 1:
    case 2: fmt_date = "dd %d.%m.%Y"; break;
    default: fmt_date = "dd %m/%d/%Y";
  }
  strftime(str_date, 21, fmt_date, lt);
  str_date[0] = wd[0];
  str_date[1] = wd[1];
  // test code
  // text_layer_set_text(tl_date, "So 30.12.2014");
  text_layer_set_text(tl_date, str_date);
}

void update_bt(bool connected) {
  if (connected) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is connected!");
    text_layer_set_text(tl_bt, "con");
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is not connected!");
    text_layer_set_text(tl_bt, "dis");
  }
}

void update_charge(BatteryChargeState charge_state) {
  static char buf[] = "1234567890";
  // snprintf(buf, sizeof(buf), "%d %d\n%d%%", charge_state.is_plugged, charge_state.is_charging, charge_state.charge_percent);
  snprintf(buf, sizeof(buf), "%d%% %c", charge_state.charge_percent, charge_state.is_charging ? 'c' : charge_state.is_plugged ? 'p' : ' ');
  // test code
  // text_layer_set_text(tl_charge, "100%");
  text_layer_set_text(tl_charge, buf);
}

void update_bitmap() {
  if (bitmap != 0) {
    gbitmap_destroy(bitmap);
    bitmap = 0;
  }
  bitmap = gbitmap_create_with_resource(bitmap_IDs[currentImage]);
  bitmap_layer_set_bitmap(bitmap_layer, bitmap);
}

void tick_handler_minutes(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  if (tick_time->tm_min == 0) {
    // change the bitmap each hour
    ++currentImage;
    currentImage %= NUM_BITMAPS;
    update_bitmap();
    update_date();
  }
}

void inboxReceived(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "inboxReceived");
}

void inboxDropped(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "inboxDropped");
}

void outboxSent(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "outboxSent");
}

void outboxFailed(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "outboxFailed");
}

void main_window_load(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load");

  /*
  ++n_window_loads;
  char* str = 0;
  switch (n_window_loads % 10) {
    case 1: str = str_st; break;
    case 2: str = str_nd; break;
    case 3: str = str_rd; break;
    default: str = str_th; break;
  }
  snprintf(str_main, 33, fmt_main, n_window_loads, str);
  
  text_layer = text_layer_create(GRect(0, 0, 144, 60));
	// Set the text, font, and text alignment
	text_layer_set_text(text_layer, str_main);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
  */
  
  // create fonts
  font_rebellion_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_REBELLION_14));
  font_rebellion_28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_REBELLION_28));
  
  // create bluetooth info layer
  tl_bt = text_layer_create(GRect(0, 0, 60, 16));
  text_layer_set_background_color(tl_bt, GColorClear);
	text_layer_set_font(tl_bt, font_rebellion_14);
	text_layer_set_text_alignment(tl_bt, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_bt));
  
  // create charge state info layer
  tl_charge = text_layer_create(GRect(0, 16, 60, 16));
  text_layer_set_background_color(tl_charge, GColorClear);
	text_layer_set_font(tl_charge, font_rebellion_14);
	text_layer_set_text_alignment(tl_charge, GTextAlignmentLeft);
  text_layer_set_overflow_mode(tl_charge, GTextOverflowModeWordWrap);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_charge));

  // create time info layer
  tl_time = text_layer_create(GRect(40, 0, 104, 32));
  text_layer_set_background_color(tl_time, GColorClear);
	text_layer_set_font(tl_time, font_rebellion_28);
	text_layer_set_text_alignment(tl_time, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_time));

  // create date info layer
  tl_date = text_layer_create(GRect(0, 32, 144, 16));
  text_layer_set_background_color(tl_date, GColorClear);
	text_layer_set_font(tl_date, font_rebellion_14);
	text_layer_set_text_alignment(tl_date, GTextAlignmentRight);
  // text_layer_set_overflow_mode(tl_date, GTextOverflowModeWordWrap);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_date));

  // create bitmap layer
  bitmap_layer = bitmap_layer_create(GRect(0, 48, 144, 120));
  bitmap_layer_set_background_color(bitmap_layer, GColorWhite);
  bitmap_layer_set_alignment(bitmap_layer, GAlignBottomRight);
  // bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpAssignInverted);
  bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpAssign);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bitmap_layer));
  
  // update all info layers's texts
  update_bt(bluetooth_connection_service_peek());
  update_charge(battery_state_service_peek());
  update_time();
  update_date();
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  currentImage = tick_time->tm_hour % NUM_BITMAPS;
  update_bitmap();
  
  // subscribe to services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler_minutes);
  bluetooth_connection_service_subscribe(update_bt);
  battery_state_service_subscribe(update_charge);
  app_message_register_inbox_received(inboxReceived);
  app_message_register_inbox_dropped(inboxDropped);
  app_message_register_outbox_sent(outboxSent);
  app_message_register_outbox_failed(outboxFailed);
  
	APP_LOG(APP_LOG_LEVEL_DEBUG, "loaded");
}

void main_window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload");

	// text_layer_destroy(text_layer);
	text_layer_destroy(tl_time);
	text_layer_destroy(tl_bt);
	text_layer_destroy(tl_charge);
  gbitmap_destroy(bitmap);
  bitmap = 0;
  bitmap_layer_destroy(bitmap_layer);
  fonts_unload_custom_font(font_rebellion_14);
  fonts_unload_custom_font(font_rebellion_28);
  
  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  app_message_deregister_callbacks();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "unloaded");
}

void init(void) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "init");

	// Create a window and text layer
	window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
	// Push the window
	window_stack_push(window, true);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "initialized");
}

void deinit(void) {	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "deinit");

  // Destroy the window
	window_destroy(window);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "deinitialized");
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}