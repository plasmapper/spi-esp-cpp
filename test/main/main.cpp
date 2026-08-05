#include "unity.h"
#include "spi_base.h"
#include "spi_controller.h"

//==============================================================================

extern "C" void app_main(void) {
  UNITY_BEGIN();
  RUN_TEST(TestSpi);
  RUN_TEST(TestSpiController);
  UNITY_END();
}
