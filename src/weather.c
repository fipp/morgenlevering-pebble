#include "pebble.h"

static Window *s_main_window;

static TextLayer *s_temperature_layer;
static TextLayer *s_city_layer;
static BitmapLayer *s_icon_layer;
static GBitmap *s_icon_bitmap = NULL;

static AppSync s_sync;
static uint8_t s_sync_buffer[64];

static char str[12];

static const char * externalProductId;

/*
enum WeatherKey {
  WEATHER_ICON_KEY = 0x0,         // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
  WEATHER_CITY_KEY = 0x2,         // TUPLE_CSTRING
};
*/

enum ProductKey {
  PRODUCT_NAME_KEY = 0x0,
  PRODUCT_ID_KEY = 0x1,
  PRODUCT_PRICE_KEY = 0x2,
  PRODUCT_EXTID_KEY = 0x3
};

static const uint32_t ABO_ICONS[] = {
  RESOURCE_ID_IMAGE_MENUITEM // 0
  /*
  RESOURCE_ID_IMAGE_SUN, // 0
  RESOURCE_ID_IMAGE_CLOUD, // 1
  RESOURCE_ID_IMAGE_RAIN, // 2
  RESOURCE_ID_IMAGE_SNOW // 3
  */
};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received message with key %d", (int) key);
  
  switch (key) {
    /*
    case WEATHER_ICON_KEY:
      if (s_icon_bitmap) {
        gbitmap_destroy(s_icon_bitmap);
      }

      s_icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[new_tuple->value->uint8]);
      bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
      bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
      break;
    */
    
    case PRODUCT_NAME_KEY:
      // App Sync keeps new_tuple in s_sync_buffer, so we may use it directly
      text_layer_set_text(s_temperature_layer, new_tuple->value->cstring);
      break;

    case PRODUCT_PRICE_KEY:
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Received message with value %s", (int) key);
      
      //sprintf(str, "%d", new_tuple->value->uint8);
      snprintf(str,sizeof(str),"Pris: %d",new_tuple->value->uint8);
      //itoa(new_tuple->value->uint8,str, 10);
      text_layer_set_text(s_city_layer, str);
      break;
    
    case PRODUCT_EXTID_KEY:
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Received message with value %s", (int) key);
    
      externalProductId = new_tuple->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "externalId: %s", externalProductId);
    
      //sprintf(str, "%d", new_tuple->value->uint8);
      //snprintf(str,sizeof(str),"Pris: %d",new_tuple->value->uint8);
      //itoa(new_tuple->value->uint8,str, 10);
      //text_layer_set_text(s_city_layer, str);
      break;
    
  }
  
}

static void order_product(void) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }
  
  dict_write_cstring(iter, 3, externalProductId);
  dict_write_end(iter);
  
  //dict_write_uint8 (iter, 0, 0x1);
  //dict_write_cstring (iter, 1, "ABCD");

  app_message_outbox_send();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_city_layer, "Up pressed!");
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(s_city_layer, "Select pressed!");
  order_product();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_city_layer, "Down pressed!");
}


static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}


static void window_load(Window *window) {
  
  window_set_background_color(window, GColorFromHEX(0xf2ddd9));
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_icon_layer = bitmap_layer_create(GRect(0, 10, bounds.size.w, 80));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));
  /*
  if (s_icon_bitmap) {
        gbitmap_destroy(s_icon_bitmap);
  }
  */
  s_icon_bitmap = gbitmap_create_with_resource(ABO_ICONS[0]);
  bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);

  s_temperature_layer = text_layer_create(GRect(0, 90, bounds.size.w, 32));
  text_layer_set_text_color(s_temperature_layer, GColorBlack);
  text_layer_set_background_color(s_temperature_layer, GColorClear);
  text_layer_set_font(s_temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_temperature_layer));

  s_city_layer = text_layer_create(GRect(0, 122, bounds.size.w, 32));
  text_layer_set_text_color(s_city_layer, GColorBlack);
  text_layer_set_background_color(s_city_layer, GColorClear);
  text_layer_set_font(s_city_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_city_layer));

  /*
  Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 1),
    TupletCString(WEATHER_TEMPERATURE_KEY, "1234\u00B0C"),
    TupletCString(WEATHER_CITY_KEY, "St Pebblesburg"),
  };
  */
  
  
  window_set_click_config_provider(window, click_config_provider);
  
  Tuplet initial_values[] = {
    TupletCString(PRODUCT_NAME_KEY, "Koiebrod"),
    TupletInteger(PRODUCT_ID_KEY, (uint8_t) 1),
    TupletInteger(PRODUCT_PRICE_KEY, (uint8_t) 29),
    TupletCString(PRODUCT_EXTID_KEY, "5578")
  };
  
  // Tuplet initial_values[] = {};

  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer),
      initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);

  //order_product();
}

static void window_unload(Window *window) {
  if (s_icon_bitmap) {
    gbitmap_destroy(s_icon_bitmap);
  }

  text_layer_destroy(s_city_layer);
  text_layer_destroy(s_temperature_layer);
  bitmap_layer_destroy(s_icon_layer);
}

static void init(void) {
  s_main_window = window_create();
  window_set_background_color(s_main_window, PBL_IF_COLOR_ELSE(GColorIndigo, GColorBlack));
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_main_window, true);

  app_message_open(64, 64);
}

static void deinit(void) {
  window_destroy(s_main_window);

  app_sync_deinit(&s_sync);
}

int main(void) {
  init(); // setup
  app_event_loop(); // will block until the app is ready to exit
  deinit(); // cleanup
}
