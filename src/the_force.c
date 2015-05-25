#include <pebble.h>

// pebble display size is 144 * 168 (width * height)

Window *window;
TextLayer *tl_time = 0;
TextLayer *tl_date = 0;
TextLayer *tl_bt = 0;
TextLayer *tl_charge = 0;
BitmapLayer *bitmap_layer = 0;

GBitmap *bitmap = 0;
const unsigned int NUM_BITMAPS = 7;
uint8_t currentImage = 0;
const uint32_t bitmap_IDs[] = {RESOURCE_ID_star_wars, RESOURCE_ID_darth_vader, RESOURCE_ID_han_solo, RESOURCE_ID_boba_fett, RESOURCE_ID_millenium_falcon, RESOURCE_ID_death_star, RESOURCE_ID_jabba};

// helper table for custom to-lower functionality
const char tolower_table[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };
// weekdays: double-array map from locale to weekday name
const char weekdays[3][7][3] = {
	{ "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" },
	{ "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa" },
	{ "Di", "Lu", "Ma", "Me", "Je", "Ve", "Sa" }
};
// current supported locales are: en, de, fr
const char* locales[] = { "en", "de", "fr" };
#define NUM_LOCALES 3
// currently supported fonts
const char* fonts[] = {"sys", "rebellion", "scp-regular", "scp-bold"};
#define NUM_FONTS 4

typedef struct {
	char locale[6];
	int8_t idx_lang;
	GFont font_14;
	GFont font_28;
	int8_t current_font;
	int8_t next_font;
} configuration;
configuration global_conf;

// persistence constants
const uint32_t PERSISTENCE_ID_LOCALE = 0;
const uint32_t PERSISTENCE_ID_FONT   = 1;

// message constants
#define MSG_KEY_LOCALE_GET       0
#define MSG_KEY_LOCALE_SET       1
#define MSG_KEY_FONT_GET         2
#define MSG_KEY_FONT_SET         3

void update_time() {
	static char str_time[] = "00:00:00";
	static const size_t str_time_max = 9;
	// Get a tm structure
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);
	size_t ret;
	if(clock_is_24h_style() == true) { // Use 24 hour format
		ret = strftime(str_time, str_time_max, "%H:%M", tick_time);
	} else { // Use 12 hour format
		ret = strftime(str_time, str_time_max, "%P %I:%M", tick_time);
	}
	if (ret == 0) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Error getting time");
	}
	// test code
	// text_layer_set_text(tl_time, "56:78");
	text_layer_set_text(tl_time, str_time);
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
		strncpy(global_conf.locale, loc, (pos - loc));
	} else {
		strcpy(global_conf.locale, loc);
	}
	char* p = global_conf.locale;
	for ( ; *p; ++p) {
		*p = tolower_table[(uint8_t)*p];
	}
	if (strcmp(global_conf.locale, "en") == 0) {
		global_conf.idx_lang = 0;
	} else if (strcmp(global_conf.locale, "de") == 0) {
		global_conf.idx_lang = 1;
	} else if (strcmp(global_conf.locale, "fr") == 0) {
		global_conf.idx_lang = 2;
	} else {
		// system default
		global_conf.idx_lang = 0;
	}
	char num[2];
	num[0] = 48 + global_conf.idx_lang;
	num[1] = 0;
	APP_LOG(APP_LOG_LEVEL_INFO, global_conf.locale);
	APP_LOG(APP_LOG_LEVEL_INFO, num);
}

