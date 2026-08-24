#include "pl_spi_base.h"
#include "esp_check.h"

//==============================================================================

static const char* TAG = "pl_spi";

//==============================================================================

namespace PL {

//==============================================================================

Spi::Spi(spi_host_device_t host, int mosiPin, int misoPin, int sclkPin) :
    host(host), mosiPin(mosiPin), misoPin(misoPin), sclkPin(sclkPin) { }

//==============================================================================

Spi::~Spi() {
  if (initialized)
    spi_bus_free(host);
}

//==============================================================================

esp_err_t Spi::Lock(TickType_t timeout) {
  esp_err_t error = mutex.Lock(timeout);
  if (error != ESP_OK && (error != ESP_ERR_TIMEOUT || timeout != 0))
    ESP_LOGE(TAG, "mutex lock failed");
  return error;
}

//==============================================================================

esp_err_t Spi::Unlock() {
  ESP_RETURN_ON_ERROR(mutex.Unlock(), TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t Spi::Initialize() {
  LockGuard lg(*this);
  if (initialized)
    return ESP_OK;
  spi_bus_config_t busConfig = {};
  busConfig.mosi_io_num = mosiPin;
  busConfig.miso_io_num = misoPin;
  busConfig.sclk_io_num = sclkPin;
  busConfig.quadwp_io_num = -1;
  busConfig.quadhd_io_num = -1;
  busConfig.max_transfer_sz = maxTransactionSize;
  spi_dma_chan_t dmaChannel = maxTransactionSize > SOC_SPI_MAXIMUM_BUFFER_SIZE ? SPI_DMA_CH_AUTO : SPI_DMA_DISABLED;
  ESP_RETURN_ON_ERROR(spi_bus_initialize(host, &busConfig, dmaChannel), TAG, "SPI bus initialize failed");
  initialized = true;
  return ESP_OK;
}

//==============================================================================

esp_err_t Spi::SetMaxTransactionSize(int maxTransactionSize) {
  LockGuard lg(*this);
  ESP_RETURN_ON_FALSE(!initialized, ESP_ERR_INVALID_STATE, TAG, "SPI interface is already initialized");
  ESP_RETURN_ON_FALSE(maxTransactionSize >= 0, ESP_ERR_INVALID_ARG, TAG, "invalid max transaction size");
  this->maxTransactionSize = maxTransactionSize;
  return ESP_OK;
}

//==============================================================================

}