#include "mbed.h"
#include "W25Q64FV.h"
#include "Hexi_OLED_SSD1351.h"
#include "string.h"

DigitalOut led1(LED1);

Serial pc(USBTX, USBRX);

/* Instantiate the SSD1351 OLED Driver */
/* (MOSI,SCLK,POWER,CS,RST,DC) */
SSD1351 oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15);

/**

    MOSI: PTD6
    MISO: PTD7
    SCL : PTD5
    CS : PTD4
    FREQ : 1000 000 HZ

**/
W25Q64FV flash(PTD6, PTD7, PTD5, PTD4, 3000000);

void dump(uint8_t *dt, uint32_t n);

// main() runs in its own thread in the OS
int main()
{
    uint8_t buf[256] = {};
    uint8_t w_data[16]= {};
    uint16_t n;           // 取得データ数

//    mau_id = flash.Id();
//    pc.printf("%d\n",mau_id);
//    pc.printf("%x\n",mau_id);

    flash.W25Q64_readManufacturer(buf);
    pc.printf("JEDEC ID : ");
    for (uint8_t i=0; i<5; i++) {
        pc.printf("%x", buf[i]);
        pc.printf(" ");
    }
    pc.printf("\n");

    for (uint8_t i=0; i < 16; i++) {
        w_data[i]='A'+i;
    }
    n = flash.W25Q64_pageWrite(0, 10, w_data, 16);
    pc.printf("page Write(0,10,d,16byte) : n = ");
    pc.printf("%d \n",n);
    memset(buf,0,256); // buf 시작 주소부터 256개를 모두 0으로 세트한다.

    n = flash.W25Q64_read(0,buf,256);
    dump(buf,256);

    while (true) {

        led1 = !led1;
        wait(0.5);
    }
}

//
// 書込みデータのダンプリスト
// dt(in) : データ格納先頭アドレス
// n(in)  : 表示データ数
//
void dump(uint8_t *dt, uint32_t n)
{

    uint32_t sz;
    char buf[64];
    uint16_t clm = 0;
    uint8_t data;
    uint8_t sum;
    uint8_t vsum[16];
    uint8_t total =0;
    uint32_t saddr =0;
    uint32_t eaddr =n-1;
    sz = eaddr - saddr;

    pc.printf("----------------------------------------------------------\n");
    for (uint16_t i=0; i<16; i++) {
        vsum[i]=0;
    }
    for (uint32_t addr = saddr; addr <= eaddr; addr++) {
        data = dt[addr];
        if (clm == 0) {
            sum =0;
            sprintf(buf,"%05lx: ",addr);
            pc.printf("%s",buf);
        }

        sum += data; // sum = sum + data
        vsum[addr % 16]+=data;

        sprintf(buf,"%02x ",data);
        pc.printf("%s",buf);
        
        clm++;
        
        if (clm == 16) {
            sprintf(buf,"|%02x ",sum);
            pc.printf("%s",buf);
            pc.printf("\n");
            clm = 0;
        }
    }
    pc.printf("----------------------------------------------------------\n");
    pc.printf("       ");
    for (uint16_t i=0; i<16; i++) {
        total+=vsum[i];
        sprintf(buf,"%02x ",vsum[i]);
        pc.printf("%s",buf);
    }
    sprintf(buf,"|%02x ",total);
    pc.printf("%s",buf);
    pc.printf("");
    pc.printf("");
}