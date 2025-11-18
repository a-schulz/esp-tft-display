#include "display_manager.h"
#include "config.h"
#include "logger.h"
#include <map>

TFT_eSPI DisplayManager::tft = TFT_eSPI();
String DisplayManager::last_time_string = "";
TemperatureData DisplayManager::last_heating = {0.0f, 0.0f, false, ""};
WeatherData DisplayManager::last_weather = {0.0f, 0, "", 0, 0, 0.0f, 0.0f, true, "", false};
bool DisplayManager::first_draw = true;
uint8_t DisplayManager::current_brightness = TFT_BACKLIGHT_BRIGHTNESS;
unsigned long DisplayManager::last_pixel_shift = 0;
int DisplayManager::pixel_shift_x = 0;
int DisplayManager::pixel_shift_y = 0;
unsigned long DisplayManager::display_on_time = 0;
unsigned long DisplayManager::last_temp_check = 0;
bool DisplayManager::screensaver_active = false;

void DisplayManager::init() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
    set_brightness(TFT_BACKLIGHT_BRIGHTNESS);
    
    display_on_time = millis();
    first_draw = true;
}

void DisplayManager::clear() {
    tft.fillScreen(TFT_BLACK);
    first_draw = true;
}

void DisplayManager::show_message(const String& message) {
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSansBold18pt7b);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(message, TFT_WIDTH / 2, TFT_HEIGHT / 2);
}

void DisplayManager::update(const TemperatureData& garage, const TemperatureData& heating, const WeatherData& weather) {
    bool heating_changed = first_draw || temperature_data_changed(last_heating, heating);
    bool weather_changed = first_draw || weather_data_changed(last_weather, weather);
    
    if (first_draw) {
        clear();
    }
    
    draw_time_header();
    
    if (heating_changed) {
        draw_temperature_panel(10, 50, 220, 260, heating);
        last_heating = heating;
    }
    
    if (weather_changed) {
        draw_weather_panel(240, 50, 230, 260, weather);
        last_weather = weather;
    }
    
    first_draw = false;
}

void DisplayManager::update_time_only() {
    draw_time_header();
}

void DisplayManager::draw_time_header() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char time_str[6];
        strftime(time_str, sizeof(time_str), "%H:%M", &timeinfo);
        
        String current_time = String(time_str);
        if (current_time != last_time_string) {
            tft.fillRect(TFT_WIDTH / 2 - 60, 5, 120, 40, TFT_BLACK);
            
            tft.setTextFont(6);
            tft.setTextSize(1);
            tft.setTextColor(TFT_CYAN, TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.drawString(time_str, TFT_WIDTH / 2, 25);
            
            last_time_string = current_time;
        }
    }
}

void DisplayManager::draw_header() {
    tft.setFreeFont(&FreeSansBold12pt7b);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Temperatur & Wetter", 10, 10);
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char time_str[6];
        strftime(time_str, sizeof(time_str), "%H:%M", &timeinfo);
        tft.setTextDatum(TR_DATUM);
        tft.drawString(time_str, TFT_WIDTH - 5, 10);
    }
    
    tft.drawLine(0, 40, TFT_WIDTH, 40, TFT_DARKGREY);
}

void DisplayManager::draw_temperature_panel(int x, int y, int width, int height, const TemperatureData& data) {
    const uint16_t bg_color = 0x2124;
    const uint16_t border_color = 0x4208;
    
    x += pixel_shift_x;
    y += pixel_shift_y;
    
    tft.fillRoundRect(x, y, width, height, 12, bg_color);
    tft.drawRoundRect(x, y, width, height, 12, border_color);
    
    if (data.valid) {
        const int icon_center_x = x + width / 2;
        
        draw_heating_icon(icon_center_x, y + 35, TFT_ORANGE);
        
        tft.setTextSize(1);
        tft.setTextFont(8);
        tft.setTextColor(TFT_WHITE, bg_color);
        tft.setTextDatum(MC_DATUM);
        
        char temp_str[8];
        snprintf(temp_str, sizeof(temp_str), "%.1f", data.temperature);
        tft.drawString(temp_str, icon_center_x, y + 150);
        
        tft.setFreeFont(&FreeSansBold18pt7b);
        tft.setTextColor(TFT_YELLOW, bg_color);
        String location = get_location_display_name(data.location_name);
        tft.drawString(location, icon_center_x, y + height - 25);
        
    } else {
        tft.setFreeFont(&FreeSansBold18pt7b);
        tft.setTextColor(TFT_RED, bg_color);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Keine Daten", x + width / 2, y + height / 2);
    }
}

