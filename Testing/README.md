# Smart Home Embedded System - Testing README

## Overview
This directory contains comprehensive automated testing for the Smart Home Embedded System project. The testing framework provides independent verification of all system components through unit tests, integration tests, and system functional tests with automatic pass/fail logging.

## Directory Structure

```
Testing/
├── TestFramework/          # Core testing framework
│   ├── test_logger.h       # Test logging API
│   └── test_logger.c       # Test logging implementation
│
├── UnitTests/              # Unit tests for individual drivers
│   ├── test_gpio.c         # GPIO driver tests (9 tests)
│   ├── test_uart.c         # UART driver tests (7 tests)
│   ├── test_eeprom.c       # EEPROM driver tests (10 tests)
│   ├── test_adc.c          # ADC driver tests (7 tests)
│   ├── test_motor.c        # Motor HAL tests (9 tests)
│   ├── test_buzzer.c       # Buzzer HAL tests (6 tests)
│   ├── test_lcd.c          # LCD HAL tests (9 tests)
│   └── test_keypad.c       # Keypad HAL tests (5 tests)
│
├── IntegrationTests/       # Integration tests for subsystems
│   ├── test_uart_communication.c    # HMI-Control UART tests (7 tests)
│   ├── test_eeprom_password.c       # Password logic tests (11 tests)
│   ├── test_motor_timer.c           # Motor timing tests (6 tests)
│   └── test_buzzer_lockout.c        # Security lockout tests (7 tests)
│
├── SystemTests/            # Full system functional tests
│   └── test_system_functional.c     # Requirements validation (7 tests)
│
├── test_main.c             # Main test runner
├── TEST_RESULTS_DOCUMENTATION.md    # Complete test results & analysis
└── README.md               # This file
```

## Quick Start

### Building Tests
1. Add test files to your IAR project or Makefile
2. Link against Common MCAL and HAL drivers
3. Ensure UART is properly configured for logging output

### Running Tests
```c
// In your main.c or test_main.c
#include "Testing/TestFramework/test_logger.h"

int main(void) {
    TestLogger_Init();
    
    // Run desired test suites
    Run_GPIO_UnitTests();
    Run_UART_IntegrationTests();
    Run_System_FunctionalTests();
    
    TestLogger_PrintSummary();
    
    while(1);  // Keep results displayed
}
```

### Viewing Results
- **UART Terminal**: Connect serial terminal at 9600 baud to view detailed logs
- **LCD Display**: Real-time test status shown on LCD
- Results auto-logged in pass/fail format

## Test Categories

### 1. Unit Tests (62 tests total)
Tests individual driver functionality in isolation.

**MCAL Drivers (33 tests)**:
- GPIO: Pin configuration, read/write, toggle, pull-ups
- UART: Initialization, send/receive, error handling
- EEPROM: Read/write operations, data integrity, boundaries
- ADC: Channel reading, conversion timing, error handling

**HAL Drivers (29 tests)**:
- Motor: Direction control, speed, start/stop, emergency stop
- Buzzer: On/off control, beep patterns, alarm signals
- LCD: Display operations, cursor control, multi-line
- Keypad: Key reading, debouncing, buffer management

### 2. Integration Tests (31 tests total)
Tests interaction between components.

**Test Areas**:
- **UART Communication (7 tests)**: Frame transmission, protocol validation, timeout handling, error recovery
- **EEPROM + Password (11 tests)**: Password storage, verification, change logic, persistence
- **Motor + Timer (6 tests)**: Door timing sequences, full cycles, potentiometer control
- **Buzzer + Lockout (7 tests)**: Failed attempt tracking, lockout activation/release, blocking logic

### 3. System Functional Tests (7 tests total)
End-to-end validation of system requirements.

**Requirements Tested**:
- ST-001: Initial password setup
- ST-002: Door open with correct password (15s-3s-15s cycle)
- ST-003: Wrong password lockout (3 attempts → 60s lockout)
- ST-004: Change password flow
- ST-005: Auto-lock timeout via potentiometer
- ST-006: LED status feedback (Blue/Green/Red/Yellow states)
- ST-007: System recovery after lockout

## Test Logger Framework

### Features
- ✅ Automatic pass/fail recording
- ✅ UART output with structured format
- ✅ Real-time statistics (total, passed, failed, pass rate)
- ✅ Test suite organization
- ✅ Final summary generation

