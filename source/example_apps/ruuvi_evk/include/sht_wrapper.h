/**
    @file   sht_wrapper.h
    @brief  Wrapper on top of LIS2DH12 driver from STMicroelectronics GitHub.

    This file is the wrapper to expose a simple API for getting the acceleration
    from lis2dh12 device. It makes the glue between the ST driver and Wirepas
    SPI driver and staticaly configure the sensor.

    @copyright  Wirepas Oy 2019
*/
#ifndef SHT_WRAPPER_H_
#define SHT_WRAPPER_H_

#include <stdio.h>
#include <stdbool.h>

/**
    @brief Structure containing sensor measurements.
*/
typedef struct
{
    int32_t  temperature;    /**< Temperature in 0.01°C. */
    int32_t humidity;       /**< Humidity in 1/1024 % relative humidity (1% is 1024). */
} sht_wrapper_measurement_t;

typedef enum
{
    E_SHT_READY = 0,
    E_SHT_BUSY,
    E_SHT_ERROR
} E_sht_wrapper_state_t;

/**
    @brief  Initialize the LIS2DH12 wrapper library.
    @return True if successfully initialized, false otherwise.
*/
bool SHT_wrapper_init(void);

/**
    @brief  Initialize the LIS2DH12 wrapper library.
    @return True if successfully initialized, false otherwise.
*/
E_sht_wrapper_state_t SHT_wrapper_readyState(void);

/**
    @brief  Start a measurement.
    @return The time in ms to wait before the measurement is ready.
*/
uint32_t SHT_wrapper_startMeasurement(void);

/**
    @brief     Get a measurement previously asked by a start measurement.
    @param[in] measurement The measurement read out from the sensor.
    @return    false if a problem occured, true otherwise.
*/
bool SHT_wrapper_readMeasurement(sht_wrapper_measurement_t * pMeasurement);

#endif /* SHT_WRAPPER_H_ */
