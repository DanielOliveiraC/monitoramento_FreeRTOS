#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

// Define the pin for the LED
const uint led_pin = 12;

// Define the pin for the button
#define button_pin 5

// Define variables to store the state of the button for send to process task
int button_state = 0;


// Define the task for read button and set button state
void vButtonTask(){
    for (;;) {
        if (!gpio_get(button_pin)) {
            button_state = 1;
        } else {
            button_state = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Define task for controll the led
void vLedTask(void *pvParameters){
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        gpio_put(led_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(led_pin, 0);
        vTaskDelay(100);
    }
}

// Define the task for process the button state
void vProcessTask(void *pvParameters){
    TaskHandle_t *vLedTask_Handle = (TaskHandle_t *)pvParameters;
    for(;;) {
        if (button_state == 1) {
            xTaskNotifyGive(*vLedTask_Handle);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Main function
int main(void) {
    TaskHandle_t vLedTask_Handle = NULL;

    stdio_init_all();
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);
    gpio_pull_up(button_pin);


    xTaskCreate(vButtonTask, "Button Task", 128, NULL, 2, NULL);
    xTaskCreate(vLedTask, "Led Task", 128, NULL, 1, &vLedTask_Handle);
    xTaskCreate(vProcessTask, "Process Task", 128, (void*)&vLedTask_Handle, 1, NULL);

    vTaskStartScheduler();
    return 0;
}