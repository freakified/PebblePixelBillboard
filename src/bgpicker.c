#include <pebble.h>
#include "bgpicker.h"
#include "sun_calc.h"


void bgpicker_init() {
  // first, load and set all the background images
  // awkwardly force bw images in the case of a black and white watch
  #ifdef PBL_COLOR
  bgpicker_dayparts[0].backgroundImageId = RESOURCE_ID_IMAGE_BG0_NIGHT;
  bgpicker_dayparts[1].backgroundImageId = RESOURCE_ID_IMAGE_BG1_TWILIGHT;
  bgpicker_dayparts[2].backgroundImageId = RESOURCE_ID_IMAGE_BG2_SUNRISE;
  bgpicker_dayparts[3].backgroundImageId = RESOURCE_ID_IMAGE_BG3_MIDDAY;
  bgpicker_dayparts[4].backgroundImageId = RESOURCE_ID_IMAGE_BG4_AFTERNOON;
  bgpicker_dayparts[5].backgroundImageId = RESOURCE_ID_IMAGE_BG5_SUNSET;
  bgpicker_dayparts[6].backgroundImageId = RESOURCE_ID_IMAGE_BG6_TWILIGHT;
  bgpicker_dayparts[7].backgroundImageId = RESOURCE_ID_IMAGE_BG0_NIGHT;
  #else
  bgpicker_dayparts[0].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG0_NIGHT;
  bgpicker_dayparts[1].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG1_TRANSITION;
  bgpicker_dayparts[2].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG1_TRANSITION;
  bgpicker_dayparts[3].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG2_DAY;
  bgpicker_dayparts[4].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG2_DAY;
  bgpicker_dayparts[5].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG1_TRANSITION;
  bgpicker_dayparts[6].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG1_TRANSITION;
  bgpicker_dayparts[7].backgroundImageId = RESOURCE_ID_IMAGE_BW_BG0_NIGHT;
  #endif

  // if we have a location set in persistent storage, use that
  if (persist_exists(BGPICKER_LOC_KEY)) {
    LocationInfo loc;
    persist_read_data(BGPICKER_LOC_KEY, &loc, sizeof(LocationInfo));
    bgpicker_updateLocation(loc);
  } else {
    // otherwise, just use the default data
    bgpicker_setSunriseSunset(DEFAULT_SUNRISE_TIME, DEFAULT_SUNSET_TIME);
  }
}

void bgpicker_destruct() {
  gbitmap_destroy(bgpicker_currentBackgroundBitmap);
}

GBitmap* bgpicker_getCurrentBG(const struct tm* time) {
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

void bgpicker_updateLocation(LocationInfo loc) {
  bgpicker_location = loc;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Calculating with location: Lat: %d, Lng: %d, TZ: %d ",
          (int)(loc.lat*100),
          (int)(loc.lng*100),
          (int)(loc.tzOffset*100));

  // determine what day it is, since we'll need this
  time_t rawTime;
  struct tm * timeInfo;
  time(&rawTime);
  timeInfo = localtime(&rawTime);

  // get the sunrise/sunset data
  float sunRiseTime, sunSetTime;
  int sunRiseMin, sunSetMin;


  APP_LOG(APP_LOG_LEVEL_DEBUG, "Params 1: Year: %d, Month: %d, Day: %d, Z: %d",
          (int)timeInfo->tm_year,
          (int)timeInfo->tm_mon,
          (int)timeInfo->tm_mday,
          (int)ZENITH_OFFICIAL);

  sunRiseTime = calcSunRise(timeInfo->tm_year,
                            timeInfo->tm_mon + 1,
                            timeInfo->tm_mday,
                            loc.lat,
                            loc.lng,
                            ZENITH_OFFICIAL);
  sunSetTime  =  calcSunSet(timeInfo->tm_year,
                            timeInfo->tm_mon + 1,
                            timeInfo->tm_mday,
                            loc.lat,
                            loc.lng,
                            ZENITH_OFFICIAL);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunrise Time Initial R: %d, S: %d", (int)(sunRiseTime*100), (int)(sunSetTime*100));

  sunRiseTime = adjustTimezone(sunRiseTime, loc.tzOffset);
  sunSetTime = adjustTimezone(sunSetTime, loc.tzOffset);

  // for some reason, we have to add/subtract 12 hours (720 minutes)
  sunRiseMin = (int)(sunRiseTime * 60) - 720;
  sunSetMin = (int)(sunSetTime * 60) + 720;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunrise Time Initial R: %d, S: %d", (int)(sunRiseTime*100), (int)(sunSetTime*100));

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunrise recalculated! R: %d, S: %d", sunRiseMin, sunSetMin);
  bgpicker_setSunriseSunset(sunRiseMin, sunSetMin);

  // save the new location data to persistent storage
  persist_write_data(BGPICKER_LOC_KEY, &loc, sizeof(LocationInfo));
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
