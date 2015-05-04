#pragma once

#define NUMBER_OF_DAYPARTS 8

#define NOON_MINUTE 720
#define DAY_END_MINUTE 1440
#define TWILIGHT_DURATION 30
#define SUNRISE_DURATION 60

// default sunrise at 6:00am, default sunset at 6:00pm:
#define DEFAULT_SUNRISE_TIME 360
#define DEFAULT_SUNSET_TIME 1080

static Daypart dayparts[NUMBER_OF_DAYPARTS];

static void dayparts_construct();
static void dayparts_destruct();

static void dayparts_setDaypartTimes(int sunriseMinute, int sunsetMinute);
static GBitmap* getCurrentBG(const struct tm* time);
