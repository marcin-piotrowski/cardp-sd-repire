#pragma once

#include "sector_io.h"
#include <cstdint>

namespace cardfix {

struct FormatOptions {
    uint8_t sectors_per_cluster = 8;
    uint16_t reserved_sectors = 32;
    uint8_t fat_count = 2;
    uint32_t root_dir_entries = 0;  // For FAT32 this must be 0
    uint8_t media_descriptor = 0xF8;
    uint32_t volume_id = 0x12345678;
};

struct Fat32Layout {
    uint32_t total_sectors = 0;
    uint32_t fat_size_sectors = 0;
    uint32_t first_data_sector = 0;
    uint32_t cluster_count = 0;
    uint32_t root_dir_cluster = 2;
};

class Formatter {
public:
    static Fat32Layout computeLayout(const CardGeometry &geometry, const FormatOptions &options);
    static bool writeMbRandFat32(RawCard &card, const CardGeometry &geometry, const FormatOptions &options);
};

}  // namespace cardfix
