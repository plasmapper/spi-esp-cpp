#include "spi_controller.h"
#include "unity.h"

//==============================================================================

const spi_host_device_t host = SPI2_HOST;
const int mosiPin = 5;
const int misoPin = 18;
const int sclkPin = 19;
const uint8_t spiMode = 0;
const int sclkFrequency = 10000000;
const int csPin = 21;
const int numberOfCommandBits = 8;
const int numberOfAddressBits = 8;
const int maxSclkMisoDelay = 30;
uint16_t command = 10;
uint64_t address = 20;
const uint8_t dataToSend[] = {1, 2, 3, 4, 5};

//==============================================================================

void TestSpiController() {
  auto spi = std::make_shared<PL::Spi>(host, mosiPin, misoPin, sclkPin);
  PL::SpiController controller(spi, spiMode, sclkFrequency, csPin);

  TEST_ASSERT(controller.SetNumberOfCommandBits(-1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfCommandBits(PL::SpiController::maxNumberOfCommandBits + 1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfCommandBits(numberOfCommandBits) == ESP_OK);

  TEST_ASSERT(controller.SetNumberOfAddressBits(-1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfAddressBits(PL::SpiController::maxNumberOfAddressBits + 1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfAddressBits(numberOfAddressBits) == ESP_OK);

  TEST_ASSERT(controller.SetMaxSclkMisoDelay(maxSclkMisoDelay) == ESP_OK);

  TEST_ASSERT(controller.Initialize() == ESP_OK);

  TEST_ASSERT(controller.SetNumberOfCommandBits(numberOfCommandBits) == ESP_ERR_INVALID_STATE);
  TEST_ASSERT(controller.SetNumberOfAddressBits(numberOfAddressBits) == ESP_ERR_INVALID_STATE);

  // MOSI and MISO must be connected with a wire for this to succeed.
  uint8_t receivedData[sizeof(dataToSend)] = {};
  TEST_ASSERT(controller.Transaction(command, address, dataToSend, receivedData, sizeof(dataToSend) * 8) == ESP_OK);
  for (size_t i = 0; i < sizeof(dataToSend); i++)
    TEST_ASSERT_EQUAL_MESSAGE(dataToSend[i], receivedData[i], "Make sure that MOSI (5) and MISO (18) pins are connected with a wire.");
}
