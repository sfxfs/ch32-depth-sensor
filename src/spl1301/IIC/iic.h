#ifndef _SPL1301_IIC_H_
#define _SPL1301_IIC_H_

uint8_t spl1301_interface_iic_init(void);
uint8_t spl1301_interface_iic_deinit(void);
uint8_t spl1301_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
uint8_t spl1301_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

#endif
