#include "spi_base.h"
#include "unity.h"

//==============================================================================

const spi_host_device_t host = SPI2_HOST;
const int mosiPin = 5;
const int misoPin = 18;
const int sclkPin = 19;
const int maxTransactionSize = 64;

//==============================================================================

void TestSpi() {
  PL::Spi spi(host, mosiPin, misoPin, sclkPin);

  TEST_ASSERT(spi.SetMaxTransactionSize(maxTransactionSize) == ESP_OK);

  TEST_ASSERT(spi.Initialize() == ESP_OK);

  TEST_ASSERT(spi.SetMaxTransactionSize(maxTransactionSize) == ESP_ERR_INVALID_STATE);
}
