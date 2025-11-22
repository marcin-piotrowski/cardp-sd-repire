#pragma once

#include <M5Unified.h>
#include <string>

class RepairUi {
public:
    void begin();
    void log(const std::string &message);
    void status(const std::string &line1, const std::string &line2 = "");
};

