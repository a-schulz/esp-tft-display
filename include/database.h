#include <ESPSupabase.h>
#ifndef DATABASE_H
#define DATABASE_H

#include "display_manager.h"

class Database
{
public:
    static void init();
    static bool fetch_temperature(const String& location, TemperatureData& data);

    static Supabase db;
};

#endif