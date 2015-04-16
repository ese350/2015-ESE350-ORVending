#include "mbed.h"
#include "TextLCD.h"
 
// Host PC Communication channels
Serial pc(USBTX, USBRX); // tx, rx
 
// I2C Communication
I2C i2c_lcd(p28,p27); // SDA, SCL

TextLCD_I2C lcd_1(&i2c_lcd, MCP23008_SA0, TextLCD::LCD20x4, TextLCD::HD44780);
TextLCD_I2C lcd_2(&i2c_lcd, MCP23008_SA1, TextLCD::LCD20x4, TextLCD::HD44780);
void setup_lcds(void){
    lcd_1.setCursor(TextLCD::CurOff_BlkOff);
    lcd_1.setBacklight(TextLCD::LightOn);
    lcd_2.setCursor(TextLCD::CurOff_BlkOff);
    lcd_2.setBacklight(TextLCD::LightOn);
}
char strings_1[4][21];
char strings_2[4][21];

void print_to_monitor(int monitor){
    if(monitor == 1){
        lcd_1.locate(0,0);
        lcd_1.printf(strings_1[0]);
        lcd_1.locate(0,1);
        lcd_1.printf(strings_1[1]);
        lcd_1.locate(0,2);
        lcd_1.printf(strings_1[2]);
        lcd_1.locate(0,3);
        lcd_1.printf(strings_1[3]);
    } else {
        lcd_2.locate(0,0);
        lcd_2.printf(strings_2[0]);
        lcd_2.locate(0,1);
        lcd_2.printf(strings_2[1]);
        lcd_2.locate(0,2);
        lcd_2.printf(strings_2[2]);
        lcd_2.locate(0,3);
        lcd_2.printf(strings_2[3]);
    }
}
int main() {
    setup_lcds();
    strcpy(strings_1[0], "test1,0");
    strcpy(strings_1[1], "garbage");
    strcpy(strings_1[2], "   test");
    strcpy(strings_1[3], "hello");
    strcpy(strings_2[0], "2test1,0");
    strcpy(strings_2[1], "2garbage");
    strcpy(strings_2[2], "2   test");
    strcpy(strings_2[3], "2hello");
    print_to_monitor(1);
    print_to_monitor(2);
}