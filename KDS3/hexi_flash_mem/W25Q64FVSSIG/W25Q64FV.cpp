#include "W25Q64FV.h"

W25Q64FV::W25Q64FV(PinName mosi, PinName miso, PinName sclk, PinName cs, int frequency)
{

    this->cs = new DigitalOut(cs);
    this->cs->write(true);
    this->frequency = frequency;

    spi = new SPI(mosi, miso, sclk);
    spi->format(8, 0);
    spi->frequency(frequency);
}

W25Q64FV::~W25Q64FV()
{
    delete spi;
    delete cs;
}

uint16_t W25Q64FV::Id()
{
    cs->write(0);   // cs 핀을 LOW로 만든다. DigitalOut.h 파일에 정의되어있음 
    spi->write(CMD_MANU_ID);
    spi->write(0);
    spi->write(0);
    spi->write(0);
    unsigned id = (spi->write(0) << 8) | spi->write(0);
    cs->write(1);
    return id;
}

void W25Q64FV::W25Q64_readManufacturer(uint8_t* d)
{

    cs -> write(0);
    spi->write(CMD_MANU_ID);
    for (uint8_t i =0; i <5; i++) {
        d[i] = spi->write(0);
    }
    cs -> write(1);
}

uint32_t W25Q64FV::JEDECId()
{
    cs->write(0);
    spi->write(CMD_JEDEC_ID);
    unsigned id = (spi->write(0) << 16) | (spi->write(0) << 8) | spi->write(0);
    cs->write(1);
    return id;
}

void W25Q64FV::writeEnable(void)
{
    cs->write(0);
    spi->write(CMD_WREN);
    cs->write(1);
}

void W25Q64FV::writeDisable(void)
{
    cs->write(0);
    spi->write(CMD_WR_DISABLE) ;
    cs->write(1);
}

uint8_t W25Q64FV::readStatus(void)
{
    uint8_t data ;
    cs->write(0);
    spi->write(CMD_RDSR1) ;
    data = spi->write(DUMMY) ;                     // dummy
    cs->write(1);
    return( data ) ;
}

void W25Q64FV::writeStatusReg(int addr)            // Write SR cmd 01h + 3B data
{
    cs->write(0);
    spi->write(CMD_WRSR1) ;                         // Write SR cmd 01h
    spi->write((addr >> 16)&0xFF) ;                // address
    spi->write((addr >>  8)&0xFF) ;
    spi->write(addr & 0xFF) ;
    cs->write(1);
}

void W25Q64FV::writeSecurityReg(int addr)          // WRSCUR cmd 2Fh + 1B data
{
    cs->write(0);
    // spi->write(CMD_WRSCUR) ;                         // Write SR cmd 01h
    spi->write(addr & 0xFF) ;
    cs->write(1);
}


uint8_t W25Q64FV::readByte(int addr)                  // Single Byte Read
{
    uint8_t data ;
    cs->write(0);
    spi->write(CMD_READ_DATA) ;                         // send 03h
    spi->write((addr >> 16)&0xFF) ;
    spi->write((addr >>  8)&0xFF) ;
    spi->write(addr & 0xFF) ;
    data = spi->write(DUMMY) ;                     // write data is dummy
    cs->write(1);
    return( data ) ;                                // return 1 byte
}

bool W25Q64FV::read(uint32_t addr, uint8_t* dst, uint32_t len)
{
    cs->write(0);
    spi->write(0x03);
    spi->write((addr >> 16) & 0xff);
    spi->write((addr >> 8) & 0xff);
    spi->write(addr & 0xff);
    for (uint32_t i=0; i<len; ++i)
        dst[i] = spi->write(0);
    cs->write(1);

    return true;
}

void W25Q64FV::hsread(uint32_t addr, uint8_t* dst, uint32_t len, int frequency)
{
    int save_frequency = this->frequency;
    spi->frequency(frequency);
    cs->write(0);
    spi->write(0x0B);
    spi->write((addr >> 16) & 0xff);
    spi->write((addr >> 8) & 0xff);
    spi->write(addr & 0xff);
    spi->write(0); // dummy
    for (uint32_t i=0; i<len; ++i)
        dst[i] = spi->write(0);
    cs->write(1);
    spi->frequency(save_frequency);
}

uint8_t W25Q64FV::readSFDP(int addr)               // Read SFDP
{
    uint8_t data ;
    cs->write(0);
    spi->write(CMD_READ_SFDP) ;                       // send cmd 5Ah
    spi->write((addr >> 16)&0xFF) ;                // address[23:16]
    spi->write((addr >>  8)&0xFF) ;                // address[15:8]
    spi->write(addr & 0xFF) ;                      // address[7:0]
    spi->write(DUMMY) ;                            // dummy cycle
    data = spi->write(DUMMY) ;                     // return 1 byte
    cs->write(1);
    return( data ) ;
}

uint8_t W25Q64FV::wait_while_busy(void)
{
    uint8_t temp = 0;
    cs->write(0);           //Enable device
    spi->write(CMD_RDSR1);   //Send RDSR command
    temp = spi->write(DUMMY);
    cs->write(1);          //Disable
    if (temp & 0x01) return 1;
    else return 0;
}


