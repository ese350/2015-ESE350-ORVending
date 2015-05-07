#include "mbed.h"
#include "Adafruit_PWMServoDriver.h"
#include "TextLCD.h"

#define THRESHOLD 20000

Serial pc(USBTX, USBRX);
LocalFileSystem local("local");

AnalogIn light_sensor(p15);
Timer t;
Adafruit_PWMServoDriver pwm(p28, p27);

void initServoDriver() {
    pwm.begin();
    //pwm.setPWMFreq(100);  //This dosen't work well because of uncertain clock speed. Use setPrescale().
    pwm.setPrescale(128);    //This value is decided for 10ms interval.
    pwm.setI2Cfreq(400000); //400kHz
}

void setServoPulse(uint8_t n, float pulse) {
    float pulselength = 10000;   // 10,000 us per second
    pulse = 4094 * pulse / pulselength;
    pwm.setPWM(n, 0, pulse);
}

void set_servo(int n, int i) {
  if(i == 0) {
      setServoPulse(n, 150);
  } else if(i == 90) {
     setServoPulse(n, 500); 
  } else {
      setServoPulse(n, 1000);
  }
}
void start_servos(){
    set_servo(0, 180);
    set_servo(1, 180);
    set_servo(2, 180);
    set_servo(3, 180);
    set_servo(4, 180);
    set_servo(5, 180);
    set_servo(6, 180);
    set_servo(7, 180);
    set_servo(8, 90);
    set_servo(9, 90);
    set_servo(10, 90);
    set_servo(11, 90);
    set_servo(12, 90);
    set_servo(13, 90);
    set_servo(14, 90);
    set_servo(15, 90);
}

//takes in 0 to 8
bool dispense_item(int n){
    int servo_1 = 2*n;
    int servo_2 = servo_1 + 1;
    bool dispensed = false;
    int return_val = 180;
    int to_val = 90;
    if(n > 3) {
        return_val = 90;
        to_val = 180;
    }
    set_servo(servo_1, to_val);
    t.reset();
    while(t.read_ms() < 1000) {
        if(light_sensor.read_u16() < THRESHOLD) {
            dispensed =  true;
            pc.printf("Detected Break 1");
        }
    }
    set_servo(servo_1, return_val);
    t.reset();
    while(t.read_ms() < 1000){
        if(light_sensor.read_u16() < THRESHOLD) {
            dispensed =  true;
            pc.printf("Detected Break 2");
        }
    }
    set_servo(servo_2, to_val);
    t.reset();
    while(t.read_ms() < 1000){
        if(light_sensor.read_u16() < THRESHOLD) {
            dispensed =  true;
            pc.printf("Detected Break 3");
        }
    }
    set_servo(servo_2, return_val); 
    t.reset();
    while(t.read_ms() < 1000){
        if(light_sensor.read_u16() < THRESHOLD) {
            dispensed =  true;
            pc.printf("Detected Break 4");
        }
    }
    return dispensed;
}


I2C i2c_lcd = I2C(p28, p27);// SDA, SCL
TextLCD_I2C lcd_1(&i2c_lcd, MCP23008_SA1, TextLCD::LCD20x4, TextLCD::HD44780);
TextLCD_I2C lcd_2(&i2c_lcd, MCP23008_SA2, TextLCD::LCD20x4, TextLCD::HD44780);

char strings_1[4][20];
char strings_2[4][20];

char strings_and_numbers_1[4][21];
char strings_and_numbers_2[4][21];

int inventory[8];

void setup_lcds(void){
    lcd_1.setCursor(TextLCD::CurOff_BlkOff);
    lcd_1.setBacklight(TextLCD::LightOn);
    lcd_2.setCursor(TextLCD::CurOff_BlkOff);
    lcd_2.setBacklight(TextLCD::LightOn);
}

void print_to_monitor(int monitor){
    if(monitor == 1){
        lcd_1.cls();
        lcd_1.locate(0,0);
        lcd_1.printf(strings_and_numbers_1[0]);
        lcd_1.locate(0,1);
        lcd_1.printf(strings_and_numbers_1[1]);
        lcd_1.locate(0,2);
        lcd_1.printf(strings_and_numbers_1[2]);
        lcd_1.locate(0,3);
        lcd_1.printf(strings_and_numbers_1[3]);
    } else {
        lcd_2.cls();
        lcd_2.locate(0,0);
        lcd_2.printf(strings_and_numbers_2[0]);
        lcd_2.locate(0,1);
        lcd_2.printf(strings_and_numbers_2[1]);
        lcd_2.locate(0,2);
        lcd_2.printf(strings_and_numbers_2[2]);
        lcd_2.locate(0,3);
        lcd_2.printf(strings_and_numbers_2[3]);
    }
}

DigitalIn i0(p5);
DigitalIn i1(p6);
DigitalIn i2(p7);
DigitalIn i3(p8);

