# ESP32 TFT Temperature & Weather Display

ESP32-basiertes Temperatur- und Wetter-Display mit 4" TFT-Touchscreen, Supabase-Datenbankintegration, OTA-Updates und Web-Interface.

**Unterstützt ESP32-WROOM und ESP32-C3 Mini**

## 🎯 Features

- ✅ **4" TFT Display (480x320)** - Visualisierung von Temperatur und Wetter
- ✅ **Supabase Integration** - Temperaturdaten aus Cloud-Datenbank
- ✅ **Wetter-API** - Aktuelle Wetterdaten und Vorhersage (Open-Meteo)
- ✅ **OTA Updates** - Drahtlose Firmware-Updates
- ✅ **Web Interface** - Dashboard mit Echtzeit-Daten und System-Logs
- ✅ **WiFiManager** - Einfache WiFi-Konfiguration über Captive Portal
- ✅ **Multi-Location Support** - Verschiedene Temperatur-Sensoren (Garage, Heizung, etc.)
- ✅ **Optimiertes Display** - Kein Flackern, nur Änderungen werden neu gezeichnet
- ✅ **NTP-Synchronisation** - Automatische Zeitaktualisierung

## 🔌 Hardware

### Unterstützte Boards

| Board | Status | Vorteile | Nachteile |
|-------|--------|----------|-----------|
| **ESP32-WROOM** | ✅ Empfohlen | Mehr GPIOs, Dual-Core, etabliert | Größer, teurer |
| **ESP32-C3 Mini** | ✅ Unterstützt | Kleiner, günstiger, native USB | Single-Core, weniger GPIOs |

### Benötigte Komponenten

- **ESP32 DevKit** (WROOM oder C3 Mini)
- **4" TFT Display** mit ILI9488/ILI9486 Controller
  - SPI LCD Display (480x320)
  - I²C Capacitive Touch Panel (optional)
  - SD-Karten-Slot (optional)
- **Micro-USB Kabel** für Stromversorgung und Programmierung

### Pin-Belegung (Wiring)

#### 📺 ESP32-WROOM Pins

| Display Pin | ESP32 GPIO | Funktion            | Hinweis                |
|-------------|------------|---------------------|------------------------|
| SCK         | GPIO 18    | SPI Clock           | Shared mit SD          |
| SDI (MOSI)  | GPIO 23    | SPI Data Out        | Shared mit SD          |
| SDO (MISO)  | GPIO 19    | SPI Data In         | Shared mit SD          |
| LCD_CS      | GPIO 15    | Chip Select LCD     |                        |
| LCD_RS (DC) | GPIO 2     | Data/Command Select | Manchmal als DC        |
| LCD_RST     | GPIO 4     | Reset LCD           |                        |
| LED (BL)    | GPIO 16    | Backlight PWM       | PWM-gesteuert          |

#### 📺 ESP32-C3 Mini Pins (Alternative)

| Display Pin | ESP32-C3 GPIO | Funktion            | Hinweis                |
|-------------|---------------|---------------------|------------------------|
| SCK         | GPIO 6        | SPI Clock           | Shared mit SD          |
| SDI (MOSI)  | GPIO 7        | SPI Data Out        | Shared mit SD          |
| SDO (MISO)  | GPIO 5        | SPI Data In         | Shared mit SD          |
| LCD_CS      | GPIO 10       | Chip Select LCD     |                        |
| LCD_RS (DC) | GPIO 2        | Data/Command Select | Manchmal als DC        |
| LCD_RST     | GPIO 4        | Reset LCD           |                        |
| LED (BL)    | GPIO 3        | Backlight PWM       | PWM-gesteuert          |

#### 👆 Touch Panel (I²C) - Optional (Beide Boards)

| Display Pin | WROOM GPIO | C3 GPIO | Funktion               |
|-------------|------------|---------|------------------------|
| CTP_SDA     | GPIO 21    | GPIO 8  | I²C Data               |
| CTP_SCL     | GPIO 22    | GPIO 9  | I²C Clock              |
| CTP_INT     | GPIO 27    | GPIO 1  | Touch Interrupt        |
| CTP_RST     | 3.3V       | 3.3V    | Touch Reset            |

#### 💾 SD-Karte (SPI) - Optional

| Display Pin | WROOM GPIO | C3 GPIO | Funktion          |
|-------------|------------|---------|-------------------|
| SD_CS       | GPIO 5     | GPIO 0  | Chip Select SD    |

