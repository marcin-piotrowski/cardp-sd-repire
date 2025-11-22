#pragma once

#include "sector_io.h"
#include "format.h"
#include <string>

namespace cardfix {

struct DiagnosticReport {
    bool card_present = false;
    bool geometry_valid = false;
    bool pattern_success = false;
    Fat32Layout layout{};
    std::string message;
};

class Diagnostics {
public:
    static DiagnosticReport quickTest(RawCard &card, const CardGeometry &geometry);

private:
    static bool writePattern(RawCard &card, uint32_t lba, uint8_t pattern);
    static bool verifyPattern(RawCard &card, uint32_t lba, uint8_t pattern);
};

}  // namespace cardfix
