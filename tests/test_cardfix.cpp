#include <unity.h>

#include "cardfix/diagnostics.h"
#include "cardfix/format.h"
#include "cardfix/sector_io.h"

using namespace cardfix;

void test_layout_computation() {
    CardGeometry geom{ kSectorSize, 1024 * 8 }; // 8k sectors
    FormatOptions opts{};
    auto layout = Formatter::computeLayout(geom, opts);
    TEST_ASSERT_GREATER_THAN(0u, layout.fat_size_sectors);
    TEST_ASSERT_GREATER_THAN(0u, layout.cluster_count);
    TEST_ASSERT_TRUE(layout.first_data_sector > opts.reserved_sectors);
}

void test_formatting_writes_signatures() {
    MemoryCard card(4096);
    CardGeometry geom{ kSectorSize, card.sectorCount() };
    FormatOptions opts{};
    TEST_ASSERT_TRUE(Formatter::writeMbRandFat32(card, geom, opts));
    Sector mbr{};
    Sector vbr{};
    TEST_ASSERT_TRUE(card.readSector(0, mbr));
    TEST_ASSERT_TRUE(card.readSector(1, vbr));
    TEST_ASSERT_EQUAL_HEX8(0x55, mbr[510]);
    TEST_ASSERT_EQUAL_HEX8(0xAA, mbr[511]);
    TEST_ASSERT_EQUAL_HEX8(0x55, vbr[510]);
    TEST_ASSERT_EQUAL_HEX8(0xAA, vbr[511]);
}

void test_diagnostics_pattern() {
    MemoryCard card(2048);
    CardGeometry geom{ kSectorSize, card.sectorCount() };
    auto report = Diagnostics::quickTest(card, geom);
    TEST_ASSERT_TRUE(report.card_present);
    TEST_ASSERT_TRUE(report.geometry_valid);
    TEST_ASSERT_TRUE(report.pattern_success);
    TEST_ASSERT_GREATER_THAN(0u, report.layout.cluster_count);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_layout_computation);
    RUN_TEST(test_formatting_writes_signatures);
    RUN_TEST(test_diagnostics_pattern);
    return UNITY_END();
}