#### ⚡ Stromversorgung (Beide Boards)

| Display Pin | ESP32 Pin | Hinweis                        |
|-------------|-----------|--------------------------------|
| VCC         | 5V        | Von USB-Port oder extern       |
| GND         | GND       | Gemeinsame Masse               |

#### 🔘 Button (Beide Boards)

| Funktion    | GPIO      | Hinweis                        |
|-------------|-----------|--------------------------------|
| BOOT Button | GPIO 0    | Bereits vorhanden auf Board    |

### 📸 Wiring-Diagramm

#### ESP32-WROOM

```
ESP32-WROOM                    4" TFT Display (ILI9488)
┌─────────────┐               ┌──────────────────┐
│             │               │                  │
│      GPIO 18├───────────────┤ SCK              │
│      GPIO 23├───────────────┤ SDI (MOSI)       │
│      GPIO 19├───────────────┤ SDO (MISO)       │
│      GPIO 15├───────────────┤ LCD_CS           │
│       GPIO 2├───────────────┤ LCD_RS (DC)      │
│       GPIO 4├───────────────┤ LCD_RST          │
│      GPIO 16├───────────────┤ LED (Backlight)  │
│             │               │                  │
│       GPIO 5├───────────────┤ SD_CS (optional) │
│             │               │                  │
│      GPIO 21├───────────────┤ CTP_SDA (Touch)  │
│      GPIO 22├───────────────┤ CTP_SCL (Touch)  │
│      GPIO 27├───────────────┤ CTP_INT (Touch)  │
│             │               │                  │
│        3.3V ├───────────────┤ CTP_RST          │
│          5V ├───────────────┤ VCC              │
│         GND ├───────────────┤ GND              │
└─────────────┘               └──────────────────┘
```

#### ESP32-C3 Mini

```
ESP32-C3 Mini                  4" TFT Display (ILI9488)
┌─────────────┐               ┌──────────────────┐
│             │               │                  │
│       GPIO 6├───────────────┤ SCK              │
│       GPIO 7├───────────────┤ SDI (MOSI)       │
│       GPIO 5├───────────────┤ SDO (MISO)       │
│      GPIO 10├───────────────┤ LCD_CS           │
│       GPIO 2├───────────────┤ LCD_RS (DC)      │
│       GPIO 4├───────────────┤ LCD_RST          │
│       GPIO 3├───────────────┤ LED (Backlight)  │
│             │               │                  │
│       GPIO 0├───────────────┤ SD_CS (optional) │
│             │               │                  │
│       GPIO 8├───────────────┤ CTP_SDA (Touch)  │
│       GPIO 9├───────────────┤ CTP_SCL (Touch)  │
│       GPIO 1├───────────────┤ CTP_INT (Touch)  │
│             │               │                  │
│        3.3V ├───────────────┤ CTP_RST          │
│          5V ├───────────────┤ VCC              │
│         GND ├───────────────┤ GND              │
└─────────────┘               └──────────────────┘
```

### 🔧 Wichtige Hinweise zum Wiring

- **LCD_RS = LCD_DC**: Manche Displays nennen den Pin DC (Data/Command), andere RS
- **LED (Backlight)**: PWM-gesteuert über GPIO für Helligkeitsanpassung
- **Shared SPI Bus**: LCD und SD-Karte nutzen denselben SPI-Bus (unterschiedliche CS-Pins)
- **Touch optional**: Funktioniert auch ohne Touch-Panel
- **5V erforderlich**: Display benötigt 5V (nicht 3.3V!)
- **C3 hat weniger Pins**: Prüfe Verfügbarkeit für zusätzliche Erweiterungen

## 🚀 Setup & Installation

### 1. PlatformIO Installation

```bash
# VS Code Extension installieren
# "PlatformIO IDE" in VS Code Extensions suchen und installieren
```

### 2. Repository klonen

```bash
git clone <repository-url>
cd esp-tft-display
```

### 3. Board auswählen

Wähle dein ESP32-Board in `platformio.ini`:

**Für ESP32-WROOM (Standard):**
```bash
pio run -e esp32dev --target upload
```

**Für ESP32-C3 Mini:**
```bash
pio run -e esp32c3 --target upload
```

### 4. Konfiguration

```bash
# Erstelle config.h aus Beispiel-Datei
cp include/config.example.h include/config.h
```

Bearbeite `include/config.h`:

