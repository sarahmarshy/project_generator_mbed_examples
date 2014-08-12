/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, STMicroelectronics
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */
#include "mbed_assert.h"
#include "spi_api.h"

#if DEVICE_SPI

#include <math.h>
#include "cmsis.h"
#include "pinmap.h"

static const PinMap PinMap_SPI_MOSI[] = {
    {PA_1,  SPI_4, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI4)},
    {PA_7,  SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI1)}, // ARDUINO
    {PA_10, SPI_5, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI5)},
//  {PB_5,  SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI1)},
    {PB_5,  SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI3)},
    {PB_8,  SPI_5, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI5)},
    {PB_15, SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI2)},
    {PC_3,  SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI2)},
    {PC_12, SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI3)},
    {NC,    NC,    0}
};

static const PinMap PinMap_SPI_MISO[] = {
    {PA_6,  SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI1)}, // ARDUINO
    {PA_11, SPI_4, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI4)},
    {PA_12, SPI_5, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI5)},
//  {PB_4,  SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI1)},
    {PB_4,  SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI3)},
    {PB_14, SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI2)},
    {PC_2,  SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI2)},
    {PC_11, SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI3)},
    {NC,    NC,    0}
};

static const PinMap PinMap_SPI_SCLK[] = {
    {PA_5,  SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI1)}, // ARDUINO
    {PB_0,  SPI_5, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI5)},
//  {PB_3,  SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI1)},
    {PB_3,  SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI3)},
    {PB_10, SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI2)},
//  {PB_12, SPI_3, STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, GPIO_AF7_SPI3)}, // Warning: also on NSS
    {PB_13, SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI2)},
//  {PB_13, SPI_4, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI4)},
    {PC_7,  SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF5_SPI2)},
    {PC_10, SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI3)},
    {NC,    NC,    0}
};

static const PinMap PinMap_SPI_SSEL[] = {
    {PA_4,  SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF5_SPI1)},
//  {PA_4,  SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF6_SPI3)},
    {PA_15, SPI_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF5_SPI1)},
//  {PA_15, SPI_3, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF6_SPI3)},
    {PB_1,  SPI_5, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_SPI5)},
    {PB_9,  SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF5_SPI2)},
    {PB_12, SPI_2, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF5_SPI2)}, // Warning: also on SCLK
//  {PB_12, SPI_4, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_NOPULL, GPIO_AF6_SPI4)}, // Warning: also on SCLK
    {NC,    NC,    0}
};

static SPI_HandleTypeDef SpiHandle;

static void init_spi(spi_t *obj)
{
    SpiHandle.Instance = (SPI_TypeDef *)(obj->spi);

    __HAL_SPI_DISABLE(&SpiHandle);

    SpiHandle.Init.Mode              = obj->mode;
    SpiHandle.Init.BaudRatePrescaler = obj->br_presc;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = obj->cpha;
    SpiHandle.Init.CLKPolarity       = obj->cpol;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
    SpiHandle.Init.CRCPolynomial     = 7;
    SpiHandle.Init.DataSize          = obj->bits;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = obj->nss;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLED;

    HAL_SPI_Init(&SpiHandle);

    __HAL_SPI_ENABLE(&SpiHandle);
}