void update_date() {
	// update_locale();
	static char str_date[] = "12345678901234567890"; // 20 chars should be fairly enough
	time_t t = time(NULL); 
	struct tm *lt = localtime(&t);
	const char* wd = weekdays[global_conf.idx_lang][lt->tm_wday];
	char* fmt_date = 0;
	switch (global_conf.idx_lang) {
		case 0: fmt_date = "dd %m/%d/%Y"; break;
		case 1: fmt_date = "dd %d.%m.%Y"; break;
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
	snprintf(buf, sizeof(buf), "%d%%\n%c", charge_state.charge_percent, charge_state.is_charging ? 'c' : charge_state.is_plugged ? 'p' : ' ');
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

void update_fonts() {
	switch (global_conf.current_font) {
		case -1:
		case 0:
			break;
		case 1: // rebellion
		case 2: // source code pro regular
		case 3: // source code pro bold
			fonts_unload_custom_font(global_conf.font_14);
			fonts_unload_custom_font(global_conf.font_28);
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Unknown current font!");
	}
	bool doSetFont = true;
	switch (global_conf.next_font) {
		case -1:
			// do not set a new font - used e.g. for shutting down
			doSetFont = false;
			break;
		case 0:
			global_conf.font_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
			global_conf.font_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);
			break;
		case 1:
			global_conf.font_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONTS_KEY_REBELLION_14));
			global_conf.font_28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONTS_KEY_REBELLION_28));
			break;
		case 2:
			global_conf.font_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONTS_KEY_SOURCECODEPRO_REGULAR_14));
			global_conf.font_28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONTS_KEY_SOURCECODEPRO_REGULAR_28));
			break;
		case 3:
			global_conf.font_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONTS_KEY_SOURCECODEPRO_BOLD_14));
			global_conf.font_28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONTS_KEY_SOURCECODEPRO_BOLD_28));
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Unknown next font!");
			doSetFont = false;
	}
	if (doSetFont) {
		text_layer_set_font(tl_bt,      global_conf.font_14);
		text_layer_set_font(tl_charge,  global_conf.font_14);
		text_layer_set_font(tl_time,    global_conf.font_28);
		text_layer_set_font(tl_date,    global_conf.font_14);
		global_conf.current_font = global_conf.next_font;
	}
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

void inbox_received_handler(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "inboxReceived");

	static char s_msg_result[160];
	int idx_msg_result = 0;
	DictionaryIterator * pDictIter = NULL;
	Tuple *t = dict_read_first(iterator);
	while (t != NULL) {
		if (t->key == MSG_KEY_LOCALE_GET) {
			if (pDictIter == NULL && (app_message_outbox_begin(&pDictIter) != APP_MSG_OK)) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "Could not get DictionaryIterator for MSGKEY_LOCALE_GET");
			} else {
				dict_write_cstring(pDictIter, MSG_KEY_LOCALE_GET, global_conf.locale);
				idx_msg_result += snprintf(s_msg_result + idx_msg_result, 32, "[get locale:%s]", global_conf.locale);
			}
		} else if (t->key == MSG_KEY_FONT_GET) {
			if (pDictIter == NULL && (app_message_outbox_begin(&pDictIter) != APP_MSG_OK)) {
				APP_LOG(APP_LOG_LEVEL_ERROR, "Could not get DictionaryIterator for MSGKEY_LOCALE_GET");
			} else {
				dict_write_cstring(pDictIter, MSG_KEY_FONT_GET, fonts[global_conf.current_font]);
				idx_msg_result += snprintf(s_msg_result + idx_msg_result, 32, "[get font:%s]", fonts[global_conf.current_font]);
			}
		} else if (t->key == MSG_KEY_LOCALE_SET) {
			char* loc = t->value->cstring;
			if (strlen(loc) == 0 || strcmp(loc, "sys") == 0) {
				persist_delete(PERSISTENCE_ID_LOCALE);
				idx_msg_result += snprintf(s_msg_result + idx_msg_result, 32, "[reset locale:%s]", loc);
				update_locale();
				update_date();
				update_time();
			} else if (strcmp(loc, "de") == 0 || strcmp(loc, "en") == 0 || strcmp(loc, "fr") == 0) {
				persist_write_string(PERSISTENCE_ID_LOCALE, loc);
				idx_msg_result += snprintf(s_msg_result + idx_msg_result, 32, "[set locale:%s]", loc);
				update_locale();
				update_date();
				update_time();
			} else {
				APP_LOG(APP_LOG_LEVEL_ERROR, "Unrecognized locale to set!");
			}
		} else if (t->key == MSG_KEY_FONT_SET) {
			char* font = t->value->cstring;
			global_conf.next_font = -1; // do nothing if setting is not recognized
			for (short i = 0; i < NUM_FONTS; ++i) {
				if (strcmp(font, fonts[i]) == 0) {
					global_conf.next_font = i;
					idx_msg_result += snprintf(s_msg_result + idx_msg_result, 32, "[set font:%s]", fonts[global_conf.next_font]);
					break;
				}
			}
			if (global_conf.next_font >= 0) {
				persist_write_int(PERSISTENCE_ID_FONT, global_conf.next_font);
			}
			update_fonts();
		}
		// Get next pair, if any
    	t = dict_read_next(iterator);
	}
	// batch reply
	if (pDictIter != NULL) {
		dict_write_end(pDictIter);
		AppMessageResult res = app_message_outbox_send();
		idx_msg_result += snprintf(s_msg_result + idx_msg_result, 32, "[AppMessageresult:%d]", res);
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG, s_msg_result);
}

