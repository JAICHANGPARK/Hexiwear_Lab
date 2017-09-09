/*
    CREATED BY JAICHANGPARK AKA DREAMWALKER
    DATE : 2017. 06. 28
    
*/

#include "mbed.h"
#include "Hexi_OLED_SSD1351.h"
#include "Hexi_KW40Z.h"
#include "string.h"
#include "MTCH6102.h"


DigitalOut led1(LED1);

/**
* HEXIWEAR SERIAL PORT 
    TX : PTD3, USBTX(WITH PC)
    RX : PTD2, USBRX(WITH PC)
**/

Serial serial(USBTX,USBRX,9600); //(TX, RX) 

/*
    HEXIWEAR OLED INTERNAL DISPLAY PIN 
    MOSI(SDI) : PTB22
    SCLK(SCK) : PTB21
    POWER     : PTC13 ( INTERNAL POWER PORT) 
    CS        : PTB20
    RST       : PTE6
    DC        : PTD15 
*/
SSD1351 display(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15); 

/*
    KW40Z INIT (/* Instantiate the Hexi KW40Z Driver (UART TX, UART RX) */ )
    UART4 
    TX : PTE24
    RX : PTE25
*/

KW40Z kw40z_device(PTE24, PTE25);

// main() runs in its own thread in the OS
int main() {
    
    char time_text[20];
    char date_text[25];
    
    // Use default properties
    oled_text_properties_t textProperties = {0};
    display.GetTextProperties(&textProperties); 
    display.FillScreen(COLOR_BLACK); 
    
    textProperties.fontColor = COLOR_WHITE;
    textProperties.alignParam = OLED_TEXT_ALIGN_CENTER;
    display.SetTextProperties(&textProperties);
    
    while (true) {
        
        time_t seconds = time(NULL); 
        const tm *t = localtime(&seconds); // Convert the unix time to actual time
        char* s = "AM"; // The suffix to use for the time of day
        int h = (t->tm_hour) + 18; // The hours
        int year = (t->tm_year) + 1947;
        if (h > 12){ // If it's entering 24/h time, change it to 12/h and add PM
            s = "PM";
            h = h - 12;    
        }
          
        // Format the time
        sprintf(time_text,"%d:%d:%d %s",h, t->tm_min, t->tm_sec, s);  
        sprintf(date_text,"%d-%d-%d",year, (t->tm_mon)+6, (t->tm_mday)+27);
       
        serial.printf(time_text);
        serial.printf("\n");
        serial.printf(date_text);
        serial.printf("\n");
        
        // Display the time on screen
        // oled_status_t SSD1351::TextBox(const uint8_t* text, int8_t xCrd, int8_t yCrd,uint8_t width,uint8_t height)
        display.TextBox((uint8_t *)time_text,2,2, 91, 15); 
        display.TextBox((uint8_t *)date_text,2,16, 91, 15); 
        led1 = !led1;
        wait(0.5);
    }
}

void edited_time(){
    
    char buffer[32];
    time_t init_time = time(NULL);
    strftime(buffer, 32, "%I:%M %p\n", localtime(&init_time));
    printf("Time as a custom formatted string = %s", buffer);
    
}

