#ifndef PROTOCOL_PC_TO_DEVICE
#define PROTOCOL_PC_TO_DEVICE

#include <stdint.h>

#if defined(_WIN32)
#define PACKED
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
#elif defined(__GNUC__)
#define PACKED   __attribute__((__packed__))
#elif defined (__PICC18__)
#define PACKED
#endif

//  Packet format:
// |    0     |  1   | 2...3  |   4   |  5   | 6.....9 | 10..(10+Lenght-6+1) | (4+Lenght)..(4+Lenght+1) |
// ------------------------------------------------------------------------------------------------------
// | Preamble | Dir. | Lenght | Encr. | Type | Reserv. | Data                | CRC16 CCITT              |
// ------------------------------------------------------------------------------------------------------
#define PREAMBLE                0x7F
#define DIRECTION_TO_DEVICE     0xCC    //  (C)ommand
#define DIRECTION_FROM_DEVICE   0xAA    //  (A)nswer
#define ENCRYPTION_OFF          0x00    //  (O)pen
#define ENCRYPTION_ON           0xEE    //  (E)ncrypted
#define CRC_LEN                 sizeof(uint16_t) // CRC is used for preserve fields: length, encrypted, type, reserved, data
#define MAX_DATA_SIZE           0x64    //  Max length of "data" section = 100b
#define MAX_PACKET_SIZE         (10 + MAX_DATA_SIZE + CRC_LEN)    //  Max total packet size (with preamble and crc) = 10b(meta)+100b(data)+2b(crc)=112bytes

typedef enum {
    //  Device common procedures
    GET_DEVICE_ID               = 0x01,
    GET_STATUS                  = 0x02,

    //  Looking after buttons state (pushed/leaved, installed/unmounted)
    GET_BUTTONS_STATE           = 0x10,

    //  LEDs control
    SET_LEDS_STATE              = 0x20,

    //  Settings
    SET_SETTINGS                = 0x30,
    GET_SETTINGS                = 0x31,
} packet_type_e;

enum device_errors_e {
    ERROR_NONE                  = 0x0000,
    ERROR_OTHER                 = 0x8000
};

typedef struct {
    uint16_t errors;         // bitmask based on enum device_errors_e
    uint32_t rtc;
} PACKED device_status_s;

enum button_state_e {
    BUTTON_UNMOUNTED            = 0x00,
    BUTTON_MOUNTED              = 0x01,
    BUTTON_PRESSED              = 0x02,
    BUTTON_RELEASED             = 0x04,
};

typedef struct {
    uint8_t state;           // bitmask based on enum button_state_s
    uint8_t uid;
    uint8_t pos;
} PACKED button_state_s;

enum led_state_e {
    LED_TURN_OFF                = 0x0000,
    LED_TURN_ON                 = 0x0001,
// Led's state pattern basing on "urgency"
    LED_URGENT                  = 0x0100, // when expected immediate user reaction
    LED_CALM                    = 0x0180, // remind about daily routines
// Led's state pattern basing on "duration"
    LED_START_OPERATION         = 0x0200, // operation is started
    LED_IN_PROGRESS             = 0x0208, // inform user about a long operation execution progress
    LED_STOP_OPERATION          = 0x0280, // operation is finished
// Led's state pattern basing on "finish status"
    LED_SUCCESS                 = 0x0400,
    LED_FAILURE                 = 0x0480,
};

typedef struct {
    uint16_t state;         // enum led_state_e
    uint8_t pos;
} PACKED led_state_s;

//  DATA FIELD's union for any command or answer
typedef union {
    //  Device common procedures
    uint8_t             device_id[8];       // GET_DEVICE_ID
    device_status_s     device_status;      // GET_STATUS

    //  Looking after buttons state (pushed/leaved, installed/unmounted)
    button_state_s      buttons_state[3];   // GET_BUTTONS_STATE

    //  LEDs control
    led_state_s         leds_state[3];      // SET_LEDS_STATE

    //  Settings
    // SET_SETTINGS
    // GET_SETTINGS
} packet_data_u;

//  HEADER format
typedef struct {
    uint8_t     preamble;   //  PREAMBLE
    uint8_t     direction;  //  DIRECTION_TO_DEVICE or DIRECTION_FROM_DEVICE
    uint16_t    length;     //  size of payload: (encrypted, type, reserved, data). 6 ... (65535 - 2) bytes
    uint8_t     encrypted;  //  ENCRYPTION_OFF or ENCRYPTION_ON
    uint8_t     type;       //  enum packet_type_e
    uint32_t    reserved;
} PACKED packet_header;

#define HEADER_LENGTH sizeof(packet_header)

#if defined(_WIN32)
#pragma pack(pop)   /* restore original alignment from stack */
#endif

#endif // PROTOCOL_PC_TO_DEVICE

