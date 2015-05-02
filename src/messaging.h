#pragma once

#define KEY_LOCATION_LAT 0
#define KEY_LOCATION_LNG 1
#define KEY_GMT_OFFSET 2
  
static void messaging_init();

static void inbox_received_callback(DictionaryIterator *iterator, void *context);
static void inbox_dropped_callback(AppMessageResult reason, void *context);
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
static void outbox_sent_callback(DictionaryIterator *iterator, void *context);
