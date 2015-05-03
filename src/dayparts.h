#pragma once

#define NUMBER_OF_DAYPARTS 8

#define NOON_MINUTE 720
#define DAY_END_MINUTE 1440
#define TWILIGHT_DURATION 30
#define SUNRISE_DURATION 60

// default sunrise at 6:00am, default sunset at 6:00pm:
#define DEFAULT_SUNRISE_TIME 360
#define DEFAULT_SUNSET_TIME 1080

/*
 * Represents a part of the day. The bounds are determined
 * by startMinute and endMinute, which are in the form of
 * minutes in a day (so, for example, 12:00 noon is 60*12 = 720)
 */
typedef struct {
  int startMinute;
  int endMinute;
  uint32_t bgResourceID;
} Daypart;

static Daypart dayparts[NUMBER_OF_DAYPARTS];

static bool daypart_containsTime(const Daypart* this, int minute);
static void initDayparts(int sunriseMinute, int sunsetMinute);
static void setDaypartTimes(int sunriseMinute, int sunsetMinute);
static uint32_t getCurrentBG(const struct tm* time);
