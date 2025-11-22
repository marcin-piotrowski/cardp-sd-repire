#include "cardfix/sector_io.h"

#include <algorithm>
#include <cstring>

namespace cardfix {

MemoryCard::MemoryCard(uint64_t sectors) : storage_(sectors) {
    for (auto &sector : storage_) {
        sector.fill(0);
    }
}

bool MemoryCard::readSector(uint32_t lba, Sector &out) {
    if (lba >= storage_.size()) {
        return false;
    }
    out = storage_[lba];
    return true;
}

bool MemoryCard::writeSector(uint32_t lba, const Sector &data) {
    if (lba >= storage_.size()) {
        return false;
    }
    storage_[lba] = data;
    return true;
}

uint64_t MemoryCard::sectorCount() const {
    return storage_.size();
}

}  // namespace cardfix
