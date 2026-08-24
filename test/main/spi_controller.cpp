#include "spi_controller.h"
#include "unity.h"
#include "esp_random.h"

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
const int smallTransactionSize = SOC_SPI_MAXIMUM_BUFFER_SIZE;
DMA_ATTR uint8_t smallTransactionDataToSend[smallTransactionSize];
DMA_ATTR uint8_t smallTransactionReceivedData[smallTransactionSize];
const int largeTransactionSize = SOC_SPI_MAXIMUM_BUFFER_SIZE * 2;
DMA_ATTR uint8_t largeTransactionDataToSend[largeTransactionSize];
DMA_ATTR uint8_t largeTransactionReceivedData[largeTransactionSize];

//==============================================================================

void TestSpiController(size_t transactionSize, uint8_t* dataToSend, uint8_t* receivedData) {
  auto spi = std::make_shared<PL::Spi>(host, mosiPin, misoPin, sclkPin);
  TEST_ASSERT(spi->SetMaxTransactionSize(transactionSize) == ESP_OK);
  PL::SpiController controller(spi, spiMode, sclkFrequency, csPin);

  TEST_ASSERT(controller.SetNumberOfCommandBits(-1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfCommandBits(PL::SpiController::maxNumberOfCommandBits + 1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfCommandBits(numberOfCommandBits) == ESP_OK);

  TEST_ASSERT(controller.SetNumberOfAddressBits(-1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfAddressBits(PL::SpiController::maxNumberOfAddressBits + 1) == ESP_ERR_INVALID_ARG);
  TEST_ASSERT(controller.SetNumberOfAddressBits(numberOfAddressBits) == ESP_OK);

  TEST_ASSERT(controller.SetMaxSclkMisoDelay(maxSclkMisoDelay) == ESP_OK);

  TEST_ASSERT(controller.Initialize() == ESP_OK);

  TEST_ASSERT(spi->SetMaxTransactionSize(transactionSize) == ESP_ERR_INVALID_STATE);
  TEST_ASSERT(controller.SetNumberOfCommandBits(numberOfCommandBits) == ESP_ERR_INVALID_STATE);
  TEST_ASSERT(controller.SetNumberOfAddressBits(numberOfAddressBits) == ESP_ERR_INVALID_STATE);

  // MOSI and MISO must be connected with a wire for this to succeed
  esp_fill_random(dataToSend, transactionSize);
  TEST_ASSERT(controller.Transaction(command, address, dataToSend, receivedData, transactionSize * 8) == ESP_OK);
  for (size_t i = 0; i < transactionSize; i++)
    TEST_ASSERT_EQUAL_MESSAGE(dataToSend[i], receivedData[i], "Make sure that MOSI (5) and MISO (18) pins are connected with a wire.");
}

//==============================================================================

void TestSpiControllerSmallTransaction() {
  TestSpiController(smallTransactionSize, smallTransactionDataToSend, smallTransactionReceivedData);
}

//==============================================================================

void TestSpiControllerLargeTransaction() {
  TestSpiController(largeTransactionSize, largeTransactionDataToSend, largeTransactionReceivedData);
}