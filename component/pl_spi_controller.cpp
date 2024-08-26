#include "pl_spi_controller.h"
#include "esp_check.h"

//==============================================================================

static const char* TAG = "pl_spi_controller";

//==============================================================================

namespace PL {

//==============================================================================

SpiController::SpiController(std::shared_ptr<Spi> spi, uint8_t mode, int sclkFrequency, int csPin) :
    spi(spi), mode(mode), sclkFrequency(sclkFrequency), csPin(csPin) { }

//==============================================================================

SpiController::~SpiController() {
  if (deviceHandle)
    spi_bus_remove_device(deviceHandle);
}

//==============================================================================

esp_err_t SpiController::Lock(TickType_t timeout) {
  esp_err_t error = mutex.Lock(timeout);
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR(error, TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t SpiController::Unlock() {
  ESP_RETURN_ON_ERROR(mutex.Unlock(), TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t SpiController::Initialize() {
  spi_host_device_t host;
  {
    LockGuard lg(*spi);
    host = spi->host;
    if (!spi->initialized) {
      spi_bus_config_t busConfig = {};
      busConfig.mosi_io_num = spi->mosiPin;
      busConfig.miso_io_num = spi->misoPin;
      busConfig.sclk_io_num = spi->sclkPin;
      busConfig.quadwp_io_num = -1;
      busConfig.quadhd_io_num = -1;
      busConfig.max_transfer_sz = spi->maxTransactionSize;
      ESP_RETURN_ON_ERROR(spi_bus_initialize(spi->host, &busConfig, SPI_DMA_DISABLED), TAG, "initialize SPI controller bus failed");
    }
  }

  {
    LockGuard lg(*this);
    if (!deviceHandle) {
      spi_device_interface_config_t deviceConfig = {};
      deviceConfig.command_bits = numberOfCommandBits;
      deviceConfig.address_bits = numberOfAddressBits;
      deviceConfig.mode = mode;
      deviceConfig.clock_speed_hz = sclkFrequency;
      deviceConfig.spics_io_num = csPin;
      deviceConfig.queue_size = 1;
      deviceConfig.input_delay_ns = maxSclkMisoDelay;
      ESP_RETURN_ON_ERROR(spi_bus_add_device(host, &deviceConfig, &deviceHandle), TAG, "initialize SPI controller device failed");
    }
  }

  return ESP_OK;
}

//==============================================================================

esp_err_t SpiController::Transaction(uint16_t command, uint64_t address, const void* writeData, void* readData, size_t dataSize) {
  spi_device_handle_t deviceHandle;
  spi_transaction_t transaction = {};
  {
    LockGuard lg(*this);
    deviceHandle = this->deviceHandle;
    transaction.cmd = command;
    transaction.addr = address;
    transaction.length = dataSize;
    transaction.tx_buffer = writeData;
    transaction.rx_buffer = readData;
    ESP_RETURN_ON_FALSE(deviceHandle, ESP_ERR_INVALID_STATE, TAG, "SPI controller is not initialized");
  }
  
  {
    LockGuard lg(*spi);
    ESP_RETURN_ON_ERROR(spi_device_polling_transmit(deviceHandle, &transaction), TAG, "SPI transaction failed");
    return ESP_OK;
  }
}

//==============================================================================

esp_err_t SpiController::SetNumberOfCommandBits(int numberOfCommandBits) {
  LockGuard lg(*this);
  ESP_RETURN_ON_FALSE(!deviceHandle, ESP_ERR_INVALID_STATE, TAG, "SPI controller is already initialized");
  this->numberOfCommandBits = numberOfCommandBits;
  return ESP_OK;
}

//==============================================================================

esp_err_t SpiController::SetNumberOfAddressBits(int numberOfAddressBits) {
  LockGuard lg(*this);
  ESP_RETURN_ON_FALSE(!deviceHandle, ESP_ERR_INVALID_STATE, TAG, "SPI controller is already initialized");
  this->numberOfAddressBits = numberOfAddressBits;
  return ESP_OK;
}

//==============================================================================

esp_err_t SpiController::SetMaxSclkMisoDelay(int delay) {
  LockGuard lg(*this);
  ESP_RETURN_ON_FALSE(!deviceHandle, ESP_ERR_INVALID_STATE, TAG, "SPI controller is already initialized");
  this->maxSclkMisoDelay = delay;
  return ESP_OK;
}

//==============================================================================

}