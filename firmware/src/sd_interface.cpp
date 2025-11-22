#include "sd_interface.h"

#include <Arduino.h>

SdCardInterface::SdCardInterface() = default;

bool SdCardInterface::begin() {
    if (!SD_MMC.begin("/sdcard", true)) {
        return false;
    }
    card_ = SD_MMC.card();
    return card_ != nullptr;
}

bool SdCardInterface::readSector(uint32_t lba, cardfix::Sector &out) {
    if (!card_) {
        return false;
    }
    auto err = sdmmc_read_sectors(card_, out.data(), lba, 1);
    return err == ESP_OK;
}

bool SdCardInterface::writeSector(uint32_t lba, const cardfix::Sector &data) {
    if (!card_) {
        return false;
    }
    auto err = sdmmc_write_sectors(card_, data.data(), lba, 1);
    return err == ESP_OK;
}

uint64_t SdCardInterface::sectorCount() const {
    if (!card_) {
        return 0;
    }
    return card_->csd.capacity;  // in sectors for SDHC
}

bool SdCardInterface::flush() {
    if (!card_) {
        return false;
    }
    // SDMMC driver is synchronous; ensure cache is flushed through SD_MMC
    SD_MMC.flush();
    return true;
}

