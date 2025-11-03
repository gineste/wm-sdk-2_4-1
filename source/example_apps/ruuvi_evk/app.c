/**
    @file  app.c
    @brief Reference application of the Ruuvi evaluation kit.
    @copyright  Wirepas Oy 2019
*/
#include <stdio.h>
#include <string.h>

#include "api.h"
#include "node_configuration.h"
#include "app_scheduler.h"
#include "app_config.h"
#include "board.h"

#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "power.h"
#include "led.h"

#include "lis2dh12_wrapper.h"
#include "sht_wrapper.h"

#include "format_data.h"
#include "tlv.h"

#define DEBUG_LOG_MODULE_NAME "BLE_SCANNER_LIB"
#define DEBUG_LOG_MAX_LEVEL LVL_DEBUG
#include "debug_log.h"

/* Inline local declaration */
static inline void led_green(bool on)       { Led_set(1,on); /* Switch LED to state. */ }
static inline void led_red(bool on)         { Led_set(0,on); /* Switch LED to state. */ }

static inline void toggle_green()           {  static bool st = false;
                                                Led_set(1,st); /* Switch LED to state. */
                                                st=!st; }
static inline void toggle_red()             {  static bool st = true;
                                                Led_set(0,st); /* Switch LED to state. */
                                                st=!st;  }


/* Endpoints on which sensor data is sent. */
#define SENSOR_DATA_DST_ENDPOINT 11
#define SENSOR_DATA_SRC_ENDPOINT 11

/* State machine to manage sensors measurement start and read data.*/
typedef enum
{
    SENSOR_TASK_STATE_START_MEAS, /**< Start the sensor measurements. */
    SENSOR_TASK_STATE_SEND_DATA   /**< Read the measurement and send them. */
} sensor_task_state_e;

/* State of the application.*/
sensor_task_state_e m_task_state;
/* Read sensors data. */
sensor_data_t m_sensor_data;

/**
    @brief     initialize SPI driver and sensors chip select pins.
*/
static bool ruuvi_spi_init(void)
{
    spi_res_e res;
    spi_conf_t conf;
    gpio_out_cfg_t gpio_conf = {
        .out_mode_cfg = GPIO_OUT_MODE_PUSH_PULL,
        .level_default = GPIO_LEVEL_HIGH
    };

    /* Initialise LIS2DH12 Chip select pin. */
    Gpio_outputSetCfg(BOARD_GPIO_ID_LIS2DX12_SPI_CS, &gpio_conf);

    /* Initialise SPI driver. */
    conf.bit_order = SPI_ORDER_MSB;
    conf.clock = 4000000;
    conf.mode = SPI_MODE_HIGH_FIRST;
    res = SPI_init(&conf);
    if ((res != SPI_RES_OK) && (res != SPI_RES_ALREADY_INITIALIZED))
    {
        return false;
    }

    return true;
}


/**
    @brief     initialize SPI driver and sensors chip select pins.
*/
static bool ruuvi_i2c_init(void)
{
    i2c_res_e res;
    i2c_conf_t conf;
    conf.clock = 400000;    // 400kHz
    conf.pullup = true;     // pull

    // Configure I2C com (SDA/SCL pins & frequency)
    res = I2C_init(&conf);
    if ((res != I2C_RES_OK) && (res != I2C_RES_ALREADY_INITIALIZED))
    {
        return false;
    }

    return true;
}


/**
    @brief     Sends the sensors data.
    @param[in] data Pointer to the structure containing sensor data to send.
*/
static void send_data(sensor_data_t *data)
{
    app_lib_state_route_info_t route_info;
    uint8_t buff[102];
    int len = 102;

    /* For now the only supported format is TLV */
    len = format_data_tlv(buff, data, len);

    /* Only send data if there is a route to the Sink. */
    app_res_e res = lib_state->getRouteInfo(&route_info);
    if (res == APP_RES_OK && route_info.state == APP_LIB_STATE_ROUTE_STATE_VALID
        && len != -1)
    {
        app_lib_data_to_send_t data_to_send;
        data_to_send.bytes = (const uint8_t *) buff;
        data_to_send.num_bytes = len;
        data_to_send.dest_address = APP_ADDR_ANYSINK;
        data_to_send.src_endpoint = SENSOR_DATA_SRC_ENDPOINT;
        data_to_send.dest_endpoint = SENSOR_DATA_DST_ENDPOINT;
        data_to_send.qos = APP_LIB_DATA_QOS_HIGH;
        data_to_send.flags = APP_LIB_DATA_SEND_FLAG_NONE;
        data_to_send.tracking_id = APP_LIB_DATA_NO_TRACKING_ID;

        /* Send the data packet. */
        lib_data->sendData(&data_to_send);
    }
}

