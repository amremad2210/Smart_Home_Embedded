/*============================================================================
 *  Module      : Control ECU - HMI Protocol Test Vectors
 *  File Name   : test_main_protocol.c
 *  Description : Hard-coded UART frames that the HMI ECU sends to Control ECU.
 *                These match the protocol implemented in CONTROL_WS/main.c
 *                and HIMI_WS/main.c and can be used to unit-test the Control
 *                ECU command handlers and UART driver.
 *
 *  Usage ideas:
 *    - In a host/unit test, copy one of these frames into your mock UART RX
 *      buffer (see setup_uart_rx_data() in test_control_ecu.c) and then call
 *      the relevant handler in main.c.
 *    - On an actual board acting as an HMI emulator, iterate over a frame and
 *      send each byte via HAL_COMM_SendByte().
 *===========================================================================*/

#include <stdint.h>

/*======================================================================
 *  Command bytes (must match CONTROL_WS/main.c and HIMI_WS/main.c)
 *====================================================================*/

#define CMD_SETUP_PASSWORD      'S'
#define CMD_OPEN_DOOR           'O'
#define CMD_CHANGE_PASSWORD     'C'
#define CMD_SET_TIMEOUT         'T'

/*======================================================================
 *  Basic HMI → Control frames
 *====================================================================*/

/* 1) Setup: query if password is needed
 *
 * HMI sends:
 *   'S', 0
 *
 * Control replies:
 *   'Y' if no password stored yet (needs setup)
 *   'N' if a password already exists
 */
const uint8_t HMI_SETUP_QUERY[] = {
    CMD_SETUP_PASSWORD,
    0u
};

/* 2) Setup: set password "12345" (both entries match)
 *
 * HMI sends:
 *   'S',
 *   len1, <len1 bytes of first password>,
 *   len2, <len2 bytes of confirmation password>
 */
const uint8_t HMI_SETUP_PWD_OK[] = {
    CMD_SETUP_PASSWORD,
    5u, '1','2','3','4','5',      /* first password */
    5u, '1','2','3','4','5'       /* confirmation */
};

/* 3) Setup: mismatched passwords "12345" vs "54321"
 *
 * Expected Control response: failure.
 */
const uint8_t HMI_SETUP_PWD_MISMATCH[] = {
    CMD_SETUP_PASSWORD,
    5u, '1','2','3','4','5',
    5u, '5','4','3','2','1'
};

/* 4) Open door with correct password "12345" */
const uint8_t HMI_OPEN_DOOR_OK[] = {
    CMD_OPEN_DOOR,
    5u, '1','2','3','4','5'
};

/* 5) Open door with wrong password "99999"
 *
 * Expected Control response:
 *   - 'N' for a wrong attempt
 *   - After 3 wrong attempts total, 'L' (lockout)
 */
const uint8_t HMI_OPEN_DOOR_WRONG[] = {
    CMD_OPEN_DOOR,
    5u, '9','9','9','9','9'
};

/* 6) Change password: old "12345" → new "54321" (match)
 *
 * Frame:
 *   'C',
 *   oldLen,  <old password>,
 *   newLen1, <new password>,
 *   newLen2, <confirmation>
 */
const uint8_t HMI_CHANGE_PWD_OK[] = {
    CMD_CHANGE_PASSWORD,
    5u, '1','2','3','4','5',      /* old */
    5u, '5','4','3','2','1',      /* new #1 */
    5u, '5','4','3','2','1'       /* new #2 (confirm) */
};

/* 7) Change password: confirmation mismatch for "54321" vs "12345" */
const uint8_t HMI_CHANGE_PWD_MISMATCH[] = {
    CMD_CHANGE_PASSWORD,
    5u, '1','2','3','4','5',      /* old */
    5u, '5','4','3','2','1',      /* new #1 */
    5u, '1','2','3','4','5'       /* new #2 (wrong confirm) */
};

/* 8) Set timeout to 15 seconds with correct password "12345"
 *
 * Frame:
 *   'T',
 *   timeoutVal (5..30),
 *   pwdLen,
 *   <pwd bytes>
 */
const uint8_t HMI_SET_TIMEOUT_15_OK[] = {
    CMD_SET_TIMEOUT,
    15u,                          /* timeout value 5..30 seconds */
    5u, '1','2','3','4','5'       /* password length + data */
};

/* 9) Set timeout with invalid value (e.g. 3 seconds) */
const uint8_t HMI_SET_TIMEOUT_INVALID_VALUE[] = {
    CMD_SET_TIMEOUT,
    3u,                           /* invalid timeout (< 5) */
    5u, '1','2','3','4','5'
};

/* 10) Set timeout with wrong password for 20 seconds */
const uint8_t HMI_SET_TIMEOUT_WRONG_PWD[] = {
    CMD_SET_TIMEOUT,
    20u,                          /* valid timeout */
    5u, '9','9','9','9','9'       /* wrong password */
};

/*======================================================================
 *  Optional lookup table (handy in unit tests)
 *====================================================================*/

typedef struct
{
    const char   *name;
    const uint8_t *data;
    uint32_t      length;
} HmiFrame;

const HmiFrame g_hmiTestFrames[] = {
    { "SETUP_QUERY",              HMI_SETUP_QUERY,              sizeof(HMI_SETUP_QUERY) },
    { "SETUP_PWD_OK",             HMI_SETUP_PWD_OK,             sizeof(HMI_SETUP_PWD_OK) },
    { "SETUP_PWD_MISMATCH",       HMI_SETUP_PWD_MISMATCH,       sizeof(HMI_SETUP_PWD_MISMATCH) },
    { "OPEN_DOOR_OK",             HMI_OPEN_DOOR_OK,             sizeof(HMI_OPEN_DOOR_OK) },
    { "OPEN_DOOR_WRONG",          HMI_OPEN_DOOR_WRONG,          sizeof(HMI_OPEN_DOOR_WRONG) },
    { "CHANGE_PWD_OK",            HMI_CHANGE_PWD_OK,            sizeof(HMI_CHANGE_PWD_OK) },
    { "CHANGE_PWD_MISMATCH",      HMI_CHANGE_PWD_MISMATCH,      sizeof(HMI_CHANGE_PWD_MISMATCH) },
    { "SET_TIMEOUT_15_OK",        HMI_SET_TIMEOUT_15_OK,        sizeof(HMI_SET_TIMEOUT_15_OK) },
    { "SET_TIMEOUT_INVALID_VALUE",HMI_SET_TIMEOUT_INVALID_VALUE,sizeof(HMI_SET_TIMEOUT_INVALID_VALUE) },
    { "SET_TIMEOUT_WRONG_PWD",    HMI_SET_TIMEOUT_WRONG_PWD,    sizeof(HMI_SET_TIMEOUT_WRONG_PWD) }
};

const uint32_t g_hmiTestFrameCount = sizeof(g_hmiTestFrames) / sizeof(g_hmiTestFrames[0]);


