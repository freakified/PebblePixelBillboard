#include <pebble.h>
#include "location_info.h"
#include "sun_calc.h"
#include "daypart.c"
#include "bgpicker.h"

static void bgpicker_init() {
  // first, load and set all the background images
  bgpicker_dayparts[0].backgroundImageId = RESOURCE_ID_IMAGE_BG0_NIGHT;
  bgpicker_dayparts[1].backgroundImageId = RESOURCE_ID_IMAGE_BG1_TWILIGHT;
  bgpicker_dayparts[2].backgroundImageId = RESOURCE_ID_IMAGE_BG2_SUNRISE;
  bgpicker_dayparts[3].backgroundImageId = RESOURCE_ID_IMAGE_BG3_MIDDAY;
  bgpicker_dayparts[4].backgroundImageId = RESOURCE_ID_IMAGE_BG4_AFTERNOON;
  bgpicker_dayparts[5].backgroundImageId = RESOURCE_ID_IMAGE_BG5_SUNSET;
  bgpicker_dayparts[6].backgroundImageId = RESOURCE_ID_IMAGE_BG6_TWILIGHT;
  bgpicker_dayparts[7].backgroundImageId = RESOURCE_ID_IMAGE_BG0_NIGHT;

  // next, ensure that the dayparts are set up (using default values)
  // don't worry, they'll be re-set as soon as we get the canonical sun data
  bgpicker_setSunriseSunset(DEFAULT_SUNRISE_TIME, DEFAULT_SUNSET_TIME);
}

static void bgpicker_destruct() {
  gbitmap_destroy(bgpicker_currentBackgroundBitmap);
}

static GBitmap* bgpicker_getCurrentBG(const struct tm* time) {
  int currentTimeInMinutes = time->tm_hour * 60 + time->tm_min;

  // check each daypart if it matches our current time, and return
  // if we find a match
  for(int i = 0; i < NUMBER_OF_DAYPARTS; i++) {
    if(daypart_containsTime(&bgpicker_dayparts[i], currentTimeInMinutes)) {
      if(bgpicker_dayparts[i].backgroundImageId != bgpicker_currentBackgroundID) {
        // if it's a different background image, deallocate the current BG and replace it
        gbitmap_destroy(bgpicker_currentBackgroundBitmap);
        bgpicker_currentBackgroundID = bgpicker_dayparts[i].backgroundImageId;
        bgpicker_currentBackgroundBitmap = gbitmap_create_with_resource(bgpicker_currentBackgroundID);
      }

      return bgpicker_currentBackgroundBitmap;
    }
  }

  // if we failed to find a match, don't update anything
  APP_LOG(APP_LOG_LEVEL_WARNING, "Failed to find daypart for minute %d", currentTimeInMinutes);
  return bgpicker_currentBackgroundBitmap;
}

static void bgpicker_setSunriseSunset(int sunriseMinute, int sunsetMinute) {
  // night 1
  bgpicker_dayparts[0].startMinute = 0;
  bgpicker_dayparts[0].endMinute   = sunriseMinute - TWILIGHT_DURATION;

  // morning twilight
  bgpicker_dayparts[1].startMinute = sunriseMinute - TWILIGHT_DURATION;
  bgpicker_dayparts[1].endMinute   = sunriseMinute;

  // sunrise
  bgpicker_dayparts[2].startMinute = sunriseMinute;
  bgpicker_dayparts[2].endMinute   = sunriseMinute + SUNRISE_DURATION;

  // midday
  bgpicker_dayparts[3].startMinute = sunriseMinute + SUNRISE_DURATION;
  bgpicker_dayparts[3].endMinute   = NOON_MINUTE;

  // afternoon
  bgpicker_dayparts[4].startMinute = NOON_MINUTE;
  bgpicker_dayparts[4].endMinute   = sunsetMinute - SUNRISE_DURATION;

  // sunset
  bgpicker_dayparts[5].startMinute = sunsetMinute - SUNRISE_DURATION;
  bgpicker_dayparts[5].endMinute   = sunsetMinute;

  // evening twilight
  bgpicker_dayparts[6].startMinute = sunsetMinute;
  bgpicker_dayparts[6].endMinute   = sunsetMinute + TWILIGHT_DURATION;

  // night 2
  bgpicker_dayparts[7].startMinute = sunsetMinute + TWILIGHT_DURATION;
  bgpicker_dayparts[7].endMinute   = DAY_END_MINUTE;
}
