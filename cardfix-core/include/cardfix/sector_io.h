#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace cardfix {

constexpr size_t kSectorSize = 512;

using Sector = std::array<uint8_t, kSectorSize>;

struct CardGeometry {
    constexpr CardGeometry() = default;
    constexpr CardGeometry(uint32_t sector_size, uint64_t sector_count)
        : sector_size(sector_size), sector_count(sector_count) {}
    uint32_t sector_size = kSectorSize;
    uint64_t sector_count = 0;
};

class RawCard {
public:
    virtual ~RawCard() = default;
    virtual bool readSector(uint32_t lba, Sector &out) = 0;
    virtual bool writeSector(uint32_t lba, const Sector &data) = 0;
    virtual uint64_t sectorCount() const = 0;
    virtual bool flush() { return true; }
};

class MemoryCard : public RawCard {
public:
    explicit MemoryCard(uint64_t sectors);
    bool readSector(uint32_t lba, Sector &out) override;
    bool writeSector(uint32_t lba, const Sector &data) override;
    uint64_t sectorCount() const override;
    std::vector<Sector> &storage() { return storage_; }

private:
    std::vector<Sector> storage_;
};

}  // namespace cardfix