void DisplayManager::draw_weather_panel(int x, int y, int width, int height, const WeatherData& data) {
    const uint16_t bg_color = 0x1082;
    const uint16_t border_color = 0x2945;
    
    x += pixel_shift_x;
    y += pixel_shift_y;
    
    tft.fillRoundRect(x, y, width, height, 12, bg_color);
    tft.drawRoundRect(x, y, width, height, 12, border_color);
    
    const int center_x = x + width / 2;
    const int left_x = x + 20;
    
    if (data.valid) {
        draw_weather_icon(left_x + 25, y + 45, data.temperature);
        
        tft.setTextFont(7);
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, bg_color);
        tft.setTextDatum(MC_DATUM);
        
        char temp_str[8];
        snprintf(temp_str, sizeof(temp_str), "%.1f", data.temperature);
        tft.drawString(temp_str, left_x + 25, y + 125);
        
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(0xFFE0, bg_color);
        tft.setTextDatum(TL_DATUM);
        
        int right_x = x + width / 2 + 10;
        int line_y = y + 25;
        int line_spacing = 30;
        
        tft.drawString(get_wind_direction(data.wind_direction), right_x, line_y);
        line_y += line_spacing;
        
        char wind_str[16];
        snprintf(wind_str, sizeof(wind_str), "%d km/h", data.wind_speed);
        tft.drawString(wind_str, right_x, line_y);
        line_y += line_spacing;
        
        char hum_str[16];
        snprintf(hum_str, sizeof(hum_str), "%d%%", data.humidity);
        tft.drawString(hum_str, right_x, line_y);
        line_y += line_spacing;
        
        if (data.precipitation > 0.0f) {
            char precip_str[16];
            snprintf(precip_str, sizeof(precip_str), "%.1f mm", data.precipitation);
            tft.drawString(precip_str, right_x, line_y);
            line_y += line_spacing;
        }
        
        char feels_str[16];
        snprintf(feels_str, sizeof(feels_str), "%.1f", data.apparent_temperature);
        tft.drawString(String("Gefuhlt ") + feels_str, right_x, line_y);
        
        tft.setFreeFont(&FreeSansBold18pt7b);
        tft.setTextColor(TFT_CYAN, bg_color);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Wetter", center_x, y + height - 25);
        
    } else {
        tft.setFreeFont(&FreeSansBold18pt7b);
        tft.setTextColor(TFT_RED, bg_color);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Nicht verfuegbar", center_x, y + height / 2);
    }
}

void DisplayManager::draw_temperature_box(int x, int y, int width, int height, const TemperatureData& data) {
    // Deprecated
}

void DisplayManager::draw_weather_box(int x, int y, int width, int height, const WeatherData& data) {
    // Deprecated
}

void DisplayManager::draw_heating_icon(int x, int y, uint16_t color) {
    const int icon_size = 50;
    const int bar_width = 10;
    const int bar_spacing = 16;
    const uint16_t bg_color = 0x2124;
    
    for (int i = 0; i < 3; i++) {
        int bar_x = x - icon_size / 2 + i * bar_spacing + 8;
        int bar_height = 35 + (i == 1 ? 12 : 0);
        
        for (int wave = 0; wave < 3; wave++) {
            int wave_y = y + wave * 12;
            tft.drawArc(bar_x + bar_width / 2, wave_y, bar_width / 2 + 3, bar_width / 2, 
                        180, 360, color, bg_color);
        }
    }
}

void DisplayManager::draw_weather_icon(int x, int y, float temperature) {
    const int icon_size = 45;
    uint16_t icon_color = TFT_YELLOW;
    
    if (temperature < 10) {
        icon_color = TFT_CYAN;
        tft.fillCircle(x, y, icon_size / 2, icon_color);
        
        for (int i = 0; i < 6; i++) {
            float angle = i * 60.0 * DEG_TO_RAD;
            int x1 = x + cos(angle) * (icon_size / 2 + 6);
            int y1 = y + sin(angle) * (icon_size / 2 + 6);
            int x2 = x + cos(angle) * (icon_size / 2 + 14);
            int y2 = y + sin(angle) * (icon_size / 2 + 14);
            tft.drawLine(x1, y1, x2, y2, icon_color);
        }
    } else if (temperature < 20) {
        tft.fillCircle(x - 18, y - 12, 22, 0x7BEF);
        tft.fillCircle(x + 6, y - 18, 24, 0x7BEF);
        tft.fillCircle(x + 18, y - 6, 20, 0x7BEF);
    } else {
        tft.fillCircle(x, y, icon_size / 2, icon_color);
        
        for (int i = 0; i < 8; i++) {
            float angle = i * 45.0 * DEG_TO_RAD;
            int x1 = x + cos(angle) * (icon_size / 2 + 4);
            int y1 = y + sin(angle) * (icon_size / 2 + 4);
            int x2 = x + cos(angle) * (icon_size / 2 + 12);
            int y2 = y + sin(angle) * (icon_size / 2 + 12);
            tft.drawLine(x1, y1, x2, y2, icon_color);
        }
    }
}

