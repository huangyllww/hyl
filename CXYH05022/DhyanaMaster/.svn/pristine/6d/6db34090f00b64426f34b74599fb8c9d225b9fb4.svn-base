#ifndef   __PCI_COMMON_H__
#define   __PCI_COMMON_H__

#define PCI_LIB_ADDRESS(Bus,Device,Function,Register)   \
  (((Register) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))


 /**
 * Register Offset Define
 * */
#define FCH_CFG_REG000                  0x000
#define FCH_CFG_REG019                  0x019
#define FCH_CFG_REG01A                  0x01A
#define FCH_CFG_REG084                  0x084
#define FCH_CFG_REG0A0                  0x0A0

#endif
