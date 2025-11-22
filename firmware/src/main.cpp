#include <Arduino.h>
#include <M5Unified.h>

#include "cardfix/diagnostics.h"
#include "cardfix/format.h"
#include "cardfix/sector_io.h"
#include "sd_interface.h"
#include "ui.h"

using namespace cardfix;

RepairUi ui;
SdCardInterface hwCard;

void runRepair() {
    ui.status("Mounting SD...", "Read-only mode");
    if (!hwCard.begin()) {
        ui.status("SD init failed", "Check card");
        return;
    }

    CardGeometry geometry{ kSectorSize, hwCard.sectorCount() };
    auto report = Diagnostics::quickTest(hwCard, geometry);
    ui.status("Diag done", report.message);

    delay(1500);
    ui.status("Formatting", "Destructive!" );
    FormatOptions opts{};
    if (!Formatter::writeMbRandFat32(hwCard, geometry, opts)) {
        ui.status("Format failed", "IO error");
        return;
    }

    ui.status("Complete", "Remove card safely");
}

void setup() {
    ui.begin();
    ui.log("Cardputer SD repair starting");
    runRepair();
}

void loop() {
    M5.update();
    delay(1000);
}

