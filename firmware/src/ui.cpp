#include "ui.h"

#include <M5GFX.h>

void RepairUi::begin() {
    M5.begin();
    M5.Display.setTextFont(1);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(0, 0);
    M5.Display.clear();
    M5.Display.println("CardFix");
    Serial.begin(115200);
}

void RepairUi::log(const std::string &message) {
    Serial.println(message.c_str());
}

void RepairUi::status(const std::string &line1, const std::string &line2) {
    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.println(line1.c_str());
    if (!line2.empty()) {
        M5.Display.println(line2.c_str());
    }
    log(line1 + " " + line2);
}