void inbox_dropped_handler(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "inboxDropped");
}

void outbox_sent_handler(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "outboxSent");
}

void outbox_failed_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "outboxFailed");
}

void main_window_load(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load");

	// create bluetooth info layer
	tl_bt = text_layer_create(GRect(0, 0, 40, 16));
	text_layer_set_background_color(tl_bt, GColorClear);
	text_layer_set_text_alignment(tl_bt, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_bt));
  
	// create charge state info layer
	tl_charge = text_layer_create(GRect(0, 16, 40, 32));
	text_layer_set_background_color(tl_charge, GColorClear);
	text_layer_set_text_alignment(tl_charge, GTextAlignmentLeft);
	text_layer_set_overflow_mode(tl_charge, GTextOverflowModeWordWrap);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_charge));

	// create time info layer
	tl_time = text_layer_create(GRect(40, 0, 104, 32));
	text_layer_set_background_color(tl_time, GColorClear);
	text_layer_set_text_alignment(tl_time, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_time));

	// create date info layer
	tl_date = text_layer_create(GRect(20, 32, 124, 16));
	text_layer_set_background_color(tl_date, GColorClear);
	text_layer_set_text_alignment(tl_date, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(tl_date));

	// create bitmap layer
	bitmap_layer = bitmap_layer_create(GRect(0, 48, 144, 120));
	bitmap_layer_set_background_color(bitmap_layer, GColorWhite);
	bitmap_layer_set_alignment(bitmap_layer, GAlignBottomRight);
	// bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpAssignInverted);
	bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpAssign);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bitmap_layer));

	// create and set fonts
	global_conf.current_font = -1;
	if (persist_exists(PERSISTENCE_ID_FONT)) {
		global_conf.next_font = persist_read_int(PERSISTENCE_ID_FONT);
	} else {
		global_conf.next_font = 0;
	}
	update_fonts();
  
	// update all info layers's texts
	update_bt(bluetooth_connection_service_peek());
	update_charge(battery_state_service_peek());
	update_locale();
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
	app_message_register_inbox_received(inbox_received_handler);
	app_message_register_inbox_dropped(inbox_dropped_handler);
	app_message_register_outbox_sent(outbox_sent_handler);
	app_message_register_outbox_failed(outbox_failed_handler);
	if (app_message_open(32,32) != APP_MSG_OK) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Could not allocate app message buffers!");
	}
  
	APP_LOG(APP_LOG_LEVEL_DEBUG, "loaded");
}

void main_window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload");

	text_layer_destroy(tl_time);
	text_layer_destroy(tl_bt);
	text_layer_destroy(tl_charge);
	gbitmap_destroy(bitmap);
	bitmap = 0;
	bitmap_layer_destroy(bitmap_layer);
	global_conf.next_font = -1; // do not set a new font
	update_fonts();
  
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
