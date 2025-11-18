#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>

struct TemperatureData {
    float temperature;
    float humidity;
    bool valid;
    String location_name;
};

struct WeatherData {
    float temperature;
    int humidity;
    String description;
    int wind_speed;
    int wind_direction;
    float precipitation;
    float apparent_temperature;
    bool is_day;
    String icon;
    bool valid;
};

class DisplayManager {
public:
    static void init();
    static void update(const TemperatureData& garage, const TemperatureData& heating, const WeatherData& weather);
    static void update_time_only();
    static void show_message(const String& message);
    static void clear();

private:
    static TFT_eSPI tft;
    static String last_time_string;
    static TemperatureData last_heating;
    static WeatherData last_weather;
    static bool first_draw;
    
    static void draw_time_header();
    static void draw_header();
    static void draw_temperature_panel(int x, int y, int width, int height, const TemperatureData& data);
    static void draw_weather_panel(int x, int y, int width, int height, const WeatherData& data);
    static void draw_temperature_box(int x, int y, int width, int height, const TemperatureData& data);
    static void draw_weather_box(int x, int y, int width, int height, const WeatherData& data);
    static void draw_heating_icon(int x, int y, uint16_t color);
    static void draw_weather_icon(int x, int y, float temperature);
    static String get_wind_direction(int degrees);
    static String get_location_display_name(const String& location);
    static bool temperature_data_changed(const TemperatureData& old_data, const TemperatureData& new_data);
    static bool weather_data_changed(const WeatherData& old_data, const WeatherData& new_data);
};

#endif