bool W25Q64FV::page_program(uint32_t addr, uint8_t* write_buffer, uint8_t len)
{
    // no point in writing FF as an empty sector already has those
    // (and if not empty, write won't succeed)
    bool skipped = false;
    while (len > 0 && *write_buffer == 0xFF) {
        ++write_buffer;
        --len;
        ++addr;
        skipped = true;
    }
    if (len == 0 && skipped)
        return true; // special case when succeeds when nothing to do

    if (len < 1 || len > 256)
        return false;

    // write enable
    writeEnable();

    cs->write(0);
    spi->write(0x02);
    spi->write((uint8_t)(addr >> 16));
    spi->write((uint8_t)(addr >> 8));
    spi->write((uint8_t)addr);
    for (uint16_t i=0; i<len; ++i)
        spi->write(write_buffer[i]);
    cs->write(1);
    wait_while_busy();

    return true;
}

void W25Q64FV::sector_erase_4k(uint32_t addr)
{
    cs->write(0);
    spi->write(CMD_ERASE_SECTOR);
    spi->write((uint8_t)(addr >> 16));
    spi->write((uint8_t)(addr >> 8));
    spi->write((uint8_t)addr);
    cs->write(1);
    wait_while_busy();
}

void W25Q64FV::block_erase_32k(uint32_t addr)
{
    cs->write(0);
    spi->write(CMD_ERASE_BLOCK32);
    spi->write((uint8_t)(addr >> 16));
    spi->write((uint8_t)(addr >> 8));
    spi->write((uint8_t)addr);
    cs->write(1);
    wait_while_busy();
}

void W25Q64FV::block_erase_64k(uint32_t addr)
{
    cs->write(0);
    spi->write(CMD_ERASE_BLOCK64);
    spi->write((uint8_t)(addr >> 16));
    spi->write((uint8_t)(addr >> 8));
    spi->write((uint8_t)addr);
    cs->write(1);
    wait_while_busy();
}

void W25Q64FV::chip_erase()
{
    cs->write(0);
    spi->write(CMD_ERASE_CHIP);
    cs->write(1);
    wait_while_busy();
}






void W25Q64FV::writeArray(uint32_t address, uint8_t* pData, uint32_t arrayLength)
{
    unsigned int i = 0;
    cs->write(0);                        //Enable device
    spi->write(CMD_WREN);                //Send WREN command
    cs->write(1);                      //Disable device

    cs->write(0);                        //Enable device
    spi->write(CMD_PAGEPROG);            //Send Byte Program command
    spi->write((uint8_t)(address >> 16) & 0xFF);
    spi->write((uint8_t)(address >>  8) & 0xFF);
    spi->write(0x00);

    for (i=0; i<arrayLength; i++) {
        spi->write(pData[i]);            //Send byte to be programmed
    }
    cs->write(1);              //Disable device

    //Wait Busy
    while ((readStatus() & 0x01) == 0x01) { //Waste time until not busy
    }
}


void W25Q64FV::readArray(uint32_t address, uint8_t* pData, uint32_t arrayLength)
{
    unsigned int i = 0;

    cs->write(0);                                    //Enable device
    spi->write(CMD_READ_DATA);                       //Read command
    spi->write((uint8_t)(address >> 16));
    spi->write((uint8_t)(address >>  8));
    spi->write((uint8_t) address);

    for (i = 0; i <arrayLength; i++) {              //Read until no_bytes is reached
        pData[i] = spi->write(DUMMY);                      //Receive bytes
    }
    cs->write(1);                                  //Disable device

    //Wait Busy
    while (readStatus() & 0x01) {    //Waste time until not busy
    }
}

//
// 書込み等の処理中チェック
// 戻り値: true:作業 、false:アイドル中
//
bool W25Q64FV::W25Q64_IsBusy()
{
    uint8_t r1;
    cs->write(0);
    spi->write(CMD_RDSR1);
    r1 = spi->write(0xff);
    cs->write(1);
    if(r1 & SR1_BUSY_MASK)
        return true;
    return false;
}

//
// データの書き込み
// sect_no(in) : セクタ番号(0x00 - 0x7FF)
// inaddr(in)  : セクタ内アドレス(0x00-0xFFF)
// data(in)    : 書込みデータ格納アドレス
// n(in)       : 書込みバイト数(0～256)
//
uint16_t W25Q64FV::W25Q64_pageWrite(uint16_t sect_no, uint16_t inaddr, uint8_t* data, uint8_t n)
{

    uint32_t addr = sect_no;
    int i;
    addr<<=12;
    addr += inaddr;

    writeEnable();

    if (W25Q64_IsBusy()) {
        return 0;
    }

    cs->write(0);
    spi->write(CMD_PAGEPROG);
    spi->write((addr>>16) & 0xff);
    spi->write((addr>>8) & 0xff);
    spi->write(addr & 0xff);

    for (i=0; i < n; i++) {
        spi->write(data[i]);
    }
    cs->write(1);

    while(W25Q64_IsBusy()) ;

    return i;
}

//
// データの読み込み
// addr(in): 読込開始アドレス (24ビット 0x00000 - 0xFFFFF)
// n(in):読込データ数
//
uint16_t W25Q64FV::W25Q64_read(uint32_t addr, uint8_t *buf, uint16_t n){ 
  cs->write(0);
  spi->write(CMD_READ_DATA);
  spi->write(addr>>16);
  spi->write((addr>>8) & 0xff);
  spi->write(addr & 0xff);
  
  uint16_t i;
  for(i = 0; i<n; i++ ) {
    buf[i] = spi->write(0x00);
  }
  
  cs -> write(1);
  return i;
}




