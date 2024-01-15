#ifndef _SPL1301_H_
#define _SPL1301_H_

#define SPL1301_I2C_ADDR 0xEE

/* 产品与版本ID 寄存器地址 */
#define PRODUCT_REVISION_ID 0x0D

/* 命令 */
#define CONTINUOUS_PRESSURE 1
#define CONTINUOUS_TEMPERATURE 2
#define CONTINUOUS_P_AND_T 3

/* 传感器标号 */
#define PRESSURE_SENSOR 0
#define TEMPERATURE_SENSOR 1

/* SPL1301 出厂校准参数 */
typedef struct
{
    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;

} spl1301_calib_param_t;

/* SPL1301 数据结构体 */
typedef struct
{
    spl1301_calib_param_t calib_param; /* calibration data */
    int8_t product_id;                 // 产品ID
    int8_t revision_id;                // 修订ID

    int32_t i32rawPressure;    // 原始压力值
    int32_t i32rawTemperature; // 原始温度值
    int32_t i32kP;             // 压力系数
    int32_t i32kT;             // 温度系数

    float pressure;    // 实际压力值
    float temperature; // 实际温度值

} spl1301_t;

int spl1301_init();
float get_spl1301_temperature();
float get_spl1301_pressure();

#endif
