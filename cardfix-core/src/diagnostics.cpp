#include "cardfix/diagnostics.h"

#include <sstream>

namespace cardfix {

DiagnosticReport Diagnostics::quickTest(RawCard &card, const CardGeometry &geometry) {
    DiagnosticReport report{};
    report.card_present = card.sectorCount() > 0;
    report.geometry_valid = geometry.sector_count >= 1024 && geometry.sector_size == kSectorSize;

    if (!report.card_present || !report.geometry_valid) {
        report.message = "Card unavailable or geometry invalid";
        return report;
    }

    // Simple destructive test on sectors 1 and 2
    if (!writePattern(card, 1, 0xA5) || !verifyPattern(card, 1, 0xA5)) {
        report.message = "Pattern test failed at sector 1";
        return report;
    }
    if (!writePattern(card, 2, 0x5A) || !verifyPattern(card, 2, 0x5A)) {
        report.message = "Pattern test failed at sector 2";
        return report;
    }

    report.pattern_success = true;
    report.layout = Formatter::computeLayout(geometry, {});

    std::ostringstream oss;
    oss << "Sectors: " << geometry.sector_count << ", FAT size sectors: " << report.layout.fat_size_sectors
        << ", clusters: " << report.layout.cluster_count;
    report.message = oss.str();
    return report;
}

bool Diagnostics::writePattern(RawCard &card, uint32_t lba, uint8_t pattern) {
    Sector tmp{};
    tmp.fill(pattern);
    return card.writeSector(lba, tmp);
}

bool Diagnostics::verifyPattern(RawCard &card, uint32_t lba, uint8_t pattern) {
    Sector tmp{};
    if (!card.readSector(lba, tmp)) {
        return false;
    }
    for (auto b : tmp) {
        if (b != pattern) {
            return false;
        }
    }
    return true;
}

}  // namespace cardfix
