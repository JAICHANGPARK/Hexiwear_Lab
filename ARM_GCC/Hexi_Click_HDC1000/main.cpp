#include "mbed.h"
#include "Hexi_KW40Z.h"
#include "Hexi_OLED_SSD1351.h"
#include "HDC1000.h"
#include "string.h"

#define LED_ON      0
#define LED_OFF     1

void StartHaptic(void);
void StopHaptic(void const *n);

DigitalOut  led1(LED1);
DigitalOut  redLed(PTC8);
DigitalOut  greenLed(PTD0);
DigitalOut  blueLed(PTC9);
DigitalOut  haptic(PTB9);

AnalogIn   ain(PTB2);

HDC1000     hdc(PTD9,PTD8);
Serial      pc(USBTX, USBRX);
SSD1351     oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15);  /* (MOSI,SCLK,POWER,CS,RST,DC) */
KW40Z       kw40z_device(PTE24, PTE25);                 /* Instantiate the Hexi KW40Z Driver (UART TX, UART RX) */ 

/* Define timer for haptic feedback */
RtosTimer hapticTimer(StopHaptic, osTimerOnce);

void ButtonRight(void)
{
    StartHaptic();
    
    redLed      = LED_OFF;
    greenLed    = LED_OFF;
    blueLed     = LED_ON;
}

void ButtonLeft(void)
{
//   char t_tmp[10] = {};
//    float tmp = 0.0f;
//    
//    tmp = hdc.conv_c_to_f();
//    
//    
//    /* Get OLED Class Default Text Properties */
//    oled_text_properties_t textProperties = {0};
//    oled.GetTextProperties(&textProperties);    
//
//    /* Turn on the backlight of the OLED Display */
//    oled.DimScreenON();
//    
//    /* Fills the screen with solid black */         
//    oled.FillScreen(COLOR_BLACK);
//    
//    textProperties.fontColor = COLOR_WHITE;
//    oled.SetTextProperties(&textProperties);  
//    
//    sprintf(t_tmp,"%4.1fC",tmp);
//    //sprintf(t_humi,"%4.1f%%",humi);
//    oled.TextBox((uint8_t *)t_tmp,40,30,46,15); //Increase textbox for more digits
//    //oled.TextBox((uint8_t *)t_humi,40,45,46,15); //Increase textbox for more digits

    StartHaptic();
    redLed      = LED_ON;
    greenLed    = LED_ON;
    blueLed     = LED_OFF;
}

void StartHaptic(void)
{
    hapticTimer.start(50);
    haptic = 1;
}

void StopHaptic(void const *n) {
    haptic = 0;
    hapticTimer.stop();
}


// main() runs in its own thread in the OS
int main() {
    
    char text[20] = {0};  /* Text Buffer */ 
    
    char t_temp[10] = {0};
    char t_humi[10] = {0};
    
    float temp = 0.0f, humi = 0.0f;
    
    
    /* Get OLED Class Default Text Properties */
    oled_text_properties_t textProperties = {0};
    oled.GetTextProperties(&textProperties);    

    /* Turn on the backlight of the OLED Display */
    oled.DimScreenON();
    
    /* Fills the screen with solid black */         
    oled.FillScreen(COLOR_BLACK);
    
     /* Change font color to blue */ 
    textProperties.fontColor   = COLOR_BLUE;
    textProperties.alignParam = OLED_TEXT_ALIGN_CENTER;
    oled.SetTextProperties(&textProperties);
    
    /* Display Text at (x=0,y=0) */
    strcpy((char *) text,"HDC1000");
    oled.TextBox((uint8_t *)text,0,0,96,15); //Increase textbox for more digits
    
    textProperties.fontColor = COLOR_WHITE;
    oled.SetTextProperties(&textProperties);  
    
    /* Display Text at (x=7,y=30) */
    strcpy((char *) text,"TEMP :");
    oled.Label((uint8_t *)text,7,30);  
    
    /* Display Text at (x=7,y=45) */
    strcpy((char *) text,"HUMI :");
    oled.Label((uint8_t *)text,7,45);  
    
    /* Display Text at (x=7,y=0) */
    strcpy((char *) text,"F");
    oled.Label((uint8_t *)text,15,81);  
    
    strcpy((char *) text,"C");
    oled.Label((uint8_t *)text,75,81);  
    
    kw40z_device.attach_buttonLeft(&ButtonLeft);
    kw40z_device.attach_buttonRight(&ButtonRight);
    
    while (true) {
        
        pc.printf("analog in read : %4.2f\n", ain.read());
        
        hdc.get();    // Triger conversion
        pc.printf("Temp: %+4.1fC, Humid: %4.1f%%\r\n", hdc.temperature(), hdc.humidity());
        temp = hdc.temperature();
        humi = hdc.humidity();
        sprintf(t_temp,"%4.1fC",temp);
        sprintf(t_humi,"%4.1f%%",humi);
        oled.TextBox((uint8_t *)t_temp,40,30,46,15); //Increase textbox for more digits
        oled.TextBox((uint8_t *)t_humi,40,45,46,15); //Increase textbox for more digits
        
        
        wait(1.0);
        led1 = !led1;
    
    }
}

