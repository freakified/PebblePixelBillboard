#include <main.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_ampm_layer;

static GFont s_time_font;
static GFont s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_date() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  static char dateText[25];
  
  strftime(dateText, 25, "%A, %b. %e", tick_time);
  
  text_layer_set_text(s_date_layer, dateText);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  update_date();

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true && !FORCE_12H) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  //now trim leading 0's
  if(buffer[0] == '0') {
    int bufferMaxIndex = (int)sizeof(buffer) - 1;
    
    //shuffle everyone back by 1
    for(int i = 0; i < bufferMaxIndex; i++) {
      buffer[i] = buffer[i + 1];
    }
    
    buffer[bufferMaxIndex] = ' ';
    
    // move the ampm layer to adjust for the smaller text
    layer_set_frame(text_layer_get_layer(s_ampm_layer), GRect(96, 57, 30, 20));
  } else {
    // adjust the position of the am/pm layer
    layer_set_frame(text_layer_get_layer(s_ampm_layer), GRect(118, 57, 30, 20));
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG4_AFTERNOON);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(18, 34, 130, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(18, 22, 130, 20));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorDarkGray);
  text_layer_set_text(s_date_layer, "Monday, Sept. 31");
  
  // Create ampm TextLayer
  s_ampm_layer = text_layer_create(GRect(96, 57, 30, 20));
  text_layer_set_background_color(s_ampm_layer, GColorClear);
//   text_layer_set_background_color(s_ampm_layer, GColorRed);
  text_layer_set_text_color(s_ampm_layer, GColorBlack);
  text_layer_set_text(s_ampm_layer, "pm");
  
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_38));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_14));

  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  text_layer_set_font(s_ampm_layer, s_date_font);
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_ampm_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
