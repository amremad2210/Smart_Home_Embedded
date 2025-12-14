//hello this is himi

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

#include "mcal/mcal_systick.h"   // adjust path/name if different
#include "hal_lcd.h"

int main(void)
{
    // Configure system clock for TM4C123G (16 MHz crystal, 50 MHz system clock)
    SysCtlClockSet(SYSCTL_SYSDIV_4 |
                   SYSCTL_USE_PLL   |
                   SYSCTL_OSC_MAIN  |
                   SYSCTL_XTAL_16MHZ);

    // Initialize SysTick delay used by LCD driver (MCALSysTickDelayMs)
    MCALSysTick_Init(SysCtlClockGet());   // use your actual init API if name differs

    // Initialize LCD (uses GPIOB pins as defined in hal_lcd.c)
    LCDInit();

    // Clear and print a welcome message
    LCDClear();
    LCDSetCursor(0, 0);
    LCDWriteString("TM4C123G LCD");
    LCDSetCursor(1, 0);
    LCDWriteString("Test Start");

    MCALSysTickDelayMs(1000);

    uint8_t counter = 0;

    while (1)
    {
        // Line 1: static text
        LCDSetCursor(0, 0);
        LCDWriteString("Count: ");

        // Line 1: counter value (0–9)
        LCDSetCursor(0, 7);
        char c = '0' + (counter % 10);
        LCDWriteChar(c);

        // Line 2: cursor / char test
        LCDSetCursor(1, 0);
        LCDWriteString("Cursor@15:* ");
        LCDSetCursor(1, 15);
        LCDWriteChar('*');

        MCALSysTickDelayMs(500);

        // Erase the star to test overwrite
        LCDSetCursor(1, 15);
        LCDWriteChar(' ');

        MCALSysTickDelayMs(500);

        counter++;
        if (counter > 9)
        {
            counter = 0;

            // Test clear and home
            LCDClear();
            LCDSetCursor(0, 0);
            LCDWriteString("Clear/Home OK");
            LCDSetCursor(1, 0);
            LCDWriteString("Restarting...");
            MCALSysTickDelayMs(1000);
        }
    }

    // return 0;  // not reached
}



int main()
{
  return 0;
}
