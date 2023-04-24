#include "mbed.h"
#include "LocalFileSystem.h"
#include "C12832.h"
#include "LM75B.h"

LocalFileSystem local("local");
C12832 lcd(p5, p7, p6, p8, p11);
LM75B temp_sensor(p28, p27);

float min_temp = 100;
float max_temp = -100;
float avg_temp = 0;
int count = 0;

void logTemperature() {
    FILE *fp;
    while(1) {
        // Read temperature from sensor
        float temp = temp_sensor.temp();
        
        // Log temperature to file
        fp = fopen("/local/temperature.txt", "a");
        if(fp == NULL) {
            printf("Could not open file!\n");
        } else {
            fprintf(fp, "%.2f\n", temp);
            fclose(fp);
        }
        wait(5);
    }
}

void updateStatistics() {
    while(1) {
        FILE *fp = fopen("/local/temperature.txt", "r");
        if(fp == NULL) {
            printf("Could not open file!\n");
        } else {
            float sum_temp = 0;
            int num_temp = 0;
            float temp;
            while(fscanf(fp, "%f", &temp) == 1) {
                sum_temp += temp;
                num_temp++;
                if (temp < min_temp) {
                    min_temp = temp;
                }
                if (temp > max_temp) {
                    max_temp = temp;
                }
            }
            fclose(fp);
            if(num_temp > 0) {
                avg_temp = sum_temp / num_temp;
            }
        }
        wait(5);
    }
}

void displayStatistics() {
    while(1) {
        lcd.cls();
        lcd.locate(0, 0);
        lcd.printf("Min: %.2f", min_temp);
        lcd.locate(0, 10);
        lcd.printf("Max: %.2f", max_temp);
        lcd.locate(0, 20);
        lcd.printf("Avg: %.2f", avg_temp);
        wait(5);
    }
}

int main() {
    Thread thread1;
    Thread thread2;
    Thread thread3;
    
    thread1.start(logTemperature);
    thread2.start(updateStatistics);
    thread3.start(displayStatistics);
    
    while(1) {
        wait(60);
    }
}
