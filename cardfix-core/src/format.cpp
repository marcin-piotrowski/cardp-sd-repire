#include "cardfix/format.h"

#include <algorithm>
#include <array>
#include <cstring>

namespace cardfix {

namespace {
uint32_t alignUp(uint32_t value, uint32_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

void write32(uint8_t *buf, uint16_t offset, uint32_t value) {
    buf[offset] = value & 0xFF;
    buf[offset + 1] = (value >> 8) & 0xFF;
    buf[offset + 2] = (value >> 16) & 0xFF;
    buf[offset + 3] = (value >> 24) & 0xFF;
}

void write16(uint8_t *buf, uint16_t offset, uint16_t value) {
    buf[offset] = value & 0xFF;
    buf[offset + 1] = (value >> 8) & 0xFF;
}

}  // namespace

Fat32Layout Formatter::computeLayout(const CardGeometry &geometry, const FormatOptions &options) {
    Fat32Layout layout{};
    layout.total_sectors = static_cast<uint32_t>(geometry.sector_count);
    const uint32_t sectors_per_cluster = options.sectors_per_cluster;

    const uint32_t data_sectors_estimate = layout.total_sectors - options.reserved_sectors;
    const uint32_t clusters_estimate = data_sectors_estimate / sectors_per_cluster;
    // fat_size = ceil((clusters_estimate + 2) * 4 / sector_size)
    const uint32_t fat_bytes = (clusters_estimate + 2) * 4;
    layout.fat_size_sectors = alignUp(fat_bytes, geometry.sector_size) / geometry.sector_size;
    layout.first_data_sector = options.reserved_sectors + options.fat_count * layout.fat_size_sectors;
    layout.cluster_count = (layout.total_sectors - layout.first_data_sector) / sectors_per_cluster;
    return layout;
}

bool Formatter::writeMbRandFat32(RawCard &card, const CardGeometry &geometry, const FormatOptions &options) {
    if (geometry.sector_count < 1000 || geometry.sector_size != kSectorSize) {
        return false;
    }

    const auto layout = computeLayout(geometry, options);

    Sector mbr{};
    mbr.fill(0);
    // Partition entry begins at offset 446
    const uint32_t first_lba = 1;  // boot sector
    const uint32_t total_lba = layout.total_sectors - first_lba;
    // status
    mbr[446] = 0x00;
    // CHS not used
    mbr[450] = 0x0C;  // FAT32 LBA type
    write32(mbr.data(), 454, first_lba);
    write32(mbr.data(), 458, total_lba);
    mbr[510] = 0x55;
    mbr[511] = 0xAA;

    if (!card.writeSector(0, mbr)) {
        return false;
    }

    Sector vbr{};
    vbr.fill(0);
    // Jump and OEM
    vbr[0] = 0xEB;
    vbr[1] = 0x58;
    vbr[2] = 0x90;
    std::memcpy(&vbr[3], "MSDOS5.0", 8);
    write16(vbr.data(), 11, geometry.sector_size);
    vbr[13] = options.sectors_per_cluster;
    write16(vbr.data(), 14, options.reserved_sectors);
    vbr[16] = options.fat_count;
    write16(vbr.data(), 17, options.root_dir_entries);
    // Total sectors 16-bit is zero
    vbr[21] = options.media_descriptor;
    write16(vbr.data(), 24, 0);  // FAT12/16 sectors per FAT
    write16(vbr.data(), 26, 63); // sectors per track (placeholder)
    write16(vbr.data(), 28, 255); // heads (placeholder)
    write32(vbr.data(), 32, 0);  // hidden sectors
    write32(vbr.data(), 36, layout.total_sectors);
    write32(vbr.data(), 40, layout.fat_size_sectors);
    write16(vbr.data(), 44, 0);  // flags
    write16(vbr.data(), 46, 0x0000);
    write16(vbr.data(), 48, 0x0000);
    write32(vbr.data(), 44 + 4, layout.root_dir_cluster);
    write16(vbr.data(), 64, options.reserved_sectors);
    write16(vbr.data(), 66, options.sectors_per_cluster);
    write32(vbr.data(), 67, options.volume_id);
    std::memcpy(&vbr[71], "CARDFIX", 7);
    std::memcpy(&vbr[82], "FAT32   ", 8);
    vbr[510] = 0x55;
    vbr[511] = 0xAA;

    if (!card.writeSector(first_lba, vbr)) {
        return false;
    }

    // Clear FAT and root directory clusters
    const uint32_t fat_start = options.reserved_sectors + first_lba;
    Sector fat_sector{};
    fat_sector.fill(0);
    // FAT32 first sector initialization
    // FAT[0]
    fat_sector[0] = options.media_descriptor;
    fat_sector[1] = 0xFF;
    fat_sector[2] = 0xFF;
    fat_sector[3] = 0x0F;
    // FAT[1]
    fat_sector[4] = 0xFF;
    fat_sector[5] = 0xFF;
    fat_sector[6] = 0xFF;
    fat_sector[7] = 0x0F;
    // FAT[2] root cluster end-of-chain
    fat_sector[8] = 0xFF;
    fat_sector[9] = 0xFF;
    fat_sector[10] = 0xFF;
    fat_sector[11] = 0x0F;

    if (!card.writeSector(fat_start, fat_sector)) {
        return false;
    }

    // Mirror FATs
    for (uint8_t fat_index = 0; fat_index < options.fat_count; ++fat_index) {
        for (uint32_t i = 0; i < layout.fat_size_sectors; ++i) {
            if (fat_index == 0 && i == 0) {
                continue;  // already written
            }
            Sector blank{};
            blank.fill(0);
            if (!card.writeSector(fat_start + fat_index * layout.fat_size_sectors + i, blank)) {
                return false;
            }
        }
    }

    // Clear first data cluster to zeros
    const uint32_t data_start = first_lba + layout.first_data_sector;
    Sector zero{};
    zero.fill(0);
    if (!card.writeSector(data_start, zero)) {
        return false;
    }

    return card.flush();
}

}  // namespace cardfix
