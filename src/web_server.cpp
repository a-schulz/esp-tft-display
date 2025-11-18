#include "web_server.h"
#include "logger.h"
#include "network_manager.h"
#include "config.h"
#include <WiFi.h>

AsyncWebServer *WebServerManager::server = nullptr;
unsigned long WebServerManager::last_web_request = 0;
bool WebServerManager::initialized = false;

TemperatureData WebServerManager::current_heating = {0, 0, false, "heating"};
TemperatureData WebServerManager::current_garage = {0, 0, false, "garage"};
WeatherData WebServerManager::current_weather = {0.0f, 0, "", 0, 0, 0.0f, 0.0f, true, "", false};

void WebServerManager::init()
{
    if (initialized)
        return;

    server = new AsyncWebServer(WEB_SERVER_PORT);
    setup_routes();
    server->begin();
    initialized = true;
    WEB_LOG("Web server started on http://" + WiFi.localIP().toString());
}

void WebServerManager::track_activity()
{
    last_web_request = millis();
}

bool WebServerManager::has_recent_activity()
{
    return (millis() - last_web_request < 60000);
}

unsigned long WebServerManager::get_last_activity_time()
{
    return last_web_request;
}

void WebServerManager::update_sensor_data(const TemperatureData& heating, const TemperatureData& garage, const WeatherData& weather)
{
    current_heating = heating;
    current_garage = garage;
    current_weather = weather;
}

void WebServerManager::setup_routes()
{
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               {
        track_activity();
        request->send(200, "text/html", build_dashboard_html()); });

    server->on("/logs", HTTP_GET, [](AsyncWebServerRequest *request)
               {
        track_activity();
        request->send(200, "text/html", build_logs_html()); });

    server->on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request)
               {
        track_activity();
        String json = "{";
        json += "\"heating\":{\"temp\":" + String(current_heating.temperature, 1);
        json += ",\"humidity\":" + String(current_heating.humidity, 0);
        json += ",\"valid\":" + String(current_heating.valid ? "true" : "false") + "},";
        json += "\"garage\":{\"temp\":" + String(current_garage.temperature, 1);
        json += ",\"humidity\":" + String(current_garage.humidity, 0);
        json += ",\"valid\":" + String(current_garage.valid ? "true" : "false") + "},";
        json += "\"weather\":{\"temp\":" + String(current_weather.temperature, 1);
        json += ",\"humidity\":" + String(current_weather.humidity);
        json += ",\"wind\":" + String(current_weather.wind_speed);
        json += ",\"wind_dir\":" + String(current_weather.wind_direction);
        json += ",\"precipitation\":" + String(current_weather.precipitation, 1);
        json += ",\"feels_like\":" + String(current_weather.apparent_temperature, 1);
        json += ",\"description\":\"" + current_weather.description + "\"";
        json += ",\"valid\":" + String(current_weather.valid ? "true" : "false") + "}";
        json += "}";
        request->send(200, "application/json", json); });
}

