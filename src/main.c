#include <pebble.h>
#include "messaging.c"
#include "dayparts.c"

#define FORCE_BACKLIGHT
#define FORCE_12H true
#define TIME_STR_LEN 6
#define AMPM_STR_LEN 9
#define DATE_STR_LEN 25

// windows and layers
static Window *mainWindow;
static TextLayer *timeLayer;
static TextLayer *dateLayer;
static TextLayer *ampmLayer;
static BitmapLayer *bgLayer;

// fonts
static GFont timeFont;
static GFont dateFont;

// long-lived strings
static char timeText[TIME_STR_LEN];
static char ampmText[AMPM_STR_LEN];
static char dateText[DATE_STR_LEN]; 

static void update_clock() {
  time_t rawTime;
  struct tm * timeInfo;

  time(&rawTime);
  timeInfo = localtime(&rawTime);
  timeInfo->tm_hour = 12;
  timeInfo->tm_min = 00;

  // set time string
  if(clock_is_24h_style() && !FORCE_12H) {
    // use 24 hour format
    strftime(timeText, TIME_STR_LEN, "%H:%M", timeInfo);
  } else {
    // use 12 hour format
    strftime(timeText, TIME_STR_LEN, "%I:%M", timeInfo);
  }
    
  //now trim leading 0's
  if(timeText[0] == '0') {
    //shuffle everyone back by 1
    for(int i = 0; i < TIME_STR_LEN; i++) {
      timeText[i] = timeText[i + 1];
    }
        
    // move the ampm layer to adjust for the smaller text
    layer_set_frame(text_layer_get_layer(ampmLayer), GRect(96, 58, 70, 20));
  } else {
    // adjust the position of the am/pm layer
    layer_set_frame(text_layer_get_layer(ampmLayer), GRect(118, 58, 70, 20));
  }

  // display this time on the TextLayer
  text_layer_set_text(timeLayer, timeText);
  
  // display the am/pm state
  if(timeInfo->tm_hour == 0 && timeInfo->tm_min == 0) {
    strncpy(ampmText, "midnight", AMPM_STR_LEN);
  } else if(timeInfo->tm_hour == 12 && timeInfo->tm_min == 0) {
    strncpy(ampmText, "noon", AMPM_STR_LEN);
  } else if(timeInfo->tm_hour < 12) {
    strncpy(ampmText, "am", AMPM_STR_LEN);
  } else {
    strncpy(ampmText, "pm", AMPM_STR_LEN);
  }
  
  text_layer_set_text(ampmLayer, ampmText);
  
  // display the date
  strftime(dateText, DATE_STR_LEN, "%A, %b. %e", timeInfo);
  text_layer_set_text(dateLayer, dateText);
  
  // TEMP let's try putting in the BG switch here:
  GBitmap* background = getCurrentBG(timeInfo);
  bitmap_layer_set_bitmap(bgLayer, background);
}

static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  bgLayer = bitmap_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bgLayer));
  
  // create fonts
  timeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_38));
  dateFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_14));
  
  // Create time TextLayer
  timeLayer = text_layer_create(GRect(18, 34, 130, 50));
  text_layer_set_background_color(timeLayer, GColorClear);
  text_layer_set_text_color(timeLayer, GColorBlack);
  text_layer_set_font(timeLayer, timeFont);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));
  
  // Create date TextLayer
  dateLayer = text_layer_create(GRect(18, 22, 130, 20));
  text_layer_set_background_color(dateLayer, GColorClear);
  text_layer_set_text_color(dateLayer, GColorDarkGray);
  text_layer_set_font(dateLayer, dateFont);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(dateLayer));
  
  // Create ampm TextLayer
  ampmLayer = text_layer_create(GRect(96, 57, 60, 20));
  text_layer_set_background_color(ampmLayer, GColorClear);
  text_layer_set_text_color(ampmLayer, GColorBlack);
  text_layer_set_font(ampmLayer, dateFont);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(ampmLayer));
  
  // Make sure the time is displayed from the start
  update_clock();
}

static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(timeFont);
  
  //Destroy GBitmap
//   gbitmap_destroy(bgBitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(bgLayer);
  
  // Destroy TextLayer
  text_layer_destroy(timeLayer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_clock();
}

  
static void init() {
  #ifdef FORCE_BACKLIGHT
  light_enable(true);
  #endif
  
  // load background images
  initDayparts();
  
  // init the messaging thing
  messaging_init();
  
  // Create main Window element and assign to pointer
  mainWindow = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(mainWindow, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(mainWindow, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(mainWindow);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}