```cpp
// WiFi wird über WiFiManager konfiguriert (Captive Portal)

// Supabase Datenbank
#define SUPABASE_URL "https://dein-projekt.supabase.co"
#define SUPABASE_KEY "dein_anon_key"

// Open-Meteo Wetter-API (kostenlos, kein API-Key!)
#define WEATHER_LATITUDE 51.4295
#define WEATHER_LONGITUDE 13.8594

// Display Helligkeit (0-255)
#define TFT_BACKLIGHT_BRIGHTNESS 100

// Timezone (Deutschland)
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"
```

### 5. Supabase Datenbank Setup

Erstelle eine Tabelle mit RPC-Funktion `latest_measurements`:

```sql
-- Tabelle für Messungen
CREATE TABLE measurements (
  id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
  created_at TIMESTAMPTZ DEFAULT NOW(),
  location TEXT NOT NULL,
  type TEXT NOT NULL,
  value DECIMAL(10,2) NOT NULL
);

CREATE INDEX idx_measurements_location ON measurements(location);
CREATE INDEX idx_measurements_created_at ON measurements(created_at DESC);

-- RPC Funktion für neueste Messungen pro Location/Type
CREATE OR REPLACE FUNCTION latest_measurements()
RETURNS TABLE (
  id UUID,
  created_at TIMESTAMPTZ,
  location TEXT,
  type TEXT,
  value DECIMAL(10,2)
) AS $$
BEGIN
  RETURN QUERY
  SELECT DISTINCT ON (m.location, m.type)
    m.id,
    m.created_at,
    m.location,
    m.type,
    m.value
  FROM measurements m
  ORDER BY m.location, m.type, m.created_at DESC;
END;
$$ LANGUAGE plpgsql;
```

### 6. Hardware verdrahten

Verbinde das Display mit dem ESP32 gemäß der Wiring-Tabelle oben für dein Board.

### 7. Upload

**Erstmaliger Upload über USB:**

```bash
# ESP32-WROOM
pio run -e esp32dev --target upload

# ESP32-C3 Mini
pio run -e esp32c3 --target upload

# Port manuell angeben (falls Auto-Detection fehlschlägt)
pio run -e esp32dev --target upload --upload-port /dev/ttyUSB0
pio run -e esp32c3 --target upload --upload-port /dev/ttyACM0
```

**Spätere Updates über WiFi (OTA):**

```bash
# IP-Adresse in platformio.ini eintragen, dann:
pio run -e esp32dev --target upload
```

### 8. WiFi-Konfiguration

Bei der ersten Inbetriebnahme:

1. ESP32 startet als Access Point: **"ESP32-TFT-Setup"**
2. Mit Smartphone/Laptop verbinden
3. Captive Portal öffnet sich automatisch
4. WiFi-Credentials eingeben
5. Speichern → ESP32 startet neu und verbindet sich

## 📊 Update-Intervalle

- ⏱️ **Zeit**: Alle 10 Sekunden (nur bei Änderung aktualisiert - kein Flackern)
- 🌡️ **Temperatur**: Alle 60 Sekunden (Supabase RPC-Abfrage)
- 🌤️ **Wetter**: Alle 10 Minuten (Open-Meteo API)

Display wird nur neu gezeichnet wenn sich Werte ändern!

## 🌐 Web Interface

Nach WiFi-Verbindung erreichbar unter:

- **http://esp32-tft.local** (mDNS)
- **http://192.168.x.x** (IP-Adresse)

### Verfügbare Seiten

- `/` - Dashboard mit aktuellen Werten und Echtzeit-Updates
- `/logs` - System-Logs mit Auto-Refresh
- `/api/data` - JSON API für externe Integration

## 🎨 Display Layout

```
┌─────────────────────────────────────┐
│           12:34                     │  ← Zentrierte Zeit
├──────────────────┬──────────────────┤
│                  │                  │
│   🔥 Heizung     │   ☀️ Wetter      │
│                  │                  │
│     78.6         │     15.2         │  ← Große Zahlen
│                  │                  │
│                  │   NO              │  ← Wind-Richtung
│   Heizung        │   12 km/h        │  ← Wind-Speed
│                  │   60%            │  ← Luftfeuchtigkeit
│                  │   Gefühlt 14.5   │  ← Gefühlte Temp
│                  │                  │
│                  │   Wetter         │
└──────────────────┴──────────────────┘
```

## 🛡️ Display-Schutz für 24/7-Betrieb

### Häufige Probleme bei Dauerbetrieb

