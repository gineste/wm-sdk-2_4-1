/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sht_wrapper.h"
#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "i2c.h"
#include "api.h"
#include <string.h>
#include "shtc1.h"

/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 */

/* Time to get first measure from POWER DOWN @1Hz, with 50% margin. */
#define SHT_MEAS_TIME_MS 1// Inf. < 1ms

/* Time to get one measurement with 50% margin. */
#define SHTC_I2C_ADDRESS            0x70

/** Maximum I2C write transfer */
#define SHTC_MAX_WRITE_SIZE             16

/** Maximum I2C read transfer */
#define SHTC_MAX_READ_SIZE              16

static uint8_t m_i2c_address = SHTC_I2C_ADDRESS;


/**
 * Select the current i2c bus by index.
 * All following i2c operations will be directed at that bus.
 *
 * THE IMPLEMENTATION IS OPTIONAL ON SINGLE-BUS SETUPS (all sensors on the same
 * bus)
 *
 * @param bus_idx   Bus index to select
 * @returns         0 on success, an error code otherwise
 */
int16_t sensirion_i2c_select_bus(uint8_t bus_idx) {
    // IMPLEMENT or leave empty if all sensors are located on one single bus
    return STATUS_FAIL;
}

/**
 * Initialize all hard- and software components that are needed for the I2C
 * communication.
 */
void sensirion_i2c_init(void) {
    // Configure device in Low Power mode
    shtc1_enable_low_power_mode(true);
}

/**
 * Release all resources initialized by sensirion_i2c_init().
 */
void sensirion_i2c_release(void) {
    // IMPLEMENT or leave empty if no resources need to be freed
}

/**
 * Execute one read transaction on the I2C bus, reading a given number of bytes.
 * If the device does not acknowledge the read command, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to read from
 * @param data    pointer to the buffer where the data is to be stored
 * @param count   number of bytes to read from I2C and store in the buffer
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_read(uint8_t address, uint8_t* data, uint16_t count) {
    i2c_res_e res;
    uint8_t rx[SHTC_MAX_READ_SIZE + 1];

    i2c_xfer_t xfer_rx = {
            .address = m_i2c_address,
            .write_ptr = NULL,
            .write_size = 0,
            .read_ptr = rx,
            .read_size = (uint16_t) count,
            .custom = 0
    };

    res = I2C_transfer(&xfer_rx, NULL);
    memcpy(data, &rx[0], count * sizeof(uint8_t));

    return res;
}

/**
 * Execute one write transaction on the I2C bus, sending a given number of
 * bytes. The bytes in the supplied buffer must be sent to the given address. If
 * the slave device does not acknowledge any of the bytes, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to write to
 * @param data    pointer to the buffer containing the data to write
 * @param count   number of bytes to read from the buffer and send over I2C
 * @returns 0 on success, error code otherwise
 */
int8_t sensirion_i2c_write(uint8_t address, const uint8_t* data, uint16_t count) {
    i2c_res_e res;
    uint8_t tx[SHTC_MAX_WRITE_SIZE + 1];

    memcpy(&tx[0], data, count * sizeof(uint8_t));

    i2c_xfer_t xfer_tx = {
            .address = m_i2c_address,
            .write_ptr = tx,
            .write_size = (uint16_t) count,
            .read_ptr = NULL,
            .read_size = 0,
            .custom = 0
    };

    res = I2C_transfer(&xfer_tx, NULL);
    return res;
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * Despite the unit, a <10 millisecond precision is sufficient.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_sleep_usec(uint32_t useconds) {
    // IMPLEMENT
    app_lib_time_timestamp_hp_t end;
    end = lib_time->addUsToHpTimestamp(lib_time->getTimestampHp(),
                                       useconds);

    /* Active wait until period is elapsed */
    while (lib_time->isHpTimestampBefore(lib_time->getTimestampHp(),
                                         end));
}

bool SHT_wrapper_init(void)
{
    sensirion_i2c_init();

    // Blocking i2c startup here ?

    return true;
}

E_sht_wrapper_state_t SHT_wrapper_readyState(void)
{
    static const uint16_t SHTC1_SERIAL_COMMON = 0x0807;
    uint8_t res = 0;
    uint32_t serial = 0u;
    E_sht_wrapper_state_t ret = E_SHT_ERROR;

    shtc1_wake_up();
    res = shtc1_read_serial(&serial);

    // Check serial is valid
    if ((serial & SHTC1_SERIAL_COMMON) == SHTC1_SERIAL_COMMON)
    {
        // Check communication was successfull
        ret = (res == 0) ? E_SHT_READY : E_SHT_BUSY;
    }
    else // Serial not valid or communication error
    {
        ret = E_SHT_ERROR;
    }

    return ret;
}

uint32_t SHT_wrapper_startMeasurement(void)
{
    // Tempo is needed after wake up.
    // Tempo <= 100ms triggers a NACK by sensor.
    shtc1_wake_up();
    sensirion_sleep_usec(200);

    shtc1_measure();

    // < 1ms in low power mode
    // 250us in low power & 11ms in normal mode)
    return SHT_MEAS_TIME_MS;
}

bool SHT_wrapper_readMeasurement(sht_wrapper_measurement_t * pMeasurement)
{
    shtc1_read(&pMeasurement->temperature, &pMeasurement->humidity);
    shtc1_sleep();
    return true;
}
