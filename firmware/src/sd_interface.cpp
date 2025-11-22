#include "sd_interface.h"

#include <Arduino.h>

SdCardInterface::SdCardInterface() = default;

bool SdCardInterface::begin() {
    host_ = SDMMC_HOST_DEFAULT();
    host_.flags = SDMMC_HOST_FLAG_1BIT;
    host_.max_freq_khz = SDMMC_FREQ_DEFAULT;

    slot_config_ = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config_.width = 1;
    slot_config_.cd = SDMMC_SLOT_NO_CD;
    slot_config_.wp = SDMMC_SLOT_NO_WP;

    if (sdmmc_host_init() != ESP_OK) {
        return false;
    }
    if (sdmmc_host_init_slot(host_.slot, &slot_config_) != ESP_OK) {
        return false;
    }
    if (sdmmc_card_init(&host_, &card_) != ESP_OK) {
        return false;
    }

    ready_ = true;
    return true;
}

bool SdCardInterface::readSector(uint32_t lba, cardfix::Sector &out) {
    if (!ready_) {
        return false;
    }
    auto err = sdmmc_read_sectors(&card_, out.data(), lba, 1);
    return err == ESP_OK;
}

bool SdCardInterface::writeSector(uint32_t lba, const cardfix::Sector &data) {
    if (!ready_) {
        return false;
    }
    auto err = sdmmc_write_sectors(&card_, data.data(), lba, 1);
    return err == ESP_OK;
}

uint64_t SdCardInterface::sectorCount() const {
    if (!ready_) {
        return 0;
    }
    return card_.csd.capacity;  // in sectors for SDHC
}

bool SdCardInterface::flush() {
    if (!ready_) {
        return false;
    }
    // SDMMC driver performs synchronous transfers; nothing buffered.
    return true;
}

