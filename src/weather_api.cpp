#include "weather_api.h"
#include "config.h"
#include "logger.h"
#include <ArduinoJson.h>

String WeatherAPI::build_api_url() {
    String url = "https://api.open-meteo.com/v1/forecast?";
    url += "latitude=";
    url += String(WEATHER_LATITUDE, 4);
    url += "&longitude=";
    url += String(WEATHER_LONGITUDE, 4);
    url += "&current=temperature_2m,relative_humidity_2m,wind_speed_10m,wind_direction_10m,precipitation,apparent_temperature,is_day";
    url += "&forecast_days=1";
    url += "&timezone=auto";
    return url;
}

bool WeatherAPI::fetch_current_weather(WeatherData& weather) {
    weather.valid = false;
    
    HTTPClient http;
    String url = build_api_url();
    
    WEB_LOG("Fetching weather data from Open-Meteo...");
    http.begin(url);
    
    int http_code = http.GET();
    
    if (http_code != 200) {
        WEB_LOG("Weather API error: " + String(http_code));
        http.end();
        return false;
    }
    
    String payload = http.getString();
    http.end();
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
        WEB_LOG("Weather JSON parse error: " + String(error.c_str()));
        return false;
    }
    
    JsonObject current = doc["current"];
    if (!current) {
        WEB_LOG("No current weather data in response");
        return false;
    }
    
    weather.temperature = current["temperature_2m"] | 0.0f;
    weather.humidity = (int)(current["relative_humidity_2m"] | 0);
    weather.apparent_temperature = current["apparent_temperature"] | 0.0f;
    weather.precipitation = current["precipitation"] | 0.0f;
    weather.is_day = (int)(current["is_day"] | 1) == 1;
    
    float wind_ms = current["wind_speed_10m"] | 0.0f;
    weather.wind_speed = (int)(wind_ms * 3.6f);
    weather.wind_direction = (int)(current["wind_direction_10m"] | 0);
    
    if (weather.temperature < 0) {
        weather.description = "Kalt";
    } else if (weather.temperature < 10) {
        weather.description = "Kuhl";
    } else if (weather.temperature < 20) {
        weather.description = "Mild";
    } else if (weather.temperature < 30) {
        weather.description = "Warm";
    } else {
        weather.description = "Heiss";
    }
    
    weather.icon = "";
    weather.valid = true;
    
    WEB_LOG("Weather: " + String(weather.temperature, 1) + "C, " + String(weather.humidity) + "% humidity, wind " + String(weather.wind_speed) + " km/h from " + String(weather.wind_direction) + " deg");
    
    return true;
}
