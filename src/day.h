#pragma once
  
/*
 * Represents a day. Based on sunrise and sunset times, 
 * splits the day into 8 different "day parts", each of which contains
 * a background image.
 */

#define NUMBER_OF_DAYPARTS 8

// default day times
#define NOON_MINUTE 720
#define DAY_END_MINUTE 1440
#define TWILIGHT_DURATION 30
#define SUNRISE_DURATION 60

// default sunrise at 6:00am, default sunset at 6:00pm:
#define DEFAULT_SUNRISE_TIME 360
#define DEFAULT_SUNSET_TIME 1080

Daypart day_dayparts[NUMBER_OF_DAYPARTS];
uint32_t day_currentBackgroundID;
GBitmap* day_currentBackgroundBitmap;
  
/*
 * Sets up the set of 7 dayparts with their respective background images,
 * and times.
 */
static void day_init();

/*
 * Deallocates the current backround image
 */
static void day_destruct();

/*
 * Returns a GBitmap pointer to the current background image,
 * based on the specified time.
 */
static GBitmap* day_getCurrentBG(const struct tm* time);

static void day_setLocation(const LocationInfo* location);

/*
 * Sets the sunrise and sunset times, causing all dayparts' start/end
 * times to be updated
 */
static void day_setSunriseSunset(int sunriseMinute, int sunsetMinute);