String DisplayManager::get_wind_direction(int degrees) {
    if (degrees >= 337 || degrees < 23) return "N";
    if (degrees >= 23 && degrees < 68) return "NO";
    if (degrees >= 68 && degrees < 113) return "O";
    if (degrees >= 113 && degrees < 158) return "SO";
    if (degrees >= 158 && degrees < 203) return "S";
    if (degrees >= 203 && degrees < 248) return "SW";
    if (degrees >= 248 && degrees < 293) return "W";
    if (degrees >= 293 && degrees < 337) return "NW";
    return "?";
}

bool DisplayManager::temperature_data_changed(const TemperatureData& old_data, const TemperatureData& new_data) {
    if (old_data.valid != new_data.valid) return true;
    if (!new_data.valid) return false;
    
    if (abs(old_data.temperature - new_data.temperature) > 0.05f) return true;
    if (abs(old_data.humidity - new_data.humidity) > 0.5f) return true;
    
    return false;
}

bool DisplayManager::weather_data_changed(const WeatherData& old_data, const WeatherData& new_data) {
    if (old_data.valid != new_data.valid) return true;
    if (!new_data.valid) return false;
    
    if (abs(old_data.temperature - new_data.temperature) > 0.05f) return true;
    if (old_data.humidity != new_data.humidity) return true;
    if (old_data.wind_speed != new_data.wind_speed) return true;
    if (old_data.wind_direction != new_data.wind_direction) return true;
    if (abs(old_data.precipitation - new_data.precipitation) > 0.05f) return true;
    if (abs(old_data.apparent_temperature - new_data.apparent_temperature) > 0.05f) return true;
    
    return false;
}

String DisplayManager::get_location_display_name(const String& location) {
    static const std::map<String, String> location_names = {
        {"garage", "Garage"},
        {"heating", "Heizung"},
        {"outdoor", "Aussen"},
        {"living", "Wohnzimmer"},
        {"bedroom", "Schlafzimmer"}
    };
    
    auto it = location_names.find(location);
    if (it != location_names.end()) {
        return it->second;
    }
    return location;
}

void DisplayManager::handle_protection() {
    unsigned long current_time = millis();
    
    #if TFT_AUTO_BRIGHTNESS_ENABLED
    adjust_brightness_for_time();
    #endif
    
    check_temperature();
    
    #if TFT_PIXEL_SHIFT_ENABLED
    if (screensaver_active && current_time - last_pixel_shift >= TFT_PIXEL_SHIFT_INTERVAL) {
        apply_pixel_shift();
        last_pixel_shift = current_time;
    }
    #endif
}

void DisplayManager::adjust_brightness_for_time() {
    uint8_t target_brightness;
    
    if (is_night_time()) {
        target_brightness = screensaver_active ? TFT_SCREENSAVER_BRIGHTNESS : TFT_NIGHT_BRIGHTNESS;
    } else {
        target_brightness = screensaver_active ? TFT_SCREENSAVER_BRIGHTNESS : TFT_BACKLIGHT_BRIGHTNESS;
    }
    
    if (current_brightness != target_brightness) {
        set_brightness(target_brightness);
    }
}

void DisplayManager::check_temperature() {
    unsigned long current_time = millis();
    
    if (current_time - last_temp_check < TEMP_CHECK_INTERVAL) {
        return;
    }
    
    last_temp_check = current_time;
    float temp = get_esp32_temperature();
    
    if (temp > TEMP_CRITICAL_THRESHOLD) {
        WEB_LOG("CRITICAL: ESP32 temperature " + String(temp, 1) + "C - Display OFF");
        set_brightness(0);
        tft.fillScreen(TFT_BLACK);
    } else if (temp > TEMP_WARNING_THRESHOLD) {
        WEB_LOG("WARNING: ESP32 temperature " + String(temp, 1) + "C - Reducing brightness");
        uint8_t safe_brightness = current_brightness * 0.5;
        set_brightness(safe_brightness);
    }
}

void DisplayManager::set_brightness(uint8_t brightness) {
    current_brightness = brightness;
    analogWrite(TFT_BACKLIGHT_PIN, brightness);
}

uint8_t DisplayManager::get_current_brightness() {
    return current_brightness;
}

unsigned long DisplayManager::get_display_uptime_hours() {
    return (millis() - display_on_time) / 3600000;
}

void DisplayManager::apply_pixel_shift() {
    pixel_shift_x = (pixel_shift_x + 3) % 10 - 5;
    pixel_shift_y = (pixel_shift_y + 2) % 10 - 5;
    
    first_draw = true;
}

bool DisplayManager::is_night_time() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return false;
    }
    
    int hour = timeinfo.tm_hour;
    return (hour >= 22 || hour < 6);
}

float DisplayManager::get_esp32_temperature() {
    return temperatureRead();
}
