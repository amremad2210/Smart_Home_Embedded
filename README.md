
# Door Locker Security System – Dual TM4C123 ECUs

### *Embedded Systems Project – MCAL / HAL / Application Architecture*

This repository contains the embedded software for a **Door Locker Security System** implemented on **two TM4C123 (Tiva C) microcontrollers** communicating over UART.
The system is built using a clean, modular layered architecture (**MCAL → HAL → Application**) and integrates user interfaces, persistent storage, motor control, and security logic.

---

# 📘 Table of Contents

1. [System Overview](#system-overview)
2. [Hardware Architecture](#hardware-architecture)
3. [Software Architecture](#software-architecture)
4. [Project Folder Structure](#project-folder-structure)
5. [TivaWare Integration](#tivaware-integration)
6. [Build & Flash Instructions](#build--flash-instructions)
7. [Git Workflow](#git-workflow)

---

# 1️ System Overview

This system uses a pair of TM4C123 microcontrollers where:

* **HMI_ECU** handles **user interface** components (LCD, keypad, RGB LED, timeout setting).
* **Control_ECU** handles **core control logic**, including EEPROM storage, motor activation, buzzer alarm, and password validation.

Based on the official project description.

---

# 2️ Hardware Architecture

###  **HMI_ECU (User Interface)**

* 16×2 LCD
* 4×4 Keypad
* RGB LED
* Potentiometer (via ADC)
* UART to Control ECU
* SysTick for timing and debouncing

###  **Control_ECU (Control Logic)**

* DC Motor (PWM or GPIO control)
* EEPROM for persistent password & timeout
* Buzzer
* UART to HMI ECU
* GPTM timers
* SysTick for system timing

---

# 3️ Software Architecture

This project uses a clean embedded architecture using layered design:

```
Application (HMI_App / Control_App)
        ↑
      HAL (LCD, keypad, motor, buzzer, RGB LED, comm, pot)
        ↑
     MCAL (GPIO, UART, GPT, ADC, EEPROM, SysTick)
        ↑
   TivaWare (driverlib)  [Vendor Layer]
        ↑
   Hardware (TM4C123 MCU)
```

### Application Layer

* Located in each ECU's `main.c`
* Implements HMI or Control state machine logic
* Calls only HAL functions

###  HAL (Hardware Abstraction Layer)

* Represents actual hardware modules (LCD, keypad, motor, etc.)
* Uses MCAL functions internally

###  MCAL (Microcontroller Abstraction Layer)

* Shared across both ECUs in `Common/`
* Thin wrappers over TivaWare driverlib
* Handles low-level MCU operations:

  * GPIO
  * UART
  * Timers
  * EEPROM
  * ADC
  * SysTick

###  TivaWare Vendor Layer

* Not included inside repository
* Linked using IAR configuration
* Provides driverlib functions like:

  * `GPIOPinWrite()`
  * `UARTConfigSetExpClk()`
  * `SysCtlClockSet()`
  * `EEPROMProgram()`
  * etc.

---

# 4️ Project Folder Structure

```text
Smart_Home_WS/
│
├── Common/
│   ├── inc/
│   │   ├── Types.h
│   │   ├── common_macros.h
│   │   ├── tm4c123gh6pm.h
│   │   ├── system.h
│   │   └── mcal/
│   │       ├── mcal_gpio.h
│   │       ├── mcal_uart.h
│   │       ├── mcal_gpt.h
│   │       ├── mcal_systick.h
│   │       ├── mcal_eeprom.h
│   │       └── mcal_adc.h
│   └── src/
│       ├── system.c
│       └── mcal/
│           ├── mcal_gpio.c
│           ├── mcal_uart.c
│           ├── mcal_gpt.c
│           ├── mcal_systick.c
│           ├── mcal_eeprom.c
│           └── mcal_adc.c
│
├── Control_WS/
│   ├── main.c
│   ├── inc/
│   │   └── hal/
│   │       ├── hal_motor.h
│   │       ├── hal_buzzer.h
│   │       ├── hal_comm.h
│   │       └── hal_eeprom_cfg.h
│   └── src/
│       └── hal/
│           ├── hal_motor.c
│           ├── hal_buzzer.c
│           ├── hal_comm.c
│           └── hal_eeprom_cfg.c
│
├── HMI_WS/
│   ├── main.c
│   ├── inc/
│   │   └── hal/
│   │       ├── hal_lcd.h
│   │       ├── hal_keypad.h
│   │       ├── hal_rgb_led.h
│   │       ├── hal_potentiometer.h
│   │       └── hal_comm.h
│   └── src/
│       └── hal/
│           ├── hal_lcd.c
│           ├── hal_keypad.c
│           ├── hal_rgb_led.c
│           ├── hal_potentiometer.c
│           └── hal_comm.c
│
└── Smart_Home_WS.eww
```

---

# 5️ TivaWare Integration

The project uses **TivaWare (driverlib)** as the vendor MCU support library.

###  We do *not* copy TivaWare inside the repo.

Instead, we configure IAR:

### **IAR → C/C++ Compiler → Additional Include Directories**

```
C:\ti\TivaWare_C_Series-2.2.0.295\inc
C:\ti\TivaWare_C_Series-2.2.0.295\driverlib\inc
```

### **IAR → Linker → Additional Libraries**

```
C:\ti\TivaWare_C_Series-2.2.0.295\driverlib\ewarm\Exe\driverlib.lib
```

### 📹 Video Guide for Linking TivaWare to IAR

A full setup video is available here:

 **[https://garraio-my.sharepoint.com/:f:/p/ahmed_hisham/IgA-lh1B90QuSKyxEtv4-_wAAWXkQYjhPH230mF_V04zHV8?e=XyEKHE](https://garraio-my.sharepoint.com/:f:/p/ahmed_hisham/IgA-lh1B90QuSKyxEtv4-_wAAWXkQYjhPH230mF_V04zHV8?e=XyEKHE)**

---

# 6️ Build & Flash Instructions

###  Build Control ECU

1. Right-click → **Set as Active**
2. Press **F7** to build
3. Download to board using **Download and Debug**

###  Build HMI ECU

Same steps:

1. Set **HMI_WS – Debug** as active
2. Build
3. Flash

---

# 7️ Git Workflow

###  Main Branch

* Must always **compile and run**
* Should remain stable
* Avoid committing directly

###  Creating a Feature Branch

```bash
git checkout main
git pull
git checkout -b feature/<name>
```

###  Committing

```bash
git add .
git commit -m "implement LCD init and clear"
```

###  Push to Remote

```bash
git push -u origin feature/<name>
```

###  Merging via Pull Request

* Create PR from your feature branch
* Merge after review
* Delete the branch


