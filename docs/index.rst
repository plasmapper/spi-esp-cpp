SPI Component
=============

.. |COMPONENT| replace:: spi

.. |ESP_IDF_VERSION| replace:: 5.3
   
.. |VERSION| replace:: 1.0.1

.. include:: ../../../installation.rst

.. include:: ../../../sdkconfig_common.rst

Features
--------

1. :cpp:class:`PL::Spi` - an SPI interface class that is used for several SPI controllers that use the same interface.
   A number of Set methods can be used to set the SPI interface parameters before its initialization.
2. :cpp:class:`PL::SpiController` - an SPI controller class that is used for communication with a single SPI target.
   :cpp:func:`PL::SpiController::Initialize` initializes the SPI interface (if it has not been previously initialized) and the SPI controller.
   A number of Set methods can be used to set the SPI controller parameters before its initialization.
   :cpp:func:`PL::SpiController::Transaction` executes an SPI transaction.

SPI target and DMA transactions are not implemented yet.

Thread safety
-------------

Class method thread safety is implemented by having the :cpp:class:`PL::Lockable` as a base class and creating the class object lock guard at the beginning of the methods.

:cpp:func:`PL::SpiController::Initialize` and ::cpp:func:`PL::SpiController::Transaction` lock both
the :cpp:class:`PL::SpiController` and the :cpp:class:`PL::Spi` objects for the duration of the method. 

Examples
--------
| `SPI controller <https://components.espressif.com/components/plasmapper/pl_spi/versions/1.0.1/examples/spi_controller>`_

API reference
-------------

.. toctree::
  
  api/spi
  api/spi_controller