DigitalIn i4(p17);
DigitalIn i5(p18);
DigitalIn i6(p19);
DigitalIn i7(p20);

DigitalIn revend(p11);
DigitalIn restock(p16);

bool left_restock = false;
bool just_revend = false;


void readfile() {
    char buff[20];
    int temp_inventory;
    FILE *fp = fopen("/local/inv.txt", "r");
    if (fp == NULL ) {
        pc.printf("Could not open file for read operation.\r\n");
    }
    char dub_buff[20];
    int i = 0;
    while(fscanf(fp, "%s %d\n", buff, &temp_inventory) != EOF) {
        inventory[i] = temp_inventory;
        sprintf(dub_buff, "%s %d", buff, temp_inventory);
        if(i < 4){
            strcpy(strings_and_numbers_1[i], dub_buff);    
            strcpy(strings_1[i], buff);
        } else {
            strcpy(strings_and_numbers_2[i-4], dub_buff); 
            strcpy(strings_2[i-4], buff);
        }
        i++;
    }
    fclose(fp);
}

void update_file_and_screens(){
    int i;
    for(i=0; i < 4; i++){
        sprintf(strings_and_numbers_1[i], "%s %d", strings_1[i], inventory[i]);
        pc.printf("Test%s\r\n", strings_and_numbers_1[i]);
        sprintf(strings_and_numbers_2[i], "%s %d", strings_2[i], inventory[i+4]);
        pc.printf("Test%s\r\n", strings_and_numbers_2[i]);      
    }
    print_to_monitor(1);
    print_to_monitor(2);
    FILE *fp = fopen("/local/inv.txt", "w");
    fprintf(fp, "%s\n", strings_and_numbers_1[0]);
    fprintf(fp, "%s\n", strings_and_numbers_1[1]);
    fprintf(fp, "%s\n", strings_and_numbers_1[2]);
    fprintf(fp, "%s\n", strings_and_numbers_1[3]);
    fprintf(fp, "%s\n", strings_and_numbers_2[0]);
    fprintf(fp, "%s\n", strings_and_numbers_2[1]);
    fprintf(fp, "%s\n", strings_and_numbers_2[2]);
    fprintf(fp, "%s\n", strings_and_numbers_2[3]);
    fclose(fp);
}

void show_error(){
    lcd_1.cls();
    lcd_1.locate(0,0);
    lcd_1.printf("ERROR");
    lcd_1.locate(0,1);
    lcd_1.printf("Item not dispensed");
    lcd_1.locate(0,2);
    lcd_1.printf("ERROR");
    lcd_1.locate(0,3);
    lcd_1.printf("Item not dispensed");
    lcd_2.cls();
    lcd_2.locate(0,0);
    lcd_2.printf("ERROR");
    lcd_2.locate(0,1);
    lcd_2.printf("Item not dispensed");
    lcd_2.locate(0,2);
    lcd_2.printf("ERROR");
    lcd_2.locate(0,3);
    lcd_2.printf("Item not dispensed");
}
void show_empty(){
    lcd_1.cls();
    lcd_1.locate(0,0);
    lcd_1.printf("ITEM OUT");
    lcd_1.locate(0,1);
    lcd_1.printf("Item not dispensed");
    lcd_1.locate(0,2);
    lcd_1.printf("ITEM OUT");
    lcd_1.locate(0,3);
    lcd_1.printf("Item not dispensed");
    lcd_2.cls();
    lcd_2.locate(0,0);
    lcd_2.printf("ITEM OUT");
    lcd_2.locate(0,1);
    lcd_2.printf("Item not dispensed");
    lcd_2.locate(0,2);
    lcd_2.printf("ITEM OUT");
    lcd_2.locate(0,3);
    lcd_2.printf("Item not dispensed");
}
void show_restock(){
    lcd_1.cls();
    lcd_1.locate(0,0);
    lcd_1.printf("Entered Restock");
    lcd_1.locate(0,1);
    lcd_1.printf("Entered Restock");
    lcd_1.locate(0,2);
    lcd_1.printf("Entered Restock");
    lcd_1.locate(0,3);
    lcd_1.printf("Entered Restock");
    lcd_2.cls();
    lcd_2.locate(0,0);
    lcd_2.printf("Entered Restock");
    lcd_2.locate(0,1);
    lcd_2.printf("Entered Restock");
    lcd_2.locate(0,2);
    lcd_2.printf("Entered Restock");
    lcd_2.locate(0,3);
    lcd_2.printf("Entered Restock");
}
void show_left_restock(){
    lcd_1.cls();
    lcd_1.locate(0,0);
    lcd_1.printf("Left Restock");
    lcd_1.locate(0,1);
    lcd_1.printf("Left Restock");
    lcd_1.locate(0,2);
    lcd_1.printf("Left Restock");
    lcd_1.locate(0,3);
    lcd_1.printf("Left Restock");
    lcd_2.cls();
    lcd_2.locate(0,0);
    lcd_2.printf("Left Restock");
    lcd_2.locate(0,1);
    lcd_2.printf("Left Restock");
    lcd_2.locate(0,2);
    lcd_2.printf("Left Restock");
    lcd_2.locate(0,3);
    lcd_2.printf("Left Restock");
}

