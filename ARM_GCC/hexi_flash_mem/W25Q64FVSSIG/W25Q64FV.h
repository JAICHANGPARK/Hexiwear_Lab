#include <mbed.h>
#include <stdint.h>

#define CMD_WREN                0x06
#define CMD_WR_DISABLE          0x04
#define CMD_RDSR1               0x05
#define CMD_RDSR2               0x35
#define CMD_WRSR1               0x01  //write status register
#define CMD_PAGEPROG            0x02

#define CMD_ERASE_SECTOR        0x20
#define CMD_ERASE_BLOCK32       0x52
#define CMD_ERASE_BLOCK64       0xD8
#define CMD_ERASE_CHIP          0x60

#define CMD_PROG_SUSPEND        0x75
#define CMD_PROG_RESUME         0x7A
#define CMD_PWR_DOWN            0xB9

#define CMD_READ_DATA           0x03
#define CMD_READ_HS             0x0B

#define CMD_MANU_ID             0x90
#define CMD_JEDEC_ID            0x9F        // Read Manufacturer and JDEC Device ID 
#define CMD_UNIQUE_ID           0x4B

#define CMD_READ_SFDP           0x5A
#define CMD_ERASE_SEC_REG       0x44
#define CMD_PROG_SEC_REG        0x42
#define CMD_READ_SEC_REG        0x48
#define CMD_ENABLE_RES          0x66
#define CMD_RESET               0x99

#define DUMMY                   0x00 // Dummy byte which can be changed to any value

#define SR1_BUSY_MASK   0x01
#define SR1_WEN_MASK    0x02

class W25Q64FV
{
public:

    static const int SID_LEN = 32;
    static const int SECTOR_LEN = 4096;
    static const int PAGE_LEN = 256;
    static const int MAX_ADDR = 0x7FFFFF;

private:

    SPI* spi;
    DigitalOut* cs;

    int frequency;
    static uint8_t sector_buffer[SECTOR_LEN];


public:
    W25Q64FV(PinName mosi, PinName miso, PinName sclk, PinName cs, int frequency=10000000);
    ~W25Q64FV();

    uint16_t Id();
    uint32_t JEDECId();
    void W25Q64_readManufacturer(uint8_t* d);
    bool W25Q64_IsBusy();
    uint16_t W25Q64_pageWrite(uint16_t sect_no, uint16_t inaddr, uint8_t* data, uint8_t n);
    uint16_t W25Q64_read(uint32_t addr,uint8_t *buf,uint16_t n);

    // Read Status Register
    // bit 0 BUSY 1=Write in progress
    // bit 1 WEL  1=Write Enabled
    // bit 2 BP0  block write protection
    // bit 3 BP1  block write protection
    // bit 4 BP2  block write protection
    // bit 5 BP3  block write protection
    // bit 6 SEC  1=Security ID space locked
    // bit 7 BPL  1=BP0..BP3 are read-only, 0=r/w
    uint8_t readStatus();
    void    writeStatusReg(int addr); // Write Status Register

    void    writeEnable();            // Write Enable
    void    writeDisable();           // Write Disable

    void    writeSecurityReg(int addr);

    uint8_t wait_while_busy(void);

    uint8_t readByte(int32_t addr);
    bool    read(uint32_t addr, uint8_t* dst, uint32_t len);

    void    hsread(uint32_t addr, uint8_t* dst, uint32_t len, int frequency);


    uint8_t readSFDP(int addr);

    void    sector_erase_4k(uint32_t addr);
    void    block_erase_32k(uint32_t addr);
    void    block_erase_64k(uint32_t addr);
    void    chip_erase();

    bool    page_program(uint32_t addr, uint8_t* write_buffer, uint8_t len);

    bool    write(int32_t addr, uint8_t* write_buffer, int32_t len);

    void    writeArray(uint32_t address, uint8_t* pData, uint32_t arrayLength);
    void    readArray(uint32_t address, uint8_t* pData, uint32_t arrayLength);


};