| Problem | Ursache | Lösung |
|---------|---------|--------|
| **Burn-In** | Statische Elemente bleiben dauerhaft sichtbar | Pixel-Shifting, Screensaver, Content-Rotation |
| **Backlight-Degradation** | LED-Backlight altert bei 100% Helligkeit schnell | Reduzierte Helligkeit, Auto-Dimming |
| **Überhitzung** | Kontinuierlicher Betrieb ohne Kühlung | Gehäuse mit Belüftung, Temperatur-Monitoring |
| **Stuck Pixels** | Einzelne Pixel bleiben hängen | Pixel-Refresh-Zyklen |
| **Kondensator-Alterung** | Elektrische Komponenten altern | Qualitativ hochwertiges Display, regelmäßige Wartung |

### 🔧 Implementierte Schutzmaßnahmen

#### 1. Reduzierte Helligkeit (Standard: 100/255 = 39%)
```cpp
#define TFT_BACKLIGHT_BRIGHTNESS 100  // In config.h
```

#### 2. Automatische Helligkeitsanpassung nach Tageszeit
- **Tag (6-22 Uhr)**: Normale Helligkeit (100)
- **Nacht (22-6 Uhr)**: Reduzierte Helligkeit (30)

#### 3. Screensaver-Modus
- Nach **10 Minuten Inaktivität** (keine Webserver-Zugriffe)
- Display zeigt weiterhin Daten, aber mit:
  - Pixel-Shifting (±5 Pixel horizontal/vertikal alle 60s)
  - Reduzierte Helligkeit (50%)

#### 4. Temperatur-Monitoring
- ESP32 interne Temperatur-Überwachung
- Warnung bei >75°C
- Automatische Helligkeitsreduktion bei >80°C

#### 5. Pixel-Refresh-Zyklen
- Einmal pro Woche vollständiges Display-Refresh
- Wechselnde Hintergrundfarben zur Pixel-Regeneration

### 📋 Zusätzliche Hardware-Empfehlungen

#### Gehäuse & Kühlung

**Passiv:**
- Gehäuse mit Belüftungsschlitzen
- Aluminium-Gehäuse als Kühlkörper
- Abstand zwischen Display und ESP32 (Wärmebrücke vermeiden)

**Aktiv (bei hohen Temperaturen):**
- 5V Lüfter (40x40mm) bei Temperaturen >60°C
- Ansteuerung über MOSFET/Transistor

#### Stromversorgung

- **Qualitativ hochwertiges 5V/2A Netzteil** verwenden
- Spannungsschwankungen können Display beschädigen
- Optional: Kondensator (1000µF) nahe Display für stabile Versorgung

#### Display-Auswahl

**Langlebigere Display-Typen:**
- IPS-Displays (bessere Blickwinkel, weniger Burn-In)
- Industrielle TFT-Displays (längere MTBF)
- OLED vermeiden (hohe Burn-In-Gefahr bei statischem Content)

### ⚙️ Konfigurierbare Optionen

In `config.h`:

```cpp
// Display Lebensdauer-Optionen
#define TFT_BACKLIGHT_BRIGHTNESS 100        // Basis-Helligkeit (0-255)
#define TFT_NIGHT_BRIGHTNESS 30             // Nacht-Helligkeit (22-6 Uhr)
#define TFT_SCREENSAVER_TIMEOUT 600000      // 10 Minuten in ms
#define TFT_SCREENSAVER_BRIGHTNESS 50       // Screensaver-Helligkeit
#define TFT_PIXEL_SHIFT_ENABLED true        // Pixel-Shifting aktivieren
#define TFT_PIXEL_SHIFT_INTERVAL 60000      // Alle 60 Sekunden
#define TFT_MAX_SAFE_TEMP 80                // Max. sichere Temperatur (°C)
#define TFT_WEEKLY_REFRESH_ENABLED true     // Wöchentlicher Full-Refresh
```

### 🔍 Monitoring & Wartung

#### Web-Interface Erweiterungen

Dashboard zeigt:
- **Display-Betriebszeit** (Total hours on)
- **Aktuelle Helligkeit** (%)
- **Interne Temperatur** (ESP32 + optional externer Sensor)
- **Screensaver-Status**
- **Letzte Pixel-Refresh-Zeit**

#### Log-Warnungen

```
⚠️ Display temperature high: 78°C - reducing brightness
ℹ️ Screensaver activated after 10 min inactivity
✓ Weekly pixel refresh completed
```

### 📊 Erwartete Lebensdauer

Bei korrekter Implementierung:

