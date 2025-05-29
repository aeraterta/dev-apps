#include "driver/src/adxl345.h"
#include "driver/src/utils/utils.h"
#include <stdbool.h>
#include <stdio.h>

static adxl345_dev dev;
static adxl345_init_param dev_param;
static adxl345_axes_data adxl345_data;

typedef enum {
  STATE_INIT = 0,
  STATE_CHECK_ONLINE,
  STATE_READ_INTERRUPT,
  STATE_READ_DATA,
  STATE_ERROR
} adxl_state_t;

int main(void) {
  bool stat          = true;
  adxl_state_t state = STATE_INIT;

  while (stat) {
    switch (state) {

    case STATE_INIT:
      dev_param.power_mode            = ADXL345_NORMAL_MODE;
      dev_param.odr                   = ADXL345_ODR_1_56HZ;
      dev_param.scale.scale           = ADXL345_SCALE_2G;
      dev_param.resolution.resolution = ADXL345_RES_FULL;

      if (adxl345_setup(&dev, dev_param) == ADXL345_STATUS_SUCCESS) {
        printf("Initialization successful!\r\n");
        state = STATE_CHECK_ONLINE;
      } else {
        printf("Initialization Failed!\r\n");
        state = STATE_ERROR;
      }
      break;

    case STATE_CHECK_ONLINE:
      if (adxl345_online()) {
        printf("ADXL345 Detected!\r\n");
        state = STATE_READ_INTERRUPT;
      } else {
        printf("ADXL345 Not Detected!\r\n");
        state = STATE_ERROR;
      }
      break;

    case STATE_READ_INTERRUPT:
      if (adxl345_get_interrupt_status(&dev) == ADXL345_STATUS_SUCCESS) {
        if (dev.interrupt_status.data_ready) {
          state = STATE_READ_DATA;
        } else {
          delay_ms(100);
        }
      } else {
        printf("Failed to read interrupt status!\r\n");
        state = STATE_ERROR;
      }
      break;

    case STATE_READ_DATA:
      if (adxl345_get_raw_xyz(&dev, &adxl345_data) == ADXL345_STATUS_SUCCESS) {
        adxl345_get_acc_xyz(&dev, &adxl345_data);

        printf("raw X: %d  acc X: %.4f\r\n", adxl345_data.raw_data.x,
               adxl345_data.acc_data.x);
        printf("raw Y: %d  acc Y: %.4f\r\n", adxl345_data.raw_data.y,
               adxl345_data.acc_data.y);
        printf("raw Z: %d  acc Z: %.4f\r\n", adxl345_data.raw_data.z,
               adxl345_data.acc_data.z);
        printf("\r\n");
      } else {
        printf("Failed to read axis data!\r\n");
        state = STATE_ERROR;
      }
      delay_ms(1000);
      state = STATE_READ_INTERRUPT;
      break;

    case STATE_ERROR:
      printf("ERROR: Entered error state. Halting...\r\n");
      stat = false;
      break;

    default:
      printf("Invalid state!\r\n");
      state = STATE_ERROR;
      break;
    }
  }

  return 0;
}
