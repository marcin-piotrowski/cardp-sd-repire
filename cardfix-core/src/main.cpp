// Entry point for PlatformIO native builds when not running unit tests.
// PlatformIO provides its own main during `pio test`, so this file is
// excluded in that scenario.

#ifndef PLATFORMIO_UNIT_TEST
int main() {
    return 0;
}
#endif