| Komponente | Ohne Schutz | Mit Schutz | Verbesserung |
|------------|--------------|------------|--------------|
| **LED Backlight** | ~10.000h (1.1 Jahre) | ~30.000h (3.4 Jahre) | 3x |
| **LCD Panel** | ~20.000h (2.3 Jahre) | ~50.000h (5.7 Jahre) | 2.5x |
| **ESP32** | 10+ Jahre | 10+ Jahre | - |

*Bei 24/7 Betrieb*

### 🚨 Notfall-Features

#### Überhitzungsschutz

```cpp
if (temperature > 85°C) {
    // Kritische Überhitzung
    tft.fillScreen(TFT_BLACK);
    analogWrite(TFT_BACKLIGHT_PIN, 0);  // Backlight AUS
    display_error_message("TEMP CRITICAL - DISPLAY OFF");
}
```

#### Burn-In Test-Modus

Aktivierbar über Web-Interface:
- Wechselt durch verschiedene Vollbild-Farben (Weiß, Schwarz, Rot, Grün, Blau)
- Hilft bei der Erkennung von Burn-In und Stuck Pixels
- Durchlauf alle 30 Sekunden für 5 Minuten

### 💡 Best Practices

1. **Regelmäßige Neustarts**: Einmal pro Woche (automatisch via RTC)
2. **Statischen Content minimieren**: Wechselnde Layouts
3. **Dunkler Hintergrund**: Spart Backlight-Energie und Wärme
4. **Luftzirkulation**: Display sollte nicht vollständig eingeschlossen sein
5. **Umgebungstemperatur**: <30°C optimal
6. **Direkte Sonneneinstrahlung vermeiden**: UV-Strahlung schadet LCD

### 🔄 Wartungsplan

**Täglich:**
- Automatisches Temperatur-Monitoring
- Helligkeitsanpassung nach Tageszeit

**Wöchentlich:**
- Automatischer Neustart (Sonntag 3:00 Uhr)
- Vollständiger Display-Refresh-Zyklus
- Log-Review im Web-Interface

**Monatlich:**
- Visuelle Inspektion auf Burn-In
- Staub entfernen (beeinträchtigt Kühlung)
- Firmware-Updates prüfen

**Jährlich:**
- Display auf Stuck Pixels prüfen
- Netzteil auf Spannungsstabilität testen
- Backup der Konfiguration

---

## 🚀 Aktivierung der Schutzfunktionen

Alle Schutzmaßnahmen sind standardmäßig aktiviert. Um sie anzupassen:

1. Bearbeite `include/config.h`
2. Passe die `TFT_*` Defines an
3. Neu kompilieren und uploaden
4. Überwache die ersten 24h im Web-Interface

**Empfohlen für maximale Lebensdauer:**
- Helligkeit: 80-100 (31-39%)
- Screensaver: aktiviert
- Pixel-Shifting: aktiviert
- Nacht-Modus: aktiviert
- Wöchentlicher Refresh: aktiviert

## 🔧 Entwicklung

### Build-Befehle

```bash
# Kompilieren
pio run -e esp32dev        # ESP32-WROOM
pio run -e esp32c3         # ESP32-C3

# Upload über USB
pio run -e esp32dev --target upload
pio run -e esp32c3 --target upload

# Serial Monitor
pio device monitor

# Clean
pio run --target clean
```

### Board wechseln

Um zwischen ESP32-WROOM und ESP32-C3 zu wechseln:

1. Hardware entsprechend umverdrahten (siehe Pin-Tabellen oben)
2. Korrektes Environment beim Build angeben: `-e esp32dev` oder `-e esp32c3`
3. Keine Code-Änderungen nötig!

### Libraries

Automatisch installiert via `platformio.ini`:

- **TFT_eSPI** - Display-Treiber
- **ESPSupabase** - Supabase Client
- **ArduinoJson** - JSON Parsing
- **ESPAsyncWebServer** - Web Interface
- **WiFiManager** - WiFi-Konfiguration
- **HTTPClient** - Weather API Requests

## 🌦️ Wetter-API

