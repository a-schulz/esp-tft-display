#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include "display_manager.h"

extern bool screensaver_active;

class WebServerManager
{
public:
    static void init();
    static void track_activity();
    static bool has_recent_activity();
    static unsigned long get_last_activity_time();
    static void update_sensor_data(const TemperatureData& heating, const TemperatureData& garage, const WeatherData& weather);

private:
    static AsyncWebServer *server;
    static unsigned long last_web_request;
    static bool initialized;
    
    static TemperatureData current_heating;
    static TemperatureData current_garage;
    static WeatherData current_weather;
    
    static void setup_routes();
    static String build_dashboard_html();
    static String build_logs_html();
};

#endif
