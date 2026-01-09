#include <Arduino.h>
#include "libs/plant_nanny/App.h"

plant_nanny::App app;

void setup()
{
    app.initialize();
}

void loop()
{
    app.run();
}
