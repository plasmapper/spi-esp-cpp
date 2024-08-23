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
  if (error == ESP_OK)
    return ESP_OK;
  if (error == ESP_ERR_TIMEOUT && timeout == 0)
    return ESP_ERR_TIMEOUT;
  ESP_RETURN_ON_ERROR (error, TAG, "mutex lock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t Spi::Unlock() {
  ESP_RETURN_ON_ERROR (mutex.Unlock(), TAG, "mutex unlock failed");
  return ESP_OK;
}

//==============================================================================

esp_err_t Spi::SetMaxTransactionSize(int maxTransactionSize) {
  LockGuard lg(*this);
  ESP_RETURN_ON_FALSE (!initialized, ESP_ERR_INVALID_STATE, TAG, "SPI interface is already initialized");
  this->maxTransactionSize = maxTransactionSize;
  return ESP_OK;
}

//==============================================================================

}