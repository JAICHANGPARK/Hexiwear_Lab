/*
 *  mbed library program
 *  Low Power, High Accuracy Digital Humidity Sensor with Integrated Temperature Sensor
 *  HDC1000 Texas Instruments
 *
 * Copyright (c) 2015,'17 Kenji Arai / JH1PJL
 *  http://www.page.sannet.ne.jp/kenjia/index.html
 *  http://mbed.org/users/kenjiArai/
 *      Created: Feburary   9th, 2015
 *      Revised: AAugust   21st, 2017
 */

#include "HDC1000.h"

HDC1000::HDC1000 (PinName p_sda, PinName p_scl) :
 _i2c_p(new I2C(p_sda, p_scl)), _i2c(*_i2c_p)
{
    HDC1000_addr = HDC1000ADDR;
    init();
}

HDC1000::HDC1000 (PinName p_sda, PinName p_scl, uint8_t addr) :
 _i2c_p(new I2C(p_sda, p_scl)), _i2c(*_i2c_p)
{
    HDC1000_addr = addr;
    init();
}

HDC1000::HDC1000 (I2C& p_i2c) : _i2c(p_i2c)
{
    HDC1000_addr = HDC1000ADDR;
    init();
}

HDC1000::HDC1000 (I2C& p_i2c, uint8_t addr) : _i2c(p_i2c)
{
    HDC1000_addr = addr;
    init();
}

/////////////// Start conv. and gwt all data //////////////
void HDC1000::get()
{

    dt[0] = HDC1000_REG_TEMP;
    _i2c.write((int)HDC1000_addr, (char *)dt, 1, true);
    wait_ms(15);
    _i2c.read((int)HDC1000_addr, (char *)dt, 4, false);
    temp = dt[0] << 8 | dt[1];
    humi = dt[2] << 8 | dt[3];
}

/////////////// Read data from sensor /////////////////////
float HDC1000::temperature()
{
    return (float)temp * 165 / 65536 - 40;
}

float HDC1000::conv_c_to_f()
{
    float tmp = temperature();
    tmp = temp;
    return (tmp * 1.8f) + 32.0f ;
}


/////////////// Read data from sensor /////////////////////
float HDC1000::humidity()
{
    return (float)humi * 100 / 65536;
}

/////////////// Initialize ////////////////////////////////
void HDC1000::init()
{
    _i2c.frequency(100000);
    get_IDs();
    set_config(BOTH_T_14_H_14);
}

/////////////// ID ////////////////////////////////////////
uint16_t HDC1000::read_M_ID()
{
    return manufacturer_id_number;
}

uint16_t HDC1000::read_D_ID()
{
    return device_id_number;
}

uint8_t HDC1000::who_am_i()
{
    if (device_id_number == I_AM_HDC1000) {
        return 1;
    } else {
        return 0;
    }
}

/////////////// Configration //////////////////////////////
uint16_t HDC1000::set_config(uint16_t cfg)
{
    dt[0] = HDC1000_REG_CONFIG;
    dt[1] = (uint8_t)(cfg >> 8);
    dt[2] = (uint8_t)(cfg & 0xff);
    _i2c.write((int)HDC1000_addr, (char *)dt, 3, false);
    return read_config();
}

uint16_t HDC1000::read_config(void)
{
    dt[0] = HDC1000_REG_CONFIG;
    _i2c.write((int)HDC1000_addr, (char *)dt, 1, true);
    _i2c.read(HDC1000_addr, (char *)dt, 2, false);
    return (uint16_t)(dt[0] << 8 | dt[1]);
}

/////////////// I2C Freq. /////////////////////////////////
void HDC1000::frequency(int hz)
{
    _i2c.frequency(hz);
}

/////////////// Read ID ///////////////////////////////////
void HDC1000::get_IDs()
{
    // Manufacturer ID
    dt[0] = HDC1000_REG_M_ID;
    _i2c.write((int)HDC1000_addr, (char *)dt, 1, true);
    _i2c.read(HDC1000_addr, (char *)dt, 2, false);
    manufacturer_id_number = dt[0] << 8 | dt[1];
    // Device ID
    dt[0] = HDC1000_REG_D_ID;
    _i2c.write((int)HDC1000_addr, (char *)dt, 1, true);
    _i2c.read(HDC1000_addr, (char *)dt, 2, false);
    device_id_number = dt[0] << 8 | dt[1];
}
