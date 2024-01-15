#ifndef _SPL1301_H_
#define _SPL1301_H_

#define SPL1301_I2C_ADDR 0xEE

/* ��Ʒ��汾ID �Ĵ�����ַ */
#define PRODUCT_REVISION_ID 0x0D

/* ���� */
#define CONTINUOUS_PRESSURE 1
#define CONTINUOUS_TEMPERATURE 2
#define CONTINUOUS_P_AND_T 3

/* ��������� */
#define PRESSURE_SENSOR 0
#define TEMPERATURE_SENSOR 1

/* SPL1301 ����У׼���� */
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

/* SPL1301 ���ݽṹ�� */
typedef struct
{
    spl1301_calib_param_t calib_param; /* calibration data */
    int8_t product_id;                 // ��ƷID
    int8_t revision_id;                // �޶�ID

    int32_t i32rawPressure;    // ԭʼѹ��ֵ
    int32_t i32rawTemperature; // ԭʼ�¶�ֵ
    int32_t i32kP;             // ѹ��ϵ��
    int32_t i32kT;             // �¶�ϵ��

    float pressure;    // ʵ��ѹ��ֵ
    float temperature; // ʵ���¶�ֵ

} spl1301_t;

int spl1301_init();
float get_spl1301_temperature();
float get_spl1301_pressure();

#endif