String WebServerManager::build_dashboard_html()
{
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<title>Temperature Display</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>";
    html += "body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;margin:0;padding:20px;background:#0d1117;color:#c9d1d9}";
    html += ".container{max-width:1200px;margin:0 auto}";
    html += "h1{color:#58a6ff;margin-bottom:30px}";
    html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:20px;margin-bottom:20px}";
    html += ".card{background:#161b22;border:1px solid #30363d;border-radius:6px;padding:20px;box-shadow:0 1px 3px rgba(0,0,0,0.12)}";
    html += ".card h2{margin:0 0 15px 0;color:#58a6ff;font-size:1.2em;display:flex;align-items:center;gap:10px}";
    html += ".temp{font-size:3em;font-weight:bold;color:#fff;margin:10px 0}";
    html += ".temp-unit{font-size:0.5em;color:#8b949e}";
    html += ".metric{display:flex;justify-content:space-between;padding:8px 0;border-bottom:1px solid #21262d}";
    html += ".metric:last-child{border-bottom:none}";
    html += ".metric-label{color:#8b949e}";
    html += ".metric-value{color:#c9d1d9;font-weight:500}";
    html += ".status{padding:10px;border-radius:4px;margin:10px 0;font-size:0.9em}";
    html += ".valid{background:#1a7f37;color:#c9d1d9;border:1px solid #2ea043}";
    html += ".invalid{background:#8b2400;color:#c9d1d9;border:1px solid #da3633}";
    html += ".info{background:#1f6feb;color:#c9d1d9;border:1px solid #388bfd}";
    html += ".btn-container{display:flex;gap:10px;flex-wrap:wrap;margin-top:20px}";
    html += ".btn{background:#238636;color:#fff;padding:12px 24px;border:none;border-radius:6px;cursor:pointer;text-decoration:none;display:inline-block;font-size:14px;font-weight:500}";
    html += ".btn:hover{background:#2ea043}";
    html += ".btn-secondary{background:#30363d}";
    html += ".btn-secondary:hover{background:#484f58}";
    html += ".icon{font-size:1.5em}";
    html += "@media(max-width:768px){.grid{grid-template-columns:1fr}}";
    html += "</style>";
    html += "<script>";
    html += "async function updateData(){";
    html += "try{";
    html += "const res=await fetch('/api/data');";
    html += "const data=await res.json();";
    html += "document.getElementById('heating-temp').textContent=data.heating.valid?data.heating.temp.toFixed(1):'--';";
    html += "document.getElementById('heating-hum').textContent=data.heating.valid?Math.round(data.heating.humidity)+'%':'--';";
    html += "document.getElementById('garage-temp').textContent=data.garage.valid?data.garage.temp.toFixed(1):'--';";
    html += "document.getElementById('garage-hum').textContent=data.garage.valid?Math.round(data.garage.humidity)+'%':'--';";
    html += "document.getElementById('weather-temp').textContent=data.weather.valid?data.weather.temp.toFixed(1):'--';";
    html += "document.getElementById('weather-feels').textContent=data.weather.valid?data.weather.feels_like.toFixed(1)+'°C':'--';";
    html += "document.getElementById('weather-desc').textContent=data.weather.valid?data.weather.description:'--';";
    html += "document.getElementById('weather-wind').textContent=data.weather.valid?data.weather.wind+' km/h ('+data.weather.wind_dir+'°)':'--';";
    html += "document.getElementById('weather-hum').textContent=data.weather.valid?data.weather.humidity+'%':'--';";
    html += "document.getElementById('weather-precip').textContent=data.weather.valid&&data.weather.precipitation>0?data.weather.precipitation.toFixed(1)+' mm':'--';";
    html += "}catch(e){console.error('Update failed:',e);}";
    html += "}";
    html += "setInterval(updateData,5000);";
    html += "</script>";
    html += "</head><body>";

    html += "<div class='container'>";
    html += "<h1>🌡️ Temperature & Weather Display</h1>";

    html += "<div class='grid'>";
    
    html += "<div class='card'>";
    html += "<h2><span class='icon'>🔥</span> Heizung</h2>";
    if (current_heating.valid) {
        html += "<div class='temp'><span id='heating-temp'>" + String(current_heating.temperature, 1) + "</span><span class='temp-unit'>°C</span></div>";
        html += "<div class='metric'><span class='metric-label'>Luftfeuchtigkeit</span><span class='metric-value' id='heating-hum'>" + String((int)current_heating.humidity) + "%</span></div>";
        html += "<div class='status valid'>✓ Online</div>";
    } else {
        html += "<div class='temp'><span id='heating-temp'>--</span><span class='temp-unit'>°C</span></div>";
        html += "<div class='status invalid'>✗ Keine Daten</div>";
    }
    html += "</div>";

    html += "<div class='card'>";
    html += "<h2><span class='icon'>🚗</span> Garage</h2>";
    if (current_garage.valid) {
        html += "<div class='temp'><span id='garage-temp'>" + String(current_garage.temperature, 1) + "</span><span class='temp-unit'>°C</span></div>";
        html += "<div class='metric'><span class='metric-label'>Luftfeuchtigkeit</span><span class='metric-value' id='garage-hum'>" + String((int)current_garage.humidity) + "%</span></div>";
        html += "<div class='status valid'>✓ Online</div>";
    } else {
        html += "<div class='temp'><span id='garage-temp'>--</span><span class='temp-unit'>°C</span></div>";
        html += "<div class='status invalid'>✗ Keine Daten</div>";
    }
    html += "</div>";

    html += "<div class='card'>";
    html += "<h2><span class='icon'>🌤️</span> Wetter (Open-Meteo)</h2>";
    if (current_weather.valid) {
        html += "<div class='temp'><span id='weather-temp'>" + String(current_weather.temperature, 1) + "</span><span class='temp-unit'>°C</span></div>";
        html += "<div class='metric'><span class='metric-label'>Gefühlt</span><span class='metric-value' id='weather-feels'>" + String(current_weather.apparent_temperature, 1) + "°C</span></div>";
        html += "<div class='metric'><span class='metric-label'>Bedingung</span><span class='metric-value' id='weather-desc'>" + current_weather.description + "</span></div>";
        html += "<div class='metric'><span class='metric-label'>Wind</span><span class='metric-value' id='weather-wind'>" + String(current_weather.wind_speed) + " km/h (" + String(current_weather.wind_direction) + "°)</span></div>";
        html += "<div class='metric'><span class='metric-label'>Luftfeuchtigkeit</span><span class='metric-value' id='weather-hum'>" + String(current_weather.humidity) + "%</span></div>";
        if (current_weather.precipitation > 0.0f) {
            html += "<div class='metric'><span class='metric-label'>Niederschlag</span><span class='metric-value' id='weather-precip'>" + String(current_weather.precipitation, 1) + " mm</span></div>";
        }
        html += "<div class='status valid'>✓ Aktualisiert</div>";
    } else {
        html += "<div class='temp'><span id='weather-temp'>--</span><span class='temp-unit'>°C</span></div>";
        html += "<div class='status invalid'>✗ Nicht verfügbar</div>";
    }
    html += "</div>";
    
    html += "</div>";

    html += "<div class='card'>";
    html += "<h2><span class='icon'>⚙️</span> System Information</h2>";
    html += "<div class='metric'><span class='metric-label'>Hostname</span><span class='metric-value'>" + String(OTA_HOSTNAME) + "</span></div>";
    html += "<div class='metric'><span class='metric-label'>IP Address</span><span class='metric-value'>" + WiFi.localIP().toString() + "</span></div>";
    html += "<div class='metric'><span class='metric-label'>MAC Address</span><span class='metric-value'>" + WiFi.macAddress() + "</span></div>";
    html += "<div class='metric'><span class='metric-label'>WiFi Signal</span><span class='metric-value'>" + String(WiFi.RSSI()) + " dBm</span></div>";
    html += "<div class='metric'><span class='metric-label'>Free Heap</span><span class='metric-value'>" + String(ESP.getFreeHeap() / 1024) + " KB</span></div>";
    html += "<div class='metric'><span class='metric-label'>Uptime</span><span class='metric-value'>" + String(millis() / 60000) + " min</span></div>";
    
    float esp_temp = DisplayManager::get_esp32_temperature();
    String temp_color = esp_temp > TEMP_WARNING_THRESHOLD ? "color:#da3633" : "color:#2ea043";
    html += "<div class='metric'><span class='metric-label'>ESP32 Temperature</span><span class='metric-value' style='" + temp_color + "'>" + String(esp_temp, 1) + "°C</span></div>";
    
    html += "</div>";
    
    html += "<div class='card'>";
    html += "<h2><span class='icon'>🖥️</span> Display Status (24/7 Mode)</h2>";
    html += "<div class='metric'><span class='metric-label'>Display Uptime</span><span class='metric-value'>" + String(DisplayManager::get_display_uptime_hours()) + " hours</span></div>";
    html += "<div class='metric'><span class='metric-label'>Current Brightness</span><span class='metric-value'>" + String((DisplayManager::get_current_brightness() * 100) / 255) + "%</span></div>";
    html += "<div class='metric'><span class='metric-label'>Screensaver</span><span class='metric-value'>" + String(screensaver_active ? "Active" : "Inactive") + "</span></div>";
    html += "<div class='metric'><span class='metric-label'>Pixel Shift</span><span class='metric-value'>" + String(TFT_PIXEL_SHIFT_ENABLED ? "Enabled" : "Disabled") + "</span></div>";
    html += "<div class='metric'><span class='metric-label'>Auto Brightness</span><span class='metric-value'>" + String(TFT_AUTO_BRIGHTNESS_ENABLED ? "Enabled" : "Disabled") + "</span></div>";
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
        html += "<div class='metric'><span class='metric-label'>Current Time</span><span class='metric-value'>" + String(time_str) + "</span></div>";
    }
    html += "</div>";

    html += "<div class='btn-container'>";
    html += "<a href='/logs' class='btn btn-secondary'>📋 View Logs</a>";
    html += "<button onclick='updateData()' class='btn'>🔄 Refresh Data</button>";
    html += "</div>";

    html += "</div>";
    html += "</body></html>";
    return html;
}

String WebServerManager::build_logs_html()
{
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<title>System Logs</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<meta http-equiv='refresh' content='5'>";
    html += "<style>";
    html += "body{font-family:monospace;margin:0;padding:20px;background:#0d1117;color:#58a6ff}";
    html += ".container{max-width:1200px;margin:0 auto}";
    html += "h1{color:#58a6ff;margin-bottom:20px}";
    html += ".log{padding:8px;margin:2px 0;background:#161b22;border-left:3px solid #30363d;border-radius:3px;font-size:13px}";
    html += ".back-link{color:#58a6ff;text-decoration:none;display:inline-block;margin-bottom:20px;padding:8px 16px;background:#21262d;border-radius:6px}";
    html += ".back-link:hover{background:#30363d}";
    html += ".refresh-info{color:#8b949e;font-size:0.9em;margin-bottom:20px}";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<h1>📋 System Logs</h1>";
    html += "<a href='/' class='back-link'>← Back to Dashboard</a>";
    html += "<div class='refresh-info'>Auto-refresh: 5 seconds</div>";

    html += Logger::getLogsHtml();

    html += "</div></body></html>";
    return html;
}
