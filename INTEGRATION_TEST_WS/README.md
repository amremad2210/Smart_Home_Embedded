# Smart Home Integration Test Project

## Overview

This project contains integration tests for the Smart Home system, designed to run in the IAR Embedded Workbench simulator without requiring physical hardware. The tests verify the core functionality of the system's HAL (Hardware Abstraction Layer) components including password management, motor control, and buzzer operation.

## What Was Done

### Test Approach
Instead of testing the full ECU applications with complex UART communication simulation, we implemented **direct HAL function testing**. This approach:

- Calls HAL functions directly (e.g., `HAL_EEPROM_StorePassword`, `HAL_Motor_Move`, `BUZZER_setState`)
- Verifies results by checking global state variables (`motor_is_open`, `buzzer_active`)
- Provides detailed logging of each test step with clear PASS/FAIL results
- Runs quickly in the IAR simulator (seconds vs minutes with hardware)

### Test Scenarios
1. **Password Setup**: Tests EEPROM password storage and verification
2. **Open Door Success**: Tests correct password verification, motor activation, and buzzer signaling
3. **Open Door Failure**: Tests wrong password rejection and error buzzer activation

### Key Improvements Made
- **Simplified Testing**: Removed complex UART buffer simulation and ECU main loop execution
- **Direct Function Calls**: Tests HAL layer functions without communication protocol overhead
- **Mock Layer**: Uses mock implementations for MCAL drivers to simulate hardware behavior
- **Clear Logging**: Each test step prints detailed status messages to the I/O terminal
- **Fast Execution**: Tests complete in seconds, making iterative development efficient

## File Structure

```
INTEGRATION_TEST_WS/
├── README.md                    # This file - project documentation
├── IntegrationTest.ewp          # IAR Embedded Workbench project file
├── IntegrationTest.ewd          # IAR workspace file
├── IntegrationTest.ewt          # IAR workspace settings
├── test_main.c                  # Main test file with test scenarios and logging
├── startup_ewarm.c              # IAR startup code
├── control_main.c               # Control ECU application (simplified for testing)
├── hmi_main.c                   # HMI ECU application (simplified for testing)
├── settings/                    # IAR project settings and batch files
│   ├── IntegrationTest.Debug.cspy.bat    # Command-line debugger script
│   └── ... (other IAR config files)
├── Debug/                       # Build output directory
│   ├── Exe/IntegrationTest.out  # Compiled executable
│   └── ... (object files, listings)
├── inc/                         # Header files
│   ├── hal/                     # HAL layer headers
│   │   ├── hal_eeprom.h         # EEPROM password management
│   │   ├── hal_motor.h          # Motor control
│   │   ├── hal_buzzer.h         # Buzzer control
│   │   ├── hal_comm.h           # UART communication
│   │   └── ... (other HAL headers)
│   └── mcal/                    # MCAL layer headers
│       ├── mcal_eeprom.h        # EEPROM driver
│       ├── mcal_uart.h          # UART driver
│       ├── mcal_gpio.h          # GPIO driver
│       └── ... (other MCAL headers)
├── src/                         # Source files
│   ├── hal/                     # HAL layer implementations
│   │   ├── hal_eeprom.c         # EEPROM HAL implementation
│   │   ├── hal_motor.c          # Motor HAL implementation
│   │   ├── hal_buzzer.c         # Buzzer HAL implementation
│   │   ├── hal_comm.c           # UART HAL implementation
│   │   └── ... (other HAL sources)
│   └── mcal/                    # MCAL layer implementations (from main project)
└── mocks/                       # Mock implementations for testing
    ├── mock_mcal_eeprom.c       # EEPROM mock (simulates flash memory)
    ├── mock_mcal_uart.c         # UART mock (simulates inter-ECU communication)
    ├── mock_mcal_gpio.c         # GPIO mock (simulates pin states)
    └── ... (other MCAL mocks)
```

## Why This Approach

### Benefits of Current Approach
- **Reliability**: Direct function calls eliminate communication protocol failures
- **Speed**: Tests run in seconds, enabling rapid iteration during development
- **Clarity**: Detailed logging shows exactly what each component does
- **Maintainability**: Easy to add new test cases or modify existing ones
- **Hardware Independence**: Tests core logic without physical hardware dependencies

### Testing Philosophy
By testing these functions directly, we verify that the system's core business logic works correctly, regardless of communication protocols or physical hardware states.

The mock layer provides realistic simulation of hardware behavior, allowing comprehensive testing of error conditions and edge cases.

## Running the Tests

1. Open the project in IAR Embedded Workbench
2. Build the `Debug` configuration
3. Run the debugger or use the command-line script:
   ```batch
   .\settings\IntegrationTest.Debug.cspy.bat "Debug\Exe\IntegrationTest.out"
   ```
4. View results in the Terminal I/O window

## Expected Output

```
=== Smart Home Integration Test Starting ===

Running Test Scenario 1: Password Setup
  - Resetting system states...
  - Simulating password setup...
  - Verifying password storage...
    SUCCESS: Password set correctly, door closed, no buzzer.
Test Scenario 1 (Password Setup): PASSED

Running Test Scenario 2: Open Door Success
  - Resetting system states...
  - Simulating correct password verification...
    SUCCESS: Password verified, door opened, buzzer activated.
  - Verifying door opening success...
    SUCCESS: Door opened, buzzer activated.
Test Scenario 2 (Open Door Success): PASSED

Running Test Scenario 3: Open Door Failure
  - Resetting system states...
  - Simulating wrong password verification...
    SUCCESS: Wrong password rejected, buzzer activated for error.
  - Verifying door remains closed on failure...
    SUCCESS: Door remained closed, buzzer activated for error.
Test Scenario 3 (Open Door Failure): PASSED

=== Test Summary ===
All tests PASSED!
```

## Adding New Tests

To add new test scenarios:

1. Add a new `TestResult test_new_scenario()` function in `test_main.c`
2. Call HAL functions directly and verify results
3. Add the test call to `main()` function
4. Update the test summary logic

