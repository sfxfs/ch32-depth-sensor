#include "debug.h"

#include "IIC/iic.h"
#include "spl1301.h"

static spl1301_t spl1301;  //spl1301数据结构体

static void spl1301_rateset(uint8_t iSensor, uint8_t u8SmplRate, uint8_t u8OverSmpl)
{
    uint8_t buf;
    uint8_t reg = 0;
    int32_t i32kPkT = 0;
    switch (u8SmplRate)
    {
    case 2:
        reg |= (1 << 4);
        break;
    case 4:
        reg |= (2 << 4);
        break;
    case 8:
        reg |= (3 << 4);
        break;
    case 16:
        reg |= (4 << 4);
        break;
    case 32:
        reg |= (5 << 4);
        break;
    case 64:
        reg |= (6 << 4);
        break;
    case 128:
        reg |= (7 << 4);
        break;
    case 1:
    default:
        break;
    }
    switch (u8OverSmpl)
    {
    case 2:
        reg |= 1;
        i32kPkT = 1572864;
        break;
    case 4:
        reg |= 2;
        i32kPkT = 3670016;
        break;
    case 8:
        reg |= 3;
        i32kPkT = 7864320;
        break;
    case 16:
        i32kPkT = 253952;
        reg |= 4;
        break;
    case 32:
        i32kPkT = 516096;
        reg |= 5;
        break;
    case 64:
        i32kPkT = 1040384;
        reg |= 6;
        break;
    case 128:
        i32kPkT = 2088960;
        reg |= 7;
        break;
    case 1:
    default:
        i32kPkT = 524288;
        break;
    }

    if (iSensor == PRESSURE_SENSOR)
    {
        spl1301.i32kP = i32kPkT;
        buf = reg;
        spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x06, &buf, 1);
        if (u8OverSmpl > 8)
        {
            spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x09, &buf, 1);
            reg = buf;
            buf = reg | 0x04;
            spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x09, &buf, 1);
        }
        else
        {
            spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x09, &buf, 1);
            reg = buf;
            buf = reg & (~0x04);
            spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x09, &buf, 1);
        }
    }
    if (iSensor == TEMPERATURE_SENSOR)
    {
        spl1301.i32kT = i32kPkT;
        buf = reg | 0x80;
        spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x07, &buf, 1);
        if (u8OverSmpl > 8)
        {
            spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x09, &buf, 1);
            reg = buf;
            buf = reg | 0x08;
            spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x09, &buf, 1);
        }
        else
        {
            spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x09, &buf, 1);
            reg = buf;
            buf = reg & (~0x08);
            spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x09, &buf, 1);
        }
    }
}

static void spl1301_get_calib_param()
{
    uint8_t buf;
    uint32_t h;
    uint32_t m;
    uint32_t l;

    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x10, &buf, 1);
    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x11, &buf, 1);
    l = buf;
    spl1301.calib_param.c0 = (int16_t)h << 4 | l >> 4;
    spl1301.calib_param.c0 = (spl1301.calib_param.c0 & 0x0800) ? (0xF000 | spl1301.calib_param.c0) : spl1301.calib_param.c0;

    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x12, &buf, 1);
    l = buf;
    spl1301.calib_param.c1 = (int16_t)(h & 0x0F) << 8 | l;
    spl1301.calib_param.c1 = (spl1301.calib_param.c1 & 0x0800) ? (0xF000 | spl1301.calib_param.c1) : spl1301.calib_param.c1;

    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x13, &buf, 1);
    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x14, &buf, 1);
    m = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x15, &buf, 1);
    l = buf;
    spl1301.calib_param.c00 = (int32_t)h << 12 | (int32_t)m << 4 | (int32_t)l >> 4;
    spl1301.calib_param.c00 = (spl1301.calib_param.c00 & 0x080000) ? (0xFFF00000 | spl1301.calib_param.c00) : spl1301.calib_param.c00;

    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x16, &buf, 1);
    m = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x17, &buf, 1);
    l = buf;
    spl1301.calib_param.c10 = (int32_t)(h & 0x0F) << 16 | (int32_t)m << 8 | l;
    spl1301.calib_param.c10 = (spl1301.calib_param.c10 & 0x080000) ? (0xFFF00000 | spl1301.calib_param.c10) : spl1301.calib_param.c10;

    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x18, &buf, 1);
    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x19, &buf, 1);
    l = buf;
    spl1301.calib_param.c01 = (int16_t)h << 8 | l;

    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x1A, &buf, 1);
    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x1B, &buf, 1);
    l = buf;
    spl1301.calib_param.c11 = (int16_t)h << 8 | l;

    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x1C, &buf, 1);
    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x1D, &buf, 1);
    l = buf;
    spl1301.calib_param.c20 = (int16_t)h << 8 | l;

    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x1E, &buf, 1);
    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x1F, &buf, 1);
    l = buf;
    spl1301.calib_param.c21 = (int16_t)h << 8 | l;

    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x20, &buf, 1);
    h = buf;
    spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x21, &buf, 1);
    l = buf;
    spl1301.calib_param.c30 = (int16_t)h << 8 | l;
}

