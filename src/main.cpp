#include <Arduino.h>

#include "libs/plant_nanny/App.h"

auto app = plant_nanny::App();

void setup() {
    app.initialize();
}

void loop() {
  app.run();
}
