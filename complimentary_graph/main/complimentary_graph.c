#include "sra18.h"
#include "mpu.h"

static const char *TAG_COMP_GRAPH = "complimentary_graph";

float acce_angle = 0;
float gyro_rate = 0;
float gyro_angle = 0;
static float counter = 0;
static uint32_t timer = 0;
static float dt = 0;
float complimentary_angle = 0;

void app_main()
{
  uint8_t* acce_rd = (uint8_t*) malloc(BUFF_SIZE);
  uint8_t* gyro_rd = (uint8_t*) malloc(BUFF_SIZE);
  int16_t* acce_raw_value = (int16_t*) malloc(BUFF_SIZE/2);
  int16_t* gyro_raw_value = (int16_t*) malloc(BUFF_SIZE/2);

  i2c_master_init();  //Initialise the I2C interface
  start_mpu();        //Intialise the MPU 
  
  enable_buttons();
  
  int ret;
  int flag = 1;

  while(true)
  {

    dt = (float) (msec() - timer) / 1000000;
    timer = msec();

    ret = mpu6050_read_acce(I2C_MASTER_NUM, acce_rd, BUFF_SIZE);
    shift_buf(acce_rd, acce_raw_value, BUFF_SIZE/2);

    ret = mpu6050_read_gyro(I2C_MASTER_NUM, gyro_rd, BUFF_SIZE);
    shift_buf(gyro_rd, gyro_raw_value, BUFF_SIZE/2);

    acce_angle = (atan2(-(acce_raw_value[0]), acce_raw_value[2]) * RAD_TO_DEG);

    gyro_rate = gyro_raw_value[1] / 131;
    gyro_angle = gyro_rate * dt;

    complimentary_angle = (complimentary_angle + gyro_angle) * 0.98 + acce_angle * 0.02;
    logD(TAG_COMP_GRAPH, "%f\t%f\t%f", gyro_angle, acce_angle, complimentary_angle);

  }
}