void spl1301_start_temperature()
{
    uint8_t buf = 0x02;
    spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x08, &buf, 1);
}

void spl1301_start_pressure()
{
    uint8_t buf = 0x01;
    spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x08, &buf, 1);
}

void spl1301_start_continuous(uint8_t mode)
{
    uint8_t buf = mode + 4;
    spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x08, &buf, 1);
}

void spl1301_stop()
{
    uint8_t buf = 0;
    spl1301_interface_iic_write(SPL1301_I2C_ADDR, 0x08, &buf, 1);
}

static uint8_t spl1301_get_raw_temp()
{
    uint8_t ret, buf, h, m, l;
    ret = 0;
    // 高位在前 (datasheet P17)
    ret += spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x03, &buf, 1);
    h = buf;
    ret += spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x04, &buf, 1);
    m = buf;
    ret += spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x05, &buf, 1);
    l = buf;

    spl1301.i32rawTemperature = (int32_t)h << 16 | (int32_t)m << 8 | (int32_t)l;
    spl1301.i32rawTemperature = (spl1301.i32rawTemperature & 0x800000) ? (0xFF000000 | spl1301.i32rawTemperature) : spl1301.i32rawTemperature;
    return ret;
}

static uint8_t spl1301_get_raw_pressure()
{
    uint8_t ret, h, m, l;

    ret = 0;
    // 高位在前 (datasheet P17)
    ret += spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x00, &h, 1);
    ret += spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x01, &m, 1);
    ret += spl1301_interface_iic_read(SPL1301_I2C_ADDR, 0x02, &l, 1);

    spl1301.i32rawPressure = (int32_t)h << 16 | (int32_t)m << 8 | (int32_t)l;
    spl1301.i32rawPressure = (spl1301.i32rawPressure & 0x800000) ? (0xFF000000 | spl1301.i32rawPressure) : spl1301.i32rawPressure;
    return ret;
}

float get_spl1301_temperature()
{
    float fTsc;
    if (spl1301_get_raw_temp() != 0) // 获取原始温度值
    {
        return -1;
    }

    fTsc = spl1301.i32rawTemperature / (float)spl1301.i32kT;
    spl1301.temperature = spl1301.calib_param.c0 * 0.5 + spl1301.calib_param.c1 * fTsc;

    return spl1301.temperature;
}

float get_spl1301_pressure()
{
    float fTsc, fPsc;
    float qua2, qua3;
    if (spl1301_get_raw_pressure() != 0) // 获取原始压力值
    {
        return -1;
    }

    fTsc = spl1301.i32rawTemperature / (float)spl1301.i32kT;
    fPsc = spl1301.i32rawPressure / (float)spl1301.i32kP;
    qua2 = spl1301.calib_param.c10 + fPsc * (spl1301.calib_param.c20 + fPsc * spl1301.calib_param.c30);
    qua3 = fTsc * fPsc * (spl1301.calib_param.c11 + fPsc * spl1301.calib_param.c21);

    spl1301.pressure = spl1301.calib_param.c00 + fPsc * qua2 + fTsc * spl1301.calib_param.c01 + qua3;

    return spl1301.pressure;
}

static int spl1301_get_id()
{
    uint8_t buf = 0;
    uint8_t ret = spl1301_interface_iic_read(SPL1301_I2C_ADDR, PRODUCT_REVISION_ID, &buf, 1);
//    if (ret == 0)
//        printf("find sensor with id:%d\r\n", buf);
//    else
//        printf("cannot find sensor iic ret:%d\r\n", ret);
    return ret;
}

int spl1301_init()
{
    spl1301_interface_iic_init();
    if (spl1301_get_id() != 0)
        return -1;
    spl1301_get_calib_param();
    spl1301_rateset(PRESSURE_SENSOR, 32, 8);
    spl1301_rateset(TEMPERATURE_SENSOR, 32, 8);
    spl1301_start_continuous(CONTINUOUS_P_AND_T);
    return 0;
}
