#include <Arduino.h>
#include "libs/plant_nanny/App.h"

plant_nanny::App app;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    
    app.initialize();
}

void loop()
{
    app.run();
}
