#pragma once

#include <SD_MMC.h>
#include <sdmmc_cmd.h>
#include "cardfix/sector_io.h"

class SdCardInterface : public cardfix::RawCard {
public:
    SdCardInterface();
    bool begin();
    bool readSector(uint32_t lba, cardfix::Sector &out) override;
    bool writeSector(uint32_t lba, const cardfix::Sector &data) override;
    uint64_t sectorCount() const override;
    bool flush() override;

private:
    sdmmc_host_t host_{};
    sdmmc_slot_config_t slot_config_{};
    sdmmc_card_t card_{};
    bool ready_ = false;
};