void spi_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel)
{
    // Determine the SPI to use
    SPIName spi_mosi = (SPIName)pinmap_peripheral(mosi, PinMap_SPI_MOSI);
    SPIName spi_miso = (SPIName)pinmap_peripheral(miso, PinMap_SPI_MISO);
    SPIName spi_sclk = (SPIName)pinmap_peripheral(sclk, PinMap_SPI_SCLK);
    SPIName spi_ssel = (SPIName)pinmap_peripheral(ssel, PinMap_SPI_SSEL);

    SPIName spi_data = (SPIName)pinmap_merge(spi_mosi, spi_miso);
    SPIName spi_cntl = (SPIName)pinmap_merge(spi_sclk, spi_ssel);

    obj->spi = (SPIName)pinmap_merge(spi_data, spi_cntl);
    MBED_ASSERT(obj->spi != (SPIName)NC);

    // Enable SPI clock
    if (obj->spi == SPI_1) {
        __SPI1_CLK_ENABLE();
    }
    if (obj->spi == SPI_2) {
        __SPI2_CLK_ENABLE();
    }
    if (obj->spi == SPI_3) {
        __SPI3_CLK_ENABLE();
    }
    if (obj->spi == SPI_4) {
        __SPI4_CLK_ENABLE();
    }
    if (obj->spi == SPI_5) {
        __SPI5_CLK_ENABLE();
    }

    // Configure the SPI pins
    pinmap_pinout(mosi, PinMap_SPI_MOSI);
    pinmap_pinout(miso, PinMap_SPI_MISO);
    pinmap_pinout(sclk, PinMap_SPI_SCLK);

    // Save new values
    obj->bits = SPI_DATASIZE_8BIT;
    obj->cpol = SPI_POLARITY_LOW;
    obj->cpha = SPI_PHASE_1EDGE;
    obj->br_presc = SPI_BAUDRATEPRESCALER_256;

    obj->pin_miso = miso;
    obj->pin_mosi = mosi;
    obj->pin_sclk = sclk;
    obj->pin_ssel = ssel;

    if (ssel == NC) { // SW NSS Master mode
        obj->mode = SPI_MODE_MASTER;
        obj->nss = SPI_NSS_SOFT;
    } else { // Slave
        pinmap_pinout(ssel, PinMap_SPI_SSEL);
        obj->mode = SPI_MODE_SLAVE;
        obj->nss = SPI_NSS_HARD_INPUT;
    }

    init_spi(obj);
}

void spi_free(spi_t *obj)
{
    // Reset SPI and disable clock
    if (obj->spi == SPI_1) {
        __SPI1_FORCE_RESET();
        __SPI1_RELEASE_RESET();
        __SPI1_CLK_DISABLE();
    }
    if (obj->spi == SPI_2) {
        __SPI2_FORCE_RESET();
        __SPI2_RELEASE_RESET();
        __SPI2_CLK_DISABLE();
    }
    if (obj->spi == SPI_3) {
        __SPI3_FORCE_RESET();
        __SPI3_RELEASE_RESET();
        __SPI3_CLK_DISABLE();
    }
    if (obj->spi == SPI_4) {
        __SPI4_FORCE_RESET();
        __SPI4_RELEASE_RESET();
        __SPI4_CLK_DISABLE();
    }
    if (obj->spi == SPI_5) {
        __SPI5_FORCE_RESET();
        __SPI5_RELEASE_RESET();
        __SPI5_CLK_DISABLE();
    }

    // Configure GPIOs
    pin_function(obj->pin_miso, STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));
    pin_function(obj->pin_mosi, STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));
    pin_function(obj->pin_sclk, STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));
    pin_function(obj->pin_ssel, STM_PIN_DATA(STM_MODE_INPUT, GPIO_NOPULL, 0));
}

void spi_format(spi_t *obj, int bits, int mode, int slave)
{
    // Save new values
    if (bits == 16) {
        obj->bits = SPI_DATASIZE_16BIT;
    } else {
        obj->bits = SPI_DATASIZE_8BIT;
    }

    switch (mode) {
        case 0:
            obj->cpol = SPI_POLARITY_LOW;
            obj->cpha = SPI_PHASE_1EDGE;
            break;
        case 1:
            obj->cpol = SPI_POLARITY_LOW;
            obj->cpha = SPI_PHASE_2EDGE;
            break;
        case 2:
            obj->cpol = SPI_POLARITY_HIGH;
            obj->cpha = SPI_PHASE_1EDGE;
            break;
        default:
            obj->cpol = SPI_POLARITY_HIGH;
            obj->cpha = SPI_PHASE_2EDGE;
            break;
    }

    if (slave == 0) {
        obj->mode = SPI_MODE_MASTER;
        obj->nss = SPI_NSS_SOFT;
    } else {
        obj->mode = SPI_MODE_SLAVE;
        obj->nss = SPI_NSS_HARD_INPUT;
    }

    init_spi(obj);
}

