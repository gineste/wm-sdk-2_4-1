/* Copyright 2018 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * @file
 *
 * Board definition for the
 * <a href="https://ruuvi.com/ruuvitag-specs/">Ruuvitag device</a>
 */
#ifndef BOARD_RUUVITAG_H_
#define BOARD_RUUVITAG_H_

// Define the SPI instance to use
#define USE_SPI0
// SPI Port pins
#define BOARD_SPI_SCK_PIN   29
#define BOARD_SPI_MOSI_PIN  25
#define BOARD_SPI_MISO_PIN  28

// Define the I2C instance to use
#define USE_I2C1
// I2C Port pins
#define BOARD_I2C_SCL_PIN   5   // mapped to pin P0.05 (07)
#define BOARD_I2C_SDA_PIN   4   // mapped to pin P0.04 (06)

// List of GPIO pins
#define BOARD_GPIO_PIN_LIST             {17, /* P0.17 */                                                                                               \
                                        19, /* P0.19 */                                                                                                \
                                        13, /* P0.13 */                                                                                                \
                                        2,  /* P0.02. LIS2DH12 INT1 pin (can be replaced by P0.06 if you want to use the LIS2DH12 INT2 pin instead) */ \
                                        6,  /* P0.06. LIS2DH12 INT2 pin (instead of LIS2DH12 INT2 pin) */                                              \
                                        8,  /* P0.08. SPI CS pin for the LIS2DH12 accelerometer */                                                     \
                                        3,  /* P0.03. SPI CS pin for the DPS310 pressure sensor */                                                     \
                                        16, /* P0.16 */                                                                                                \
                                        12, /* P0.12 */                                                                                                \
                                        7}  /* P0.07.*/

// User friendly name for GPIOs (IDs mapped to the BOARD_GPIO_PIN_LIST table)
#define BOARD_GPIO_ID_LED1              0  // mapped to pin P0.17
#define BOARD_GPIO_ID_LED2              1  // mapped to pin P0.19
#define BOARD_GPIO_ID_BUTTON1           2  // mapped to pin P0.13
#define BOARD_GPIO_ID_LIS2DX12_INT1     3  // mapped to pin P0.02
#define BOARD_GPIO_ID_LIS2DX12_INT2     4  // mapped to pin P0.06
#define BOARD_GPIO_ID_LIS2DX12_SPI_CS   5  // mapped to pin P0.08
#define BOARD_GPIO_ID_DPS310_SPI_CS     6  // mapped to pin P0.03
#define BOARD_GPIO_ID_TMP116_ALERT      7  // mapped to pin P0.16
#define BOARD_GPIO_ID_SENSOR_PWR_1      8  // mapped to pin P0.12
#define BOARD_GPIO_ID_SENSOR_PWR_2      9  // mapped to pin P0.07

// List of LED IDs
#define BOARD_LED_ID_LIST               {BOARD_GPIO_ID_LED1,   BOARD_GPIO_ID_LED2}

// Active low polarity for LEDs
#define BOARD_LED_ACTIVE_LOW            true

// List of button IDs
#define BOARD_BUTTON_ID_LIST           {BOARD_GPIO_ID_BUTTON1}

// Active low polarity for buttons
#define BOARD_BUTTON_ACTIVE_LOW         true

// Active internal pull-up for buttons
#define BOARD_BUTTON_INTERNAL_PULL      true

// The board supports DCDC (#define BOARD_SUPPORT_DCDC)
// Since SDK v1.2 (bootloader > v7) this option has been move to
// board/<board_name>/config.mk. Set board_hw_dcdc to yes to enable DCDC.
#ifdef BOARD_SUPPORT_DCDC
#error This option has been moved to board/<board_name>/config.mk
#endif


#endif /* BOARD_RUUVITAG_H_ */
