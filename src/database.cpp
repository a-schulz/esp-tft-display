#include "database.h"
#include "config.h"
#include "logger.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

Supabase Database::db;

void Database::init()
{
    db.begin(SUPABASE_URL, SUPABASE_KEY);
}

bool Database::fetch_temperature(const String& location, TemperatureData& data) {
    data.valid = false;
    data.location_name = location;
    
    WEB_LOG("Fetching temperature for location: " + location);
    
    HTTPClient http;
    String url = String(SUPABASE_URL) + "/rest/v1/rpc/latest_measurements?location=eq." + location;
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", SUPABASE_KEY);
    http.addHeader("Authorization", String("Bearer ") + SUPABASE_KEY);
    
    int http_code = http.POST("");
    
    if (http_code != 200) {
        WEB_LOG("RPC error: " + String(http_code));
        http.end();
        return false;
    }
    
    String response = http.getString();
    http.end();
    
    if (response.isEmpty()) {
        WEB_LOG("Empty response from database for " + location);
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (error) {
        WEB_LOG("JSON parse error: " + String(error.c_str()));
        return false;
    }
    
    if (!doc.is<JsonArray>() || doc.size() == 0) {
        WEB_LOG("No data found for location: " + location);
        return false;
    }
    
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool has_temp = false;
    bool has_hum = false;
    
    for (JsonObject measurement : doc.as<JsonArray>()) {
        String type = measurement["type"] | "";
        float value = measurement["value"] | 0.0f;
        
        if (type == "temperature") {
            temperature = value;
            has_temp = true;
        } else if (type == "humidity") {
            humidity = value;
            has_hum = true;
        }
    }
    
    if (!has_temp) {
        WEB_LOG("No temperature measurement for location: " + location);
        return false;
    }
    
    data.temperature = temperature;
    data.humidity = humidity;
    data.valid = true;
    
    WEB_LOG("Temperature for " + location + ": " + String(data.temperature, 1) + "°C" + 
            (has_hum ? ", Humidity: " + String((int)data.humidity) + "%" : ""));
    
    return true;
}