Das Projekt nutzt **Open-Meteo** (https://open-meteo.com), eine **kostenlose** Wetter-API:

- ✅ **Kein API-Key erforderlich**
- ✅ Kostenlos für nicht-kommerzielle Nutzung
- ✅ Bis zu 10.000 Anfragen/Tag
- ✅ Aktuelle Wetterdaten + Vorhersage
- ✅ Deutsche Einheiten (°C, km/h)

### Verfügbare Wetterdaten

- Temperatur (aktuell)
- Gefühlte Temperatur
- Luftfeuchtigkeit
- Windgeschwindigkeit (km/h)
- Windrichtung (Himmelsrichtung)
- Niederschlag (mm)
- Tag/Nacht Status

### Konfiguration

In `config.h` nur Koordinaten angeben:

```cpp
#define WEATHER_LATITUDE 51.4295   // Riesa
#define WEATHER_LONGITUDE 13.8594
```

Finde deine Koordinaten auf: https://www.latlong.net/

## 🐛 Troubleshooting

### Display bleibt weiß/schwarz

- ✅ Wiring überprüfen (besonders CS, DC, RST Pins)
- ✅ 5V Versorgung sicherstellen (nicht 3.3V!)
- ✅ Korrektes Environment gewählt? (`esp32dev` vs `esp32c3`)
- ✅ TFT_eSPI Konfiguration via Build-Flags in `platformio.ini`

### Display flackert

- ✅ Sollte nicht mehr vorkommen - optimiertes Update nur bei Änderungen
- ✅ Zeit wird nur jede Minute neu gezeichnet
- ✅ Panels nur bei Datenänderung aktualisiert

### Keine WiFi-Verbindung

- ✅ Button (GPIO 0) 3 Sekunden gedrückt halten
- ✅ WiFi-Konfiguration wird zurückgesetzt
- ✅ ESP32 startet als Access Point neu

### Keine Supabase-Daten

- ✅ Supabase URL und Key in `config.h` prüfen
- ✅ RPC-Funktion `latest_measurements()` existiert
- ✅ Row Level Security (RLS) deaktiviert oder richtig konfiguriert
- ✅ Logs im Web Interface prüfen: http://esp32-tft.local/logs
- ✅ Location-Namen korrekt (lowercase: "garage", "heating")

### Kein Upload möglich

**ESP32-WROOM:**
- ✅ USB-Kabel mit Datenübertragung verwenden
- ✅ CP2102/CH340 Treiber installiert
- ✅ Port: `/dev/ttyUSB0` (Linux), `COM3` (Windows)

**ESP32-C3:**
- ✅ Native USB - kein extra Treiber nötig
- ✅ Port: `/dev/ttyACM0` (Linux), `COM4` (Windows)
- ✅ Manchmal BOOT-Button beim Upload drücken

### OTA-Update funktioniert nicht

- ✅ ESP32 muss eingeschaltet sein (nicht im Deep Sleep)
- ✅ Port 3232 in Firewall freigeben
- ✅ IP-Adresse in `platformio.ini` aktualisieren
- ✅ OTA-Passwort korrekt in `config.h`

### ESP32-C3 spezifische Probleme

- ✅ Weniger GPIOs - keine Pins mehr für Erweiterungen
- ✅ Single-Core - etwas langsamere Performance
- ✅ ttyACM statt ttyUSB Port verwenden
- ✅ Native USB kann beim ersten Mal zickig sein

## 📊 Board-Vergleich

| Feature | ESP32-WROOM | ESP32-C3 Mini | Gewinner |
|---------|-------------|---------------|----------|
| **Preis** | ~4 EUR | ~2 EUR | 🏆 C3 |
| **Größe** | 25.5x18mm | 13x16.6mm | 🏆 C3 |
| **GPIO Pins** | 34+ | 22 | 🏆 WROOM |
| **CPU Cores** | 2x 240MHz | 1x 160MHz | 🏆 WROOM |
| **RAM** | 520 KB | 400 KB | 🏆 WROOM |
| **Flash** | 4-16 MB | 4 MB | 🏆 WROOM |
| **USB** | Extern (CP2102) | Nativ | 🏆 C3 |
| **Bluetooth** | Classic + BLE | Nur BLE 5.0 | 🏆 WROOM |
| **Stromverbrauch** | Höher | Niedriger | 🏆 C3 |
| **Zukunft** | Etabliert | RISC-V Modern | 🏆 C3 |

**Empfehlung**: ESP32-WROOM für maximale Kompatibilität, ESP32-C3 für kompakte Projekte

## 📝 Lizenz

MIT License

## 🙏 Credits

- **TFT_eSPI** by Bodmer
- **ESPSupabase** by jhagas
- **WiFiManager** by tzapu
- **Open-Meteo** - Kostenlose Wetter-API
- **ESPAsyncWebServer** by me-no-dev
