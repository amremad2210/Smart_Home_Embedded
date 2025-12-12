#include "hal_rgb_led.h"
#include "mcal/mcal_gpio.h"   // Use the GPIO driver, not registers

// Which port do the RGB LED pins belong to?
#define RGB_LED_PORT     GPIO_PORTF_BASE

// Which pins are connected to each LED color?
#define RED_LED_PIN      GPIO_PIN_1
#define BLUE_LED_PIN     GPIO_PIN_2
#define GREEN_LED_PIN    GPIO_PIN_3

// These remember the last color you set
static uint8_t saved_red   = 0;
static uint8_t saved_green = 0;
static uint8_t saved_blue  = 0;

void RGB_LED_Init(void)
{
    // Enable the clock for port F so we can use its pins
    MCAL_GPIO_EnablePort(SYSCTL_PERIPH_GPIOF);

    // Configure each LED pin as an output
    MCAL_GPIO_InitPin(RGB_LED_PORT, RED_LED_PIN,   GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    MCAL_GPIO_InitPin(RGB_LED_PORT, GREEN_LED_PIN, GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);
    MCAL_GPIO_InitPin(RGB_LED_PORT, BLUE_LED_PIN,  GPIO_DIR_OUTPUT, GPIO_ATTACH_DEFAULT);

    // Make sure all LEDs start OFF
    RGB_LED_Off();
}

void RGB_LED_SetColor(uint8_t red, uint8_t green, uint8_t blue)
{
    // Save the desired color
    saved_red   = red;
    saved_green = green;
    saved_blue  = blue;

    // Turn each LED ON or OFF using the GPIO driver
    if (red == 1)
        MCAL_GPIO_WritePin(RGB_LED_PORT, RED_LED_PIN, 1);
    else
        MCAL_GPIO_WritePin(RGB_LED_PORT, RED_LED_PIN, 0);

    if (green == 1)
        MCAL_GPIO_WritePin(RGB_LED_PORT, GREEN_LED_PIN, 1);
    else
        MCAL_GPIO_WritePin(RGB_LED_PORT, GREEN_LED_PIN, 0);

    if (blue == 1)
        MCAL_GPIO_WritePin(RGB_LED_PORT, BLUE_LED_PIN, 1);
    else
        MCAL_GPIO_WritePin(RGB_LED_PORT, BLUE_LED_PIN, 0);
}

void RGB_LED_On(void)
{
    // Re-apply the last saved color
    RGB_LED_SetColor(saved_red, saved_green, saved_blue);
}

void RGB_LED_Off(void)
{
    // Turn ALL LED colors OFF
    MCAL_GPIO_WritePin(RGB_LED_PORT, RED_LED_PIN,   0);
    MCAL_GPIO_WritePin(RGB_LED_PORT, GREEN_LED_PIN, 0);
    MCAL_GPIO_WritePin(RGB_LED_PORT, BLUE_LED_PIN,  0);
}