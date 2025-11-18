#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <HTTPClient.h>
#include "display_manager.h"

class WeatherAPI {
public:
    static bool fetch_current_weather(WeatherData& weather);

private:
    static String build_api_url();
};

#endif
