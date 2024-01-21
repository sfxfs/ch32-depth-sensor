#include "interface/spi.h"
#include "ms5803.h"
#include "math.h"

#define SENSOR_CMD_RESET      0x1E
#define SENSOR_CMD_ADC_READ   0x00
#define SENSOR_CMD_ADC_CONV   0x40
#define SENSOR_CMD_ADC_D1     0x00
#define SENSOR_CMD_ADC_D2     0x10
#define SENSOR_CMD_ADC_256    0x00
#define SENSOR_CMD_ADC_512    0x02
#define SENSOR_CMD_ADC_1024   0x04
#define SENSOR_CMD_ADC_2048   0x06
#define SENSOR_CMD_ADC_4096   0x08

static unsigned int      sensorCoefficients[8];           // calibration coefficients
static unsigned long     D1                       = 0;    // Stores uncompensated pressure value
static unsigned long     D2                       = 0;    // Stores uncompensated temperature value
static float             deltaTemp                = 0;    // These three variable are used for the conversion.
static float             sensorOffset             = 0;
static float             sensitivity              = 0;

static unsigned char ms5803CRC4(unsigned int n_prom[])
{
    int cnt;
    unsigned int n_rem;
    unsigned int crc_read;
    unsigned char  n_bit;

    n_rem = 0x00;
    crc_read = sensorCoefficients[7];
    sensorCoefficients[7] = ( 0xFF00 & ( sensorCoefficients[7] ) );

    for (cnt = 0; cnt < 16; cnt++)
    { // choose LSB or MSB
        if ( cnt%2 == 1 ) n_rem ^= (unsigned short) ( ( sensorCoefficients[cnt>>1] ) & 0x00FF );
        else n_rem ^= (unsigned short) ( sensorCoefficients[cnt>>1] >> 8 );
        for ( n_bit = 8; n_bit > 0; n_bit-- )
        {
            if ( n_rem & ( 0x8000 ) )
            {
                n_rem = ( n_rem << 1 ) ^ 0x3000;
            }
            else {
                n_rem = ( n_rem << 1 );
            }
        }
    }

    n_rem = ( 0x000F & ( n_rem >> 12 ) );// // final 4-bit reminder is CRC code
    sensorCoefficients[7] = crc_read; // restore the crc_read to its original place

    return ( n_rem ^ 0x00 ); // The calculated CRC should match what the device initally returned.
}

static unsigned int ms5803_read_coefficient(uint8_t index)
{
    spi_start_transmit();
    uint8_t tx_buf[3] = {0xA0 + ( index * 2 ), 0x00, 0x00};
    uint8_t rx_buf[3] = {0};
    spi_transmit(tx_buf, rx_buf, 3);
    unsigned int result = (rx_buf[1] << 8) | rx_buf[2];
    spi_end_transmit();
    return result;
}

static int ms5803_reset()
{
    spi_start_transmit();
    uint8_t tx, rx;
    tx = SENSOR_CMD_RESET;
    int ret = spi_transmit(&tx, &rx, 1);
    Delay_Ms(10);
    spi_end_transmit();
    Delay_Ms(5);
    return ret;
}

int ms5803_init()
{
    spi_init();
    if (ms5803_reset() != 0)
        return -1;
    for (int i = 0; i < 8; i++)
    {
        sensorCoefficients[ i ] = ms5803_read_coefficient( i );
        printf("coefficient[%d]=%d\r\n", i, sensorCoefficients[ i ]);
        Delay_Ms(10);
    }
    unsigned char p_crc = sensorCoefficients[ 7 ];
    unsigned char n_crc = ms5803CRC4( sensorCoefficients );
    if (p_crc != n_crc)
        return -2;
    return 0;
}

static unsigned long ms5803_cmd_adc(char cmd) {

    unsigned int result = 0;

    spi_start_transmit();
    uint8_t tx, rx;
    tx = SENSOR_CMD_ADC_CONV + cmd;
    spi_transmit(&tx, &rx, 1);

    switch ( cmd & 0x0f )
    {
        case SENSOR_CMD_ADC_256 :
            Delay_Ms( 1 );
            break;
        case SENSOR_CMD_ADC_512 :
            Delay_Ms( 3 );
            break;
        case SENSOR_CMD_ADC_1024:
            Delay_Ms( 4 );
            break;
        case SENSOR_CMD_ADC_2048:
            Delay_Ms( 6 );
            break;
        case SENSOR_CMD_ADC_4096:
            Delay_Ms( 10 );
            break;
    }

    spi_end_transmit();
    Delay_Ms(3);
    spi_start_transmit();
    uint8_t tx_buf[4] = {SENSOR_CMD_ADC_READ, 0x00, 0x00, 0x00};
    uint8_t rx_buf[4] = {0};
    spi_transmit(tx_buf, rx_buf, 4);
    result = 65536 * rx_buf[1] + 256 * rx_buf[2] + rx_buf[3];
    spi_end_transmit();

    return result;
}

void ms5803_read_temp_and_press(float *temp, float *press)
{
    D1 = ms5803_cmd_adc( SENSOR_CMD_ADC_D1 + SENSOR_CMD_ADC_4096 );    // read uncompensated pressure
    D2 = ms5803_cmd_adc( SENSOR_CMD_ADC_D2 + SENSOR_CMD_ADC_4096 );    // read uncompensated temperature

    // calculate 1st order pressure and temperature correction factors (MS5803 1st order algorithm).
    deltaTemp = D2 - sensorCoefficients[5] * pow( 2, 8 );
    sensorOffset = sensorCoefficients[2] * pow( 2, 16 ) + ( deltaTemp * sensorCoefficients[4] ) / pow( 2, 7 );
    sensitivity = sensorCoefficients[1] * pow( 2, 15 ) + ( deltaTemp * sensorCoefficients[3] ) / pow( 2, 8 );

    // calculate 2nd order pressure and temperature (MS5803 2st order algorithm)
    *temp = ( 2000 + (deltaTemp * sensorCoefficients[6] ) / pow( 2, 23 ) ) / 100;
    *press = ( ( ( ( D1 * sensitivity ) / pow( 2, 21 ) - sensorOffset) / pow( 2, 15 ) ) / 10 );
}
