#include <Arduino.h>
#include "config.h"
#include "logger.h"
#include "network_manager.h"
#include "web_server.h"
#include "database.h"
#include "display_manager.h"
#include "weather_api.h"

RTC_DATA_ATTR int boot_count = 0;

volatile bool button_pressed = false;
bool screensaver_active = false;
unsigned long last_button_press = 0;
unsigned long boot_time = 0;
unsigned long last_temperature_update = 0;
unsigned long last_weather_update = 0;
unsigned long last_time_update = 0;
unsigned long last_activity_time = 0;

const unsigned long OTA_WINDOW_DURATION = 300000;
const unsigned long TEMPERATURE_UPDATE_INTERVAL = 60000;
const unsigned long WEATHER_UPDATE_INTERVAL = 600000;
const unsigned long TIME_UPDATE_INTERVAL = 10000;

TemperatureData garage_data;
TemperatureData heating_data;
WeatherData weather_data;

void IRAM_ATTR button_isr();
void setup_button();
void fetch_temperature_data();
void fetch_weather_data();
void update_data_if_needed();
void check_screensaver_timeout();

void setup()
{
  Serial.begin(115200);
  delay(100);

  boot_time = millis();
  boot_count++;

  Logger::init(MAX_LOG_ENTRIES);
  WEB_LOG("=== ESP32 TFT Display Starting ===");
  WEB_LOG("Boot count: " + String(boot_count));

  DisplayManager::init();
  DisplayManager::show_message("Verbinde WiFi...");

  setup_button();
  Database::init();
  
  if (NetworkManager::connect_wifi())
  {
    DisplayManager::show_message("WiFi verbunden");
    
    if (boot_count == 1)
    {
      NetworkManager::setup_ota();
      WebServerManager::init();
    }

    NetworkManager::sync_time();
    
    DisplayManager::show_message("Lade Daten...");
    
    fetch_temperature_data();
    fetch_weather_data();
    
    DisplayManager::update(garage_data, heating_data, weather_data);
    
    last_temperature_update = millis();
    last_weather_update = millis();
    last_time_update = millis();
  }
  else
  {
    DisplayManager::show_message("WiFi Fehler!");
    WEB_LOG("WiFi connection failed");
  }
}

void loop()
{
  if (boot_count == 1)
  {
    NetworkManager::handle_ota();
  }

  update_data_if_needed();
  
  DisplayManager::handle_protection();
  
  check_screensaver_timeout();

  delay(100);
}

void setup_button()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_isr, FALLING);
}

void IRAM_ATTR button_isr()
{
  button_pressed = true;
  last_button_press = millis();
}

void fetch_temperature_data()
{
  WEB_LOG("Fetching temperature data from Supabase...");
  Database::fetch_temperature("garage", garage_data);
  Database::fetch_temperature("heating", heating_data);
}

void fetch_weather_data()
{
  WEB_LOG("Fetching weather data from Open-Meteo...");
  WeatherAPI::fetch_current_weather(weather_data);
}

void update_data_if_needed()
{
  bool needs_full_update = false;
  
  if (millis() - last_temperature_update >= TEMPERATURE_UPDATE_INTERVAL)
  {
    fetch_temperature_data();
    last_temperature_update = millis();
    needs_full_update = true;
  }
  
  if (millis() - last_weather_update >= WEATHER_UPDATE_INTERVAL)
  {
    fetch_weather_data();
    last_weather_update = millis();
    needs_full_update = true;
  }
  
  if (needs_full_update)
  {
    DisplayManager::update(garage_data, heating_data, weather_data);
    WebServerManager::update_sensor_data(heating_data, garage_data, weather_data);
  }
  else if (millis() - last_time_update >= TIME_UPDATE_INTERVAL)
  {
    DisplayManager::update_time_only();
    last_time_update = millis();
  }
}

void check_screensaver_timeout()
{
  unsigned long current_time = millis();
  unsigned long last_web_activity = WebServerManager::get_last_activity_time();
  
  if (current_time - last_web_activity > TFT_SCREENSAVER_TIMEOUT)
  {
    if (!screensaver_active)
    {
      screensaver_active = true;
      WEB_LOG("Screensaver activated after " + String(TFT_SCREENSAVER_TIMEOUT / 60000) + " min inactivity");
    }
  }
  else
  {
    if (screensaver_active)
    {
      screensaver_active = false;
      DisplayManager::adjust_brightness_for_time();
      WEB_LOG("Screensaver deactivated - user activity detected");
    }
  }
}
