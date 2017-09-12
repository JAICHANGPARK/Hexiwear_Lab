
/*
 * mbed library program
 *  Low Power, High Accuracy Digital Humidity Sensor with Integrated Temperature Sensor
 *  HDC1000 Texas Instruments
 *
 * Copyright (c) 2015,'17 Kenji Arai / JH1PJL
 *  http://www.page.sannet.ne.jp/kenjia/index.html
 *  http://mbed.org/users/kenjiArai/
 *      Created: Feburary   9th, 2015
 *      Revised: AAugust   21st, 2017
 */
/*
 *---------------- REFERENCE ----------------------------------------------------------------------
 *  http://www.ti.com/product/HDC1000/description
 *  http://akizukidenshi.com/catalog/g/gM-08775/ (Not avairable now)
 */
 
#ifndef HDC1000_H
#define HDC1000_H
 
#include "mbed.h"
 
// Humidity / Temperature Sensor, HDC1000 T.I.
// Address b7=1,b6=0,b5=0,b4=0,b3=0,b2=0,b1=0, b0=R/W
#define HDC1000ADDR   (0x40 << 1)  // CLICK BOARD

 
////////////// Registers //////////////////////////////////
// Register definition
#define HDC1000_REG_TEMP    0x00
#define HDC1000_REG_HUMI    0x01
#define HDC1000_REG_CONFIG  0x02
#define HDC1000_REG_S_ID_F  0xfb
#define HDC1000_REG_S_ID_M  0xfc
#define HDC1000_REG_S_ID_L  0xfd
#define HDC1000_REG_M_ID    0xfe
#define HDC1000_REG_D_ID    0xff
 
////////////// ID /////////////////////////////////////////
#define I_AM_HDC1000        0x1000
#define DEV_REG_ID          0x5449
 
////////////// Operating mode ///////////////////
#define ACQ_MODE_SEPARETE   (0UL << 12)
#define ACQ_MODE_BOTH       (1UL << 12)
#define TRES_14BIT          (0UL << 10)
#define TRES_11BIT          (1UL << 10)
#define HRES_14BIT          (0UL << 8)
#define HRES_11BIT          (1UL << 8)
#define HRES_08BIT          (2UL << 8)
#define BOTH_T_14_H_14      (TRES_14BIT + HRES_14BIT + ACQ_MODE_BOTH)
 

class HDC1000
{
public:
    /** Configure data pin (with other devices on I2C line)
      * @param data SDA and SCL pins
      */
    HDC1000(PinName p_sda, PinName p_scl);
 
    /** Configure data pin (with other devices on I2C line)
      * @param data SDA and SCL pins
      * @param device address
      */
    HDC1000(PinName p_sda, PinName p_scl, uint8_t addr);
 
    /** Configure data pin (with other devices on I2C line)
      * @param I2C previous definition
      */
    HDC1000(I2C& p_i2c);
    HDC1000(I2C& p_i2c, uint8_t addr);
 
    /** Start convertion & data save
      * @param none
      * @return none
      */
    void get(void);
 
    /** Read temperature data
      * @param none
      * @return temperature
      */
    float temperature(void);
    
    float conv_c_to_f(void);
 
    /** Read humidity data
      * @param none
      * @return humidity
      */
    float humidity(void);
 
    /** HDC1000 Configuration
      * @param none
      * @return none
      */
    void config(void);
 
    /** Read Configuration
      * @param none
      * @return config. data
      */
    uint16_t read_config(void);
 
    /** Set config register
      * @param config parameter
      * @return config read data
      */
    uint16_t set_config(uint16_t cfg);
 
    /** Set I2C clock frequency
      * @param freq.
      * @return none
      */
    void frequency(int hz);
 
    /** check Device ID number
      * @param none
      * @return HDC1000 = 1, others  0
      */
    uint8_t who_am_i(void);
 
    /** Read Manufacturer ID
      * @param none
      * @return ID
      */
    uint16_t read_M_ID(void);
 
    /** Read Device ID
      * @param none
      * @return ID
      */
    uint16_t read_D_ID(void);
 
protected:
    I2C *_i2c_p;
    I2C &_i2c;
 
    void get_IDs(void);
    void init(void);
 
private:
    uint8_t  HDC1000_addr;
    uint8_t  dt[4];
    uint16_t temp;
    uint16_t humi;
    uint16_t manufacturer_id_number;
    uint16_t device_id_number;
};
 
#endif      // HDC1000_H