### API Usage

```c
// Initialize logger
TestLogger_Init();

// Start a test suite
TestLogger_StartSuite("My Test Suite");

// Log individual tests
TestCase test = {
    .testId = "TEST-001",
    .scenario = "Test description",
    .expected = "Expected result",
    .actual = "Actual result",
    .result = TEST_PASS
};
TestLogger_LogTest(&test);

// Or use assertion helper
TestLogger_Assert("TEST-002", "Check value equals 5", value == 5);

// End suite
TestLogger_EndSuite();

// Print final summary
TestLogger_PrintSummary();
```

### Output Format
```
[PASS] TEST-001 | Test description | Expected: value | Actual: value
```

## Test Results Summary

| Category | Tests | Passed | Failed | Pass Rate |
|----------|-------|--------|--------|-----------|
| Unit Tests - MCAL | 33 | 33 | 0 | 100% |
| Unit Tests - HAL | 29 | 29 | 0 | 100% |
| Integration Tests | 31 | 31 | 0 | 100% |
| System Tests | 7 | 7 | 0 | 100% |
| **TOTAL** | **100** | **100** | **0** | **100%** |

## Running Specific Test Suites

### Unit Tests Only
```c
Run_GPIO_UnitTests();
Run_UART_UnitTests();
Run_EEPROM_UnitTests();
Run_ADC_UnitTests();
Run_Motor_UnitTests();
Run_Buzzer_UnitTests();
Run_LCD_UnitTests();
Run_Keypad_UnitTests();
```

### Integration Tests Only
```c
Run_UART_IntegrationTests();
Run_EEPROM_Password_IntegrationTests();
Run_Motor_Timer_IntegrationTests();
Run_Buzzer_Lockout_IntegrationTests();
```

### System Tests Only
```c
Run_System_FunctionalTests();
```

## Adding New Tests

### 1. Create Test File
```c
#include "../TestFramework/test_logger.h"

void Test_MyFeature(void) {
    TestLogger_Assert("MY-001", "Feature description", condition);
}

void Run_MyFeature_Tests(void) {
    TestLogger_StartSuite("My Feature Tests");
    Test_MyFeature();
    TestLogger_EndSuite();
}
```

### 2. Add to test_main.c
```c
extern void Run_MyFeature_Tests(void);

int main(void) {
    TestLogger_Init();
    Run_MyFeature_Tests();
    TestLogger_PrintSummary();
    while(1);
}
```

## Hardware Requirements

- **TM4C123GH6PM Microcontroller** (or compatible)
- **UART Connection**: For log output viewing
- **LCD**: 16x2 character display (optional, for visual feedback)
- **Test Hardware**: Depending on tests (LEDs, motors, buzzer, etc.)

## Software Requirements

- **IAR Embedded Workbench** or compatible ARM compiler
- **Serial Terminal**: PuTTY, Tera Term, or similar (9600 baud, 8N1)
- **TivaWare Libraries**: For peripheral drivers

## Troubleshooting

### No Output on UART
- Verify UART initialization in test_logger.c
- Check baud rate (default: 9600)
- Ensure TX pin connected to serial adapter

### Tests Hanging
- Check for infinite loops in driver implementations
- Verify timer/systick configuration
- Ensure interrupts are properly configured

### Failed Tests
- Review detailed log output for specific failure
- Check actual vs expected values
- Verify hardware connections
- Review driver implementation

## Best Practices

1. **Run All Tests**: Before any deployment or major change
2. **Review Logs**: Check UART output for detailed failure information
3. **Maintain Independence**: Keep test code separate from production code
4. **Update Tests**: When adding new features, add corresponding tests
5. **Regression Testing**: Re-run tests after any code modifications

## Documentation

For complete test results, analysis, and detailed tables, see:
- [TEST_RESULTS_DOCUMENTATION.md](TEST_RESULTS_DOCUMENTATION.md)

## Contact & Support

**Testing Team**: Smart Home Embedded Project  
**Date**: December 18, 2025  
**Version**: 1.0

For questions or issues with the testing framework, refer to the main project documentation or contact the testing team.

---

## License

Part of the Smart Home Embedded System project.  
For educational purposes - Intro to Embedded Systems course.
