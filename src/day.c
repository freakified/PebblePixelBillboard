#include <pebble.h>
#include "location_info.h"
#include "sun_calc.h"
#include "daypart.c"
#include "day.h"

static void day_init() {
  // first, load and set all the background images
  day_dayparts[0].backgroundImageId = RESOURCE_ID_IMAGE_BG0_NIGHT;
  day_dayparts[1].backgroundImageId = RESOURCE_ID_IMAGE_BG1_TWILIGHT;
  day_dayparts[2].backgroundImageId = RESOURCE_ID_IMAGE_BG2_SUNRISE;
  day_dayparts[3].backgroundImageId = RESOURCE_ID_IMAGE_BG3_MIDDAY;
  day_dayparts[4].backgroundImageId = RESOURCE_ID_IMAGE_BG4_AFTERNOON;
  day_dayparts[5].backgroundImageId = RESOURCE_ID_IMAGE_BG5_SUNSET;
  day_dayparts[6].backgroundImageId = RESOURCE_ID_IMAGE_BG6_TWILIGHT;
  day_dayparts[7].backgroundImageId = RESOURCE_ID_IMAGE_BG0_NIGHT;

  // next, ensure that the dayparts are set up (using default values)
  // don't worry, they'll be re-set as soon as we get the canonical sun data
  day_setSunriseSunset(DEFAULT_SUNRISE_TIME, DEFAULT_SUNSET_TIME);
}

static void day_destruct() {
  gbitmap_destroy(day_currentBackgroundBitmap);
}

static GBitmap* day_getCurrentBG(const struct tm* time) {
  int currentTimeInMinutes = time->tm_hour * 60 + time->tm_min;

  // check each daypart if it matches our current time, and return
  // if we find a match
  for(int i = 0; i < NUMBER_OF_DAYPARTS; i++) {
    if(daypart_containsTime(&day_dayparts[i], currentTimeInMinutes)) {
      if(day_dayparts[i].backgroundImageId != day_currentBackgroundID) {
        // if it's a different background image, deallocate the current BG and replace it 
        gbitmap_destroy(day_currentBackgroundBitmap); 
        day_currentBackgroundID = day_dayparts[i].backgroundImageId;
        day_currentBackgroundBitmap = gbitmap_create_with_resource(day_currentBackgroundID);
      }
      
      return day_currentBackgroundBitmap;
    }
  }

  // if we failed to find a match, don't update anything
  APP_LOG(APP_LOG_LEVEL_WARNING, "Failed to find daypart for minute %d", currentTimeInMinutes);
  return day_currentBackgroundBitmap;
}

static void day_setSunriseSunset(int sunriseMinute, int sunsetMinute) {
  // night 1
  day_dayparts[0].startMinute = 0;
  day_dayparts[0].endMinute   = sunriseMinute - TWILIGHT_DURATION;

  // morning twilight
  day_dayparts[1].startMinute = sunriseMinute - TWILIGHT_DURATION;
  day_dayparts[1].endMinute   = sunriseMinute;

  // sunrise
  day_dayparts[2].startMinute = sunriseMinute;
  day_dayparts[2].endMinute   = sunriseMinute + SUNRISE_DURATION;

  // midday
  day_dayparts[3].startMinute = sunriseMinute + SUNRISE_DURATION;
  day_dayparts[3].endMinute   = NOON_MINUTE;

  // afternoon
  day_dayparts[4].startMinute = NOON_MINUTE;
  day_dayparts[4].endMinute   = sunsetMinute - SUNRISE_DURATION;

  // sunset
  day_dayparts[5].startMinute = sunsetMinute - SUNRISE_DURATION;
  day_dayparts[5].endMinute   = sunsetMinute;

  // evening twilight
  day_dayparts[6].startMinute = sunsetMinute;
  day_dayparts[6].endMinute   = sunsetMinute + TWILIGHT_DURATION;

  // night 2
  day_dayparts[7].startMinute = sunsetMinute + TWILIGHT_DURATION;
  day_dayparts[7].endMinute   = DAY_END_MINUTE;
}

