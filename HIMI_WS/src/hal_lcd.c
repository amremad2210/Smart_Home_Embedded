#include "hal/hal_lcd.h"
#include "mcal/mcal_i2c.h"
#include "mcal/mcal_systick.h"
#include <stdint.h> 
#include "Types.h"

// I2C Address (Try 0x27 first, if that fails try 0x3F)
#define LCD_ADDRESS   0x27

// Helper Delay (Timing is critical for Init!)
void Delay_us(uint32_t n) {
    uint32_t i, j;
    for(i=0; i<n; i++) for(j=0; j<3; j++);
}

/* ======================================================= */
/* HELPER: SEND ONLY 4 BITS (Used for Init Sync)           */
/* ======================================================= */
void Lcd_Write_Nibble(uint8_t nibble, uint8_t rs_mode)
{
    uint8_t data_packet = 0;
    uint8_t backlight = 0x08; // 0x08 = Backlight ON
    
    // Construct packet: Nibble + Backlight + RS + Enable(1)
    data_packet = (nibble & 0xF0) | backlight | rs_mode | 0x04; 
    I2C0_WriteByte(LCD_ADDRESS, data_packet);
    Delay_us(100); // Enable Pulse Width

    // Pulse Enable OFF
    data_packet = (nibble & 0xF0) | backlight | rs_mode | 0x00;
    I2C0_WriteByte(LCD_ADDRESS, data_packet);
    Delay_us(100); // Wait for LCD to process
}

/* ======================================================= */
/* HELPER: SEND FULL BYTE (2 Nibbles)                      */
/* ======================================================= */
void Lcd_WriteI2C(uint8_t data, uint8_t rs_mode)
{
    // 1. Send High Nibble (Bits 4-7)
    // We pass the data directly; the logic inside Write_Nibble masks it with 0xF0
    Lcd_Write_Nibble(data, rs_mode);

    // 2. Send Low Nibble (Bits 0-3)
    // We shift it left so it occupies the 0xF0 position
    Lcd_Write_Nibble(data << 4, rs_mode);
}

/* ======================================================= */
/* COMMANDS & DATA                                         */
/* ======================================================= */
void Lcd_SendCommand(uint8_t command)
{
    Lcd_WriteI2C(command, 0x00); // RS = 0
}

void Lcd_DisplayCharacter(uint8_t data)
{
    Lcd_WriteI2C(data, 0x01); // RS = 1
}

void Lcd_DisplayString(const char *Str)
{
    uint8_t i = 0;
    while(Str[i] != '\0') {
        Lcd_DisplayCharacter(Str[i]);
        i++;
    }
}

void Lcd_GoToRowColumn(uint8_t row, uint8_t col)
{
    uint8_t Address;
    switch(row) {
        case 0: Address = 0x00 + col; break;
        case 1: Address = 0x40 + col; break;
    }
    Lcd_SendCommand(Address | 0x80);
}

void Lcd_Clear(void)
{
    Lcd_SendCommand(0x01);
    Delay_us(2000); // Clear command is SLOW!
}

/* ======================================================= */
/* ROBUST INITIALIZATION (The Fix)                         */
/* ======================================================= */
void Lcd_Init(void)
{
    I2C0_Init();     // Start I2C Bus
    Delay_us(50000); // Wait >40ms after power up

    // --- STEP 1: RESET SEQUENCE (Send 0x30 three times) ---
    // Note: We use Write_Nibble, NOT SendCommand.
    // Sending a full byte here causes "Gibberish" sync errors.
    
    Lcd_Write_Nibble(0x30, 0); 
    Delay_us(5000); // Wait >4.1ms
    
    Lcd_Write_Nibble(0x30, 0); 
    Delay_us(200);  // Wait >100us
    
    Lcd_Write_Nibble(0x30, 0); 
    Delay_us(200);
    
    // --- STEP 2: SWITCH TO 4-BIT MODE ---
    Lcd_Write_Nibble(0x20, 0); // Send 0x20 (Set 4-bit)
    Delay_us(2000);

    // --- STEP 3: CONFIGURE LCD (Now safe to use SendCommand) ---
    Lcd_SendCommand(0x28); // Function Set: 4-bit, 2 Line, 5x8 Dots
    Lcd_SendCommand(0x08); // Display OFF
    Lcd_SendCommand(0x01); // Clear Display
    Delay_us(2000);        // Clear is slow
    Lcd_SendCommand(0x06); // Entry Mode: Auto Increment
    
    // --- STEP 4: TURN ON DISPLAY ---
    Lcd_SendCommand(0x0C); // Display ON, Cursor OFF
}