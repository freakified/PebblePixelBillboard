#include <pebble.h>
#include "dayparts.h"


/*
 * Check if a given time is contained within a specified daypart
 */
static bool daypart_containsTime(const Daypart* this, int minute) {
  return (this->startMinute < minute && this->endMinute > minute);
}

/*
 * Sets up the set of 7 dayparts with their respective background images,
 * and times.
 */
static void initDayparts(int sunriseMinute, int sunsetMinute) {
  // first, set all the background images
  dayparts[0].bgResourceID = RESOURCE_ID_IMAGE_BG0_NIGHT;
  dayparts[1].bgResourceID = RESOURCE_ID_IMAGE_BG1_TWILIGHT;
  dayparts[2].bgResourceID = RESOURCE_ID_IMAGE_BG2_SUNRISE;
  dayparts[3].bgResourceID = RESOURCE_ID_IMAGE_BG3_MIDDAY;
  dayparts[4].bgResourceID = RESOURCE_ID_IMAGE_BG4_AFTERNOON;
  dayparts[5].bgResourceID = RESOURCE_ID_IMAGE_BG5_SUNSET;
  dayparts[6].bgResourceID = RESOURCE_ID_IMAGE_BG6_TWILIGHT;
  dayparts[7].bgResourceID = RESOURCE_ID_IMAGE_BG0_NIGHT;

  // next, use the default values to set up the initial dayparts
  setDaypartTimes(DEFAULT_SUNRISE_TIME, DEFAULT_SUNSET_TIME);
}

static void setDaypartTimes(int sunriseMinute, int sunsetMinute) {
  // night 1
  dayparts[0].startMinute = 0;
  dayparts[0].endMinute   = sunriseMinute - TWILIGHT_DURATION;

  // morning twilight
  dayparts[1].startMinute = sunriseMinute - TWILIGHT_DURATION;
  dayparts[1].endMinute   = sunriseMinute;

  // sunrise
  dayparts[2].startMinute = sunriseMinute;
  dayparts[2].endMinute   = sunriseMinute + SUNRISE_DURATION;

  // midday
  dayparts[3].startMinute = sunriseMinute + SUNRISE_DURATION;
  dayparts[3].endMinute   = NOON_MINUTE;

  // afternoon
  dayparts[4].startMinute = NOON_MINUTE;
  dayparts[4].endMinute   = sunsetMinute - SUNRISE_DURATION;

  // sunset
  dayparts[5].startMinute = sunsetMinute - SUNRISE_DURATION;
  dayparts[5].endMinute   = sunsetMinute;

  // evening twilight
  dayparts[6].startMinute = sunsetMinute;
  dayparts[6].endMinute   = sunsetMinute + TWILIGHT_DURATION;

  // night 2
  dayparts[7].startMinute = sunsetMinute + TWILIGHT_DURATION;
  dayparts[7].endMinute   = DAY_END_MINUTE;
}

/*
 * Returns the current background image for a specified time
 */
static uint32_t getCurrentBG(const struct tm* time) {
  int currentTimeInMinutes = time->tm_hour * 60 + time->tm_min;

  // check each daypart if it matches our current time, and return
  // if we find a match
  for(int i = 0; i < NUMBER_OF_DAYPARTS; i++) {
    if(daypart_containsTime(&dayparts[i], currentTimeInMinutes)) {
      return dayparts[i].bgResourceID;
    }
  }

  // if we failed to find a match, just return midday
  APP_LOG(APP_LOG_LEVEL_WARNING, "Failed to find daypart for minute %d", currentTimeInMinutes);
  return RESOURCE_ID_IMAGE_BG3_MIDDAY;
}
