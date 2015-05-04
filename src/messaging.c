#include <pebble.h>
#include "location_info.h"
#include "messaging.h"
  
  
static void messaging_init() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Watch messaging is started!");
  locationInfoRecieved = false;
  app_message_register_inbox_received(inbox_received_callback);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *lat_tuple = dict_find(iterator, KEY_LOCATION_LAT);
  Tuple *lng_tuple = dict_find(iterator, KEY_LOCATION_LNG);
  Tuple *tzOffset_tuple = dict_find(iterator, KEY_GMT_OFFSET);
  
  if (lat_tuple != NULL && lng_tuple != NULL && tzOffset_tuple != NULL) {
    // set our location thing
    float lat = lat_tuple->value->int32;
    lat /= 1000000;
    float lng = lng_tuple->value->int32;
    lng /= 1000000;
    
    location.lat = lat;
    location.lng = lng;
    location.tzOffset = tzOffset_tuple->value->int32;
    locationInfoRecieved = true;
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Location recieved!");
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}