#include "stm32f4xx_hal.h"

// Define sensor and motor pins
#define SENSOR_COUNT 8
uint16_t sensor_pins[SENSOR_COUNT] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
GPIO_TypeDef* sensor_ports[SENSOR_COUNT] = {GPIOA, GPIOA, GPIOA, GPIOA, GPIOB, GPIOB, GPIOB, GPIOB};

#define MOTOR_LEFT_PIN GPIO_PIN_8
#define MOTOR_RIGHT_PIN GPIO_PIN_9
#define MOTOR_PORT GPIOC

// PID parameters
float Kp = 1.0, Ki = 0.0, Kd = 0.0;
float integral = 0, previous_error = 0;

// Motor speed control function
void set_motor_speed(int left_speed, int right_speed) {
    // Implement PWM control for motors here
}

// Read sensor values
uint8_t read_sensors() {
    uint8_t sensor_value = 0;
    for(int i = 0; i < SENSOR_COUNT; i++) {
        if(HAL_GPIO_ReadPin(sensor_ports[i], sensor_pins[i]) == GPIO_PIN_SET) {
            sensor_value |= (1 << i);
        }
    }
    return sensor_value;
}

// Line following logic
void line_following() {
    uint8_t sensors = read_sensors();
    int error = 0;
    
    // Calculate error based on sensor input
    for(int i = 0; i < SENSOR_COUNT; i++) {
        if(sensors & (1 << i)) {
            error += (i - (SENSOR_COUNT / 2));
        }
    }
    
    // PID control
    integral += error;
    float derivative = error - previous_error;
    float control = Kp * error + Ki * integral + Kd * derivative;
    previous_error = error;
    
    // Adjust motor speeds
    int left_speed = 100 - control;
    int right_speed = 100 + control;
    set_motor_speed(left_speed, right_speed);
}

int main(void) {
    // HAL initialization
    HAL_Init();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // Initialize sensor pins as input
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // Initialize motor pins as output
    GPIO_InitStruct.Pin = MOTOR_LEFT_PIN | MOTOR_RIGHT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR_PORT, &GPIO_InitStruct);
    
    // Main loop
    while (1) {
        line_following();
        HAL_Delay(100);
    }
}