/**
    @brief     Sensor task. Manage the sensors and sends the measurement to
               the Sink.
*/
static uint32_t sensor_task()
{
    const app_config_t * cfg = App_Config_get();
    uint32_t time_to_run;

    switch (m_task_state)
    {
        case SENSOR_TASK_STATE_START_MEAS:
        {
            time_to_run = APP_SCHEDULER_SCHEDULE_ASAP;
            m_task_state = SENSOR_TASK_STATE_SEND_DATA;

            toggle_green();
            toggle_red();

            if(cfg->temperature_enable ||
               cfg->humidity_enable )
            {
                time_to_run = SHT_wrapper_startMeasurement();
            }

            if(cfg->pressure_enable )
            {
                // TO BE DONE
            }

            if(cfg->accel_x_enable ||
               cfg->accel_y_enable ||
               cfg->accel_z_enable )
            {
                uint32_t lis2dh12_time_to_run;
                lis2dh12_time_to_run = LIS2DH12_wrapper_startMeasurement();

                if(lis2dh12_time_to_run > time_to_run)
                {
                    time_to_run = lis2dh12_time_to_run;
                }
            }
            break;
        }
        case SENSOR_TASK_STATE_SEND_DATA:
        {
            /* Counter is always enabled. Increment it each period. */
            m_sensor_data.count++;

            if (cfg->temperature_enable ||
                cfg->humidity_enable )
            {
                sht_wrapper_measurement_t measurement = { 0 };
                SHT_wrapper_readMeasurement(&measurement);

                m_sensor_data.temp = (int32_t) measurement.temperature;
                m_sensor_data.humi = (int32_t) measurement.humidity;
            }

            if(cfg->pressure_enable )
            {
                // TO BE DONE
            }

            if(cfg->accel_x_enable ||
               cfg->accel_y_enable ||
               cfg->accel_z_enable )
            {
                lis2dh12_wrapper_measurement_t measurement;
                LIS2DH12_wrapper_readMeasurement(&measurement);

                m_sensor_data.acc_x = measurement.accel_x;
                m_sensor_data.acc_y = measurement.accel_y;
                m_sensor_data.acc_z = measurement.accel_z;
            }

            send_data(&m_sensor_data);

            toggle_green();
            toggle_red();

            m_task_state = SENSOR_TASK_STATE_START_MEAS;
            time_to_run = cfg->sensors_period_ms;
            break;
        }
        default:
        {
            m_task_state = SENSOR_TASK_STATE_START_MEAS;
            time_to_run = cfg->sensors_period_ms;
            break;
        }
    }

    return time_to_run;
}

/**
    @brief Function called when the application configuration as changed.
*/
static void on_config_update(void)
{
    const app_config_t * cfg = App_Config_get();

    if(cfg->sensors_period_ms == 0)
    {
        /* Cancel sensor task. Don't send sensor data anymore. */
        App_Scheduler_cancelTask(sensor_task);
    }
    else
    {
        /* Config changed, schedule Sensor task ASAP. */
        App_Scheduler_addTask_execTime(sensor_task, APP_SCHEDULER_SCHEDULE_ASAP, 100);
    }
}

/**
    @brief Task called every 2s until the device is ready. Then the task proceed with the main sensor task.
*/
static uint32_t startup_task (void)
{
    E_sht_wrapper_state_t res = 0;
    res = SHT_wrapper_readyState();

    // Check serial is valid
    if (res == E_SHT_READY)
    {
        led_red(false);
        led_green(true);

        /* Launch the sensor task. */
        App_Scheduler_addTask_execTime(sensor_task, APP_SCHEDULER_SCHEDULE_ASAP, 100);
        return APP_SCHEDULER_STOP_TASK;
    }
    else if (res == E_SHT_BUSY)
    {
        // Wait for communication to be fully ready
        led_red(true);
        led_green(true);
        return 2000;
    }
    else // Serial not valid or communication error
    {
        led_red(true);
        led_green(false);
        return 2000;
    }
}

/**
    @brief   Initialization callback for application

    This function is called after hardware has been initialized but the
    stack is not yet running.
*/
void App_init(const app_global_functions_t * functions)
{

    led_red(true);
    led_green(true);

    /* Basic configuration of the node with a unique node address. */
    if (configureNodeFromBuildParameters() != APP_RES_OK)
    {
        /*
         * Could not configure the node.
         * It should not happen except if one of the config value is invalid.
         */
        return;
    }

    m_sensor_data.count = 0;
    m_task_state = SENSOR_TASK_STATE_START_MEAS;

    /* Initialize all the modules. */
    App_Config_init(on_config_update);

    // Power ON sensors ! => No difference if set / unset or commented....
    //Gpio_outputWrite(BOARD_GPIO_ID_SENSOR_PWR_1, GPIO_LEVEL_HIGH);
    //Gpio_outputWrite(BOARD_GPIO_ID_SENSOR_PWR_2, GPIO_LEVEL_HIGH);

    // Communication initialisation
    ruuvi_i2c_init();
    ruuvi_spi_init();

    // Driver initialisation
    SHT_wrapper_init();
    LIS2DH12_wrapper_init();

    // Start startup task
    App_Scheduler_addTask_execTime(startup_task, APP_SCHEDULER_SCHEDULE_ASAP, 500);

    /*
     * Start the stack.
     * This is really important step, otherwise the stack will stay stopped and
     * will not be part of any network. So the device will not be reachable
     * without reflashing it
     */
    lib_state->startStack();
}