void enter_restock_mode(){
    bool finish_restock_mode = false;
    pc.printf("entered function");
    while(!finish_restock_mode){   
        if(i0) {
            inventory[0] = inventory[0] + 1;
            update_file_and_screens();
        } else if(i1) {
            inventory[1] = inventory[1] + 1;
            update_file_and_screens();
        } else if(i2) {
            inventory[2] = inventory[2] + 1;
            update_file_and_screens();
        } else if(i3) {
            inventory[3] = inventory[3] + 1;
            update_file_and_screens();
        } else if(i4) {
            inventory[4] = inventory[4] + 1;
            update_file_and_screens();
        } else if(i5) {
            inventory[5] = inventory[5] + 1;
            update_file_and_screens();
        } else if(i6) {
            inventory[6] = inventory[6] + 1;
            update_file_and_screens();
        } else if(i7) {
            inventory[7] = inventory[7] + 1;
            update_file_and_screens();
        } else if(restock) {
            pc.printf("exited function");
            finish_restock_mode = true;
            left_restock = true;
        } else {
            continue;
        }
    }
}

int main() {
    

    setup_lcds();
    initServoDriver();
    
    start_servos();
    t.start();
    readfile();
    lcd_1.cls();
    lcd_2.cls();
    print_to_monitor(1);
    print_to_monitor(2);
    bool test = false;
    bool empty = false;
    int dispensed_item_this_round = 0;
    int revend_number = -1;
    
    update_file_and_screens();
    
    /*
    while(1){
        wait(0.5);
        pc.printf("i0: %d\r\n", i0.read());
        pc.printf("i1: %d\r\n", i1.read());
        pc.printf("i2: %d\r\n", i2.read());
        pc.printf("i3: %d\r\n", i3.read());
        pc.printf("i4: %d\r\n", i4.read());
        pc.printf("i5: %d\r\n", i5.read());
        pc.printf("i6: %d\r\n", i6.read());
        pc.printf("i7: %d\r\n", i7.read());
    */
    
    while(1){
        if(i0) {
            if(inventory[0] > 0) {
                test = dispense_item(0);
                dispensed_item_this_round = 0;
                revend_number = 0;
            } else {
                pc.printf("0");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(i1) {
            if(inventory[1] > 0) {
                test = dispense_item(1);
                dispensed_item_this_round = 1;
                revend_number = 1;
            } else {
                pc.printf("1");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(i2) {
            if(inventory[2] > 0) {
                test = dispense_item(2);
                dispensed_item_this_round = 2;
                revend_number = 2;
            } else {
                pc.printf("2");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(i3) {
            if(inventory[3] > 0) {
                test = dispense_item(3);
                dispensed_item_this_round = 3;
                revend_number = 3;
            } else {
                pc.printf("3");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(i4) {
            if(inventory[4] > 0) {
                test = dispense_item(4);
                dispensed_item_this_round = 4;
                revend_number = 4;
            } else {
                pc.printf("4");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(i5) {
            if(inventory[5] > 0) {
                test = dispense_item(5);
                dispensed_item_this_round = 5;
                revend_number = 5;
            } else {
                pc.printf("5");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(i6) {
            if(inventory[6] > 0) {
                test = dispense_item(6);
                dispensed_item_this_round = 6;
                revend_number = 6;
            } else {
                pc.printf("6");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(i7) {
            if(inventory[7] > 0) {
                test = dispense_item(7);
                dispensed_item_this_round = 7;
                revend_number = 7;
            } else {
                pc.printf("7");
                show_empty();
                wait(3.0);
                empty = true;
            }
        } else if(revend) {
            pc.printf("rev");
            if(revend_number >=0 && revend_number < 8){
                test = dispense_item(revend_number);
                dispensed_item_this_round = revend_number;
            }
            just_revend = true;
        } else if(restock) {
            show_restock();
            wait(2.0);
            update_file_and_screens();
            enter_restock_mode();
        } else {
            continue;
        }
        
        if(test && !empty){
            inventory[dispensed_item_this_round] = 
                inventory[dispensed_item_this_round] - 1;
            update_file_and_screens();
            test = false;
            empty = false;
        } else if(!empty && (!left_restock && !just_revend)) {
            pc.printf("Error, item not vended");
            show_error();
            wait(3.0);
            update_file_and_screens();
            left_restock = false;
        } else if(left_restock) {
            show_left_restock();
            wait(1.0);
            update_file_and_screens();
            left_restock = false;
        } else {
            update_file_and_screens();
        }
        empty = false;
        just_revend = false;
        left_restock = false;
        
    } 
    
    
}