/**
 ******************************************************************************
 * @file    main.c
 * @author  - Anthony E.Raterta
 * @version V1.0.0
 * @date    21-January-2025
 * @brief   Application entrypoint for I2C testing
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 mcu-dev
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>

#include "driver/src/bmp180.h"
#include "driver/src/utils/utils.h"

static bmp180_dev dev;
static bpm180_init_param dev_param;
static bmp180_data data;

int main(void) {
  if (bmp180_setup(&dev, dev_param) == BMP180_STATUS_SUCCESS) {
    printf("Initialization successful!\r\n");
  } else {
    printf("Initialization Failed, exiting app!\r\n");
    return BMP180_STATUS_INIT_ERR;
  }

  if (bmp180_online()) {
    printf("BMP180 detected\r\n\r\n");
  } else {
    printf("BMP180 not detected\r\n");
    return BMP180_STATUS_INIT_ERR;
  }

  while (true) {
    if (bmp180_get_temperature(&dev, &data) == BMP180_STATUS_SUCCESS) {
      printf("temperature: %.2f °C\r\n", data.temp);
    }
    if (bmp180_get_pressure(&dev, &data) == BMP180_STATUS_SUCCESS) {
      printf("pressure: %.3f atm\r\n", data.pressure_atm);
      printf("pressure: %d Pa\r\n", data.pressure_pa);
    }
    printf("\r\n");
    delay_ms(1000);
  }
  return 0;
}