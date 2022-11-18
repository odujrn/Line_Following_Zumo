#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include <stdlib.h>
#define BLACK 1
#define WHITE 0
#include <stdbool.h>

#if 1//Zumo project line following done
void sensors_init(void){
    Ultra_Start();
    reflectance_set_threshold(15000, 15000, 15000, 15000, 15000, 15000);
    reflectance_start();
    IR_Start();
    motor_start();
    motor_forward(0,0);
    IR_flush();
}
void get_start_time(void){
    TickType_t start;
    motor_forward(0,0);
    print_mqtt("Zumo101/ready", "line");
	IR_wait();
    start = xTaskGetTickCount();
    printf("Start time: %d\n", (int) start);
    print_mqtt("Zumo101/start", "%d",(int) start);
    
}
void get_end_time(void){
    TickType_t end;
    motor_forward(0,0);        
    motor_stop();
    end = xTaskGetTickCount();
    print_mqtt("Zumo101/stop", "%d", end);
}
void follow_line(struct sensors_ dig){
    if ((dig.R1 == 1) && (dig.R2 == 1)){
        /*Turn left;*/
        motor_turn(80,0, 10);
    }
    else if ((dig.L1 == 1) && (dig.L2 == 1)){
        /*Turn right;*/
        motor_turn(0,80, 10);
    }
    else if ((dig.R2 == 1) && (dig.R3 ==1)){
        /*Hard turn left;*/
        motor_turn(150,0, 30);
    }
    else if ((dig.L2 == 1) && (dig.L3 ==1)){
        /*Hard turn right;*/
        motor_turn(0,150, 20);
    }
    else if (dig.R3 == 1 ){
        /*Extra hard turn left;*/
        motor_turn(255,0, 25);
    }
    else if (dig.L3 == 1){
        /*Extra hard turn right;*/
         motor_turn(0,255,25);
    }
}
void zmain(void){
    printf("place robot in start\n");
    TickType_t end;
    TickType_t start;
    struct sensors_ dig;
    sensors_init();
    int count = 0;
    while(SW1_Read() == 1){ //press user button for robot to run
	    vTaskDelay(100);
    }
    while (count <3){
        reflectance_digital(&dig);
        motor_forward(255, 0);
        if ((dig.R3 == 1) && (dig.L3 == 1) ){
            count++;
            printf("Line: %d\n",count);
            if (count == 1){
                get_start_time();   
                start = xTaskGetTickCount();
            }
            while((dig.R3 == 1) && (dig.L3 == 1) && count < 3){
                reflectance_digital(&dig);
                motor_forward(255, 0);
            }
        }
        else {
            follow_line(dig);
        }
        if (count == 3){
            get_end_time();
            end = xTaskGetTickCount();
            TickType_t timeStamp = end - start;
            print_mqtt("Zumo101/time", "%d", timeStamp);
        }
    }
}
#endif 