void spi_frequency(spi_t *obj, int hz)
{
    // Values depend of PCLK2: 100 MHz
    if ((obj->spi == SPI_1) || (obj->spi == SPI_4) || (obj->spi == SPI_5)) {
        if (hz < 700000) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_256; // 391 kHz
        } else if ((hz >= 700000) && (hz < 1000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_128; // 781 kHz
        } else if ((hz >= 1000000) && (hz < 3000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_64;  // 1.56 MHz
        } else if ((hz >= 3000000) && (hz < 6000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_32;  // 3.13 MHz
        } else if ((hz >= 6000000) && (hz < 12000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_16;  // 6.25 MHz
        } else if ((hz >= 12000000) && (hz < 25000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_8;   // 12.5 MHz
        } else if ((hz >= 25000000) && (hz < 50000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_4;   // 25 MHz
        } else { // >= 50000000
            obj->br_presc = SPI_BAUDRATEPRESCALER_2;   // 50 MHz
        }
    }

    // Values depend of PCLK1: 50 MHz
    if ((obj->spi == SPI_2) || (obj->spi == SPI_3)) {
        if (hz < 400000) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_256; // 195 kHz
        } else if ((hz >= 400000) && (hz < 700000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_128; // 391 kHz
        } else if ((hz >= 700000) && (hz < 1000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_64;  // 781 MHz
        } else if ((hz >= 1000000) && (hz < 3000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_32;  // 1.56 MHz
        } else if ((hz >= 3000000) && (hz < 6000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_16;  // 3.13 MHz
        } else if ((hz >= 6000000) && (hz < 12000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_8;   // 6.25 MHz
        } else if ((hz >= 12000000) && (hz < 25000000)) {
            obj->br_presc = SPI_BAUDRATEPRESCALER_4;   // 12.5 MHz
        } else { // >= 25000000
            obj->br_presc = SPI_BAUDRATEPRESCALER_2;   // 25 MHz
        }
    }

    init_spi(obj);
}

static inline int ssp_readable(spi_t *obj)
{
    int status;
    SpiHandle.Instance = (SPI_TypeDef *)(obj->spi);
    // Check if data is received
    status = ((__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_RXNE) != RESET) ? 1 : 0);
    return status;
}

static inline int ssp_writeable(spi_t *obj)
{
    int status;
    SpiHandle.Instance = (SPI_TypeDef *)(obj->spi);
    // Check if data is transmitted
    status = ((__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_TXE) != RESET) ? 1 : 0);
    return status;
}

static inline void ssp_write(spi_t *obj, int value)
{
    SPI_TypeDef *spi = (SPI_TypeDef *)(obj->spi);
    while (!ssp_writeable(obj));
    spi->DR = (uint16_t)value;
}

static inline int ssp_read(spi_t *obj)
{
    SPI_TypeDef *spi = (SPI_TypeDef *)(obj->spi);
    while (!ssp_readable(obj));
    return (int)spi->DR;
}

static inline int ssp_busy(spi_t *obj)
{
    int status;
    SpiHandle.Instance = (SPI_TypeDef *)(obj->spi);
    status = ((__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_BSY) != RESET) ? 1 : 0);
    return status;
}

int spi_master_write(spi_t *obj, int value)
{
    ssp_write(obj, value);
    return ssp_read(obj);
}

int spi_slave_receive(spi_t *obj)
{
    return ((ssp_readable(obj) && !ssp_busy(obj)) ? 1 : 0);
};

int spi_slave_read(spi_t *obj)
{
    SPI_TypeDef *spi = (SPI_TypeDef *)(obj->spi);
    while (!ssp_readable(obj));
    return (int)spi->DR;
}

void spi_slave_write(spi_t *obj, int value)
{
    SPI_TypeDef *spi = (SPI_TypeDef *)(obj->spi);
    while (!ssp_writeable(obj));
    spi->DR = (uint16_t)value;
}

int spi_busy(spi_t *obj)
{
    return ssp_busy(obj);
}

#endif
