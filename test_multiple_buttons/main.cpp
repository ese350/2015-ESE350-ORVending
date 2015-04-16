#include "mbed.h"

#include <string.h>

LocalFileSystem local("local");

DigitalOut o1(p5);
DigitalOut o2(p6);
DigitalOut o3(p7);
DigitalOut o4(p8);
DigitalOut o5(p9);
DigitalOut o6(p10);
DigitalIn i1(p11);

Ticker button_ticker;
Timer t;

Serial pc(USBTX, USBRX);

int driven_row = 0;

void output_to_light_demux(int i){
    if(i & 1){
        o1 = 1;
    } else { 
        o1 = 0;
    }
    if(i & 2){
        o2 = 1;
    } else { 
        o2 = 0;
    }
    if(i & 4){
        o3 = 1;
    } else { 
        o3 = 0;
    }
}

void output_to_button_demux(int i){
    if(i & 1){
        o4 = 1;
    } else { 
        o4 = 0;
    }
    if(i & 2){
        o5 = 1;
    } else { 
        o5 = 0;
    }
    if(i & 4){
        o6 = 1;
    } else { 
        o6 = 0;
    }
}

void tick(){
    driven_row = (++driven_row)%8;
    output_to_button_demux(driven_row);
}
    
int main() {
    pc.baud(9600);
    o1 = 0;
    o2 = 0;
    o3 = 0;
    o4 = 0;
    o5 = 0;
    o6 = 0;
    button_ticker.attach(&tick, 0.005f);
    t.start();
    while(1) {
        if(!i1){
            int val = driven_row;
            output_to_light_demux(val);
            t.reset();
            while(t.read_ms() < 500);
        }
    }
}
