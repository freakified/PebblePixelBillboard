#pragma once

typedef struct {
  float lat;
  float lng;
  int tzOffset;
} LocationInfo;

static LocationInfo location;
static bool locationInfoRecieved = false;
