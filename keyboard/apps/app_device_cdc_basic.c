/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

/** INCLUDES *******************************************************/
#include <system.h>

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <usb.h>

#include <app_led_usb_status.h>
#include <app_device_cdc_basic.h>
#include <usb_config.h>
#include <adc.h>
#include "../../device/protocol_pc_to_device.h"
#include "../../device/crc16.h"

/** VARIABLES ******************************************************/

static button_state_s buttons_state[3];
static uint8_t readBuffer[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer[CDC_DATA_IN_EP_SIZE];

/*********************************************************************
* Function: void APP_DeviceCDCBasicDemoInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCBasicDemoInitialize()
{
    CDCInitEP();

    
    line_coding.bCharFormat = 0;
    line_coding.bDataBits = 8;
    line_coding.bParityType = 0;
    line_coding.dwDTERate = 9600;

    memset(buttons_state, 0x00, 3 * sizeof(button_state_s));
    buttons_state[0].pos = 0;
    buttons_state[0].state = BUTTON_UNMOUNTED | BUTTON_RELEASED;
    buttons_state[1].pos = 1;
    buttons_state[1].state = BUTTON_UNMOUNTED | BUTTON_RELEASED;
    buttons_state[2].pos = 2;
    buttons_state[2].state = BUTTON_UNMOUNTED | BUTTON_RELEASED;
}

void assemblyPacket(packet_type_e command, const packet_data_u *data, uint8_t packet[MAX_PACKET_SIZE], uint8_t *packetSize)
{
    //  Fill header
    packet_header *header = (packet_header *)packet;
    memset(header, 0x00, HEADER_LENGTH);

    header->preamble = PREAMBLE;
    header->direction = DIRECTION_FROM_DEVICE;
    header->encrypted = ENCRYPTION_OFF;
    header->type = command;
    header->reserved = 0xFFFFFFFF;

    //  Copy data in packet
    uint8_t *dataField = &packet[10];
    unsigned int dataFieldSize = 0;

    switch (command) {
    case GET_DEVICE_ID:
        dataFieldSize = 8;
        memcpy(dataField, data->device_id, dataFieldSize);
        break;
    case GET_BUTTONS_STATE:
        dataFieldSize = 3 * sizeof(button_state_s);
        memcpy(dataField, data->buttons_state, dataFieldSize);
        break;
    default:
        *packetSize = 0;
        return;
    }

    //  Calculate packet size
    header->length = 6 + dataFieldSize;
    *packetSize = 4 + header->length + CRC_LEN;
    
    //  Calculate CRC over: length, encrypted, type, reserved, data
    uint16_t crc;
    crc = crc16_ccitt(&packet[2], (2 + header->length));
    memcpy(packet + HEADER_LENGTH + dataFieldSize, &crc, CRC_LEN);
}

//  Return true if another packet may present in Incoming buffer
bool parsePacket(const uint8_t *rawData, uint8_t rawDataSize, packet_type_e *type, uint8_t payload[MAX_DATA_SIZE], uint8_t *payloadSize)
{
    *type = UNKNOWN_PACKET;
    *payloadSize = 0;
    
    //  Verify prefix of packet
    if (rawDataSize < 4) {
        return false;
    }
    packet_header *header = (packet_header *)rawData;
    if ((header->preamble != PREAMBLE) || (header->direction != DIRECTION_TO_DEVICE)) {
        return true;
    }

    //  Wait until get full packet
    uint16_t dataSize = header->length;
    if (dataSize > MAX_DATA_SIZE) {
        return true;
    }
    if (rawDataSize < (4 + dataSize + CRC_LEN)) {
        return false;
    }
    if (header->encrypted != ENCRYPTION_OFF) {
        return true;
    }

    //  Verify integrity of data field
    uint16_t calculatedCRC;
    calculatedCRC = crc16_ccitt((uint8_t *)&rawData[2], (dataSize + 2));
    uint16_t packetCRC;
    memcpy(&packetCRC, rawData + 4 + dataSize, CRC_LEN);
    if (calculatedCRC != packetCRC) {
        return true;
    }

    //  Extract meaningful data
    packet_type_e command = (packet_type_e)header->type;
    packet_data_u *data = (packet_data_u *)(rawData + HEADER_LENGTH);

    switch (command) {
    case GET_DEVICE_ID: {
        *type = GET_DEVICE_ID;
        break;
    }
    case SET_LEDS_STATE: {
        *type = SET_LEDS_STATE;
        *payloadSize = sizeof(led_state_s);
        memcpy(payload, &data->led_state, *payloadSize);
        break;
    }
    }
    return true;
}

/*********************************************************************
* Function: void APP_DeviceCDCBasicDemoTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceCDCBasicDemoInitialize() and APP_DeviceCDCBasicDemoStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCBasicDemoTasks()
{
    uint8_t numBytesWrite = 0;
    uint8_t packet[MAX_PACKET_SIZE];
    uint8_t packetSize;
    packet_data_u data;

    bool button1IsPressed = BUTTON_IsPressed(BUTTON_S1);
    bool button2IsPressed = BUTTON_IsPressed(BUTTON_S2);
    bool button3IsPressed = BUTTON_IsPressed(BUTTON_S3);
    
    bool button1WasPressed = (buttons_state[0].state & BUTTON_PRESSED) ? true : false;
    bool button2WasPressed = (buttons_state[1].state & BUTTON_PRESSED) ? true : false;
    bool button3WasPressed = (buttons_state[2].state & BUTTON_PRESSED) ? true : false;

    if ((button1IsPressed != button1WasPressed) || (button2IsPressed != button2WasPressed) || (button3IsPressed != button3WasPressed)) {
        /* Make sure that the CDC driver is ready for a transmission.
         */
        if (mUSBUSARTIsTxTrfReady() == true) {
            uint16_t value;
            uint32_t percent;
            value = ADC_Read10bit(ADC_CHANNEL_1);
            percent = ((uint32_t)100 * value) / 0x03FF;
            buttons_state[0].state = button1IsPressed ? BUTTON_PRESSED : BUTTON_RELEASED;
            buttons_state[0].uid = percent;

            value = ADC_Read10bit(ADC_CHANNEL_2);
            percent = ((uint32_t)100 * value) / 0x03FF;
            buttons_state[1].state = button2IsPressed ? BUTTON_PRESSED : BUTTON_RELEASED;
            buttons_state[1].uid = percent;

            value = ADC_Read10bit(ADC_CHANNEL_3);
            percent = ((uint32_t)100 * value) / 0x03FF;
            buttons_state[2].state = button3IsPressed ? BUTTON_PRESSED : BUTTON_RELEASED;
            buttons_state[2].uid = percent;

            memset(&data, 0x00, sizeof(packet_data_u));
            memcpy(data.buttons_state, buttons_state, 3 * sizeof(button_state_s));
            assemblyPacket(GET_BUTTONS_STATE, &data, packet, &packetSize);
            memcpy(writeBuffer, packet, packetSize);
            numBytesWrite = packetSize;
            
            if (numBytesWrite > 0) {
                putUSBUSART(writeBuffer, numBytesWrite);
            }
        }
    }

    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
    if( USBUSARTIsTxTrfReady() == true)
    {
        uint8_t i;
        uint8_t numBytesRead;
        char deviceID[] = "Keys 011";
        char *e;
        packet_type_e type;
        uint8_t payload[MAX_DATA_SIZE];
        uint8_t payloadSize;

        numBytesRead = getsUSBUSART(readBuffer, sizeof(readBuffer));
        if (numBytesRead > 0) {
            e = memchr(readBuffer, PREAMBLE, numBytesRead);
            if (e == 0) {
                memcpy(writeBuffer, readBuffer, numBytesRead);
                numBytesWrite = numBytesRead;
            } else {
                i = (uint8_t)(e - readBuffer);
                parsePacket(&readBuffer[i], numBytesRead - i, &type, payload, &payloadSize);
                if (type == GET_DEVICE_ID) {
                    memset(&data, 0x00, sizeof(packet_data_u));
                    memcpy(data.device_id, deviceID, 8);
                    assemblyPacket(GET_DEVICE_ID, &data, packet, &packetSize);
                    memcpy(writeBuffer, packet, packetSize);
                    numBytesWrite = packetSize;
                } else if (type == SET_LEDS_STATE) {
                    led_state_s *led = (led_state_s *)payload;
                    if (led->pos != 0) {
                        memcpy(writeBuffer, readBuffer, numBytesRead);
                        numBytesWrite = numBytesRead;
                    } else {
                        if (led->state == LED_TURN_ON) {
                            LED_On(LED_D2);
                        } else {
                            LED_Off(LED_D2);
                        }
                        memset(&data, 0x00, sizeof(packet_data_u));
                        memcpy(data.device_id, deviceID, 3);
                        assemblyPacket(GET_DEVICE_ID, &data, packet, &packetSize);
                        memcpy(writeBuffer, packet, packetSize);
                        numBytesWrite = packetSize;
                    }
                } else {
                    memcpy(writeBuffer, readBuffer, numBytesRead);
                    numBytesWrite = numBytesRead;
                }
            }

            if (numBytesWrite > 0) {
                putUSBUSART(writeBuffer, numBytesWrite);
            }
        }
    }

    CDCTxService();
}