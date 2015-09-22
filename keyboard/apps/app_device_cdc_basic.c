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
#include <xprintf.h>
#include "../../device/protocol_pc_to_device.h"
#include "../../device/crc16.h"

/** VARIABLES ******************************************************/

static bool buttonPressed;
static char buttonMessage[] = "Button pressed.\r\n";
static char messageADC[30];
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

    buttonPressed = false;
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
    bool button1pressed = BUTTON_IsPressed(BUTTON_S1);
    bool button2pressed = BUTTON_IsPressed(BUTTON_S2);
    bool button3pressed = BUTTON_IsPressed(BUTTON_S3);
    /* If the user has pressed the button associated with this demo, then we
     * are going to send a "Button Pressed" message to the terminal.
     */
    if (button1pressed || button2pressed || button3pressed)
    {
        /* Make sure that we only send the message once per button press and
         * not continuously as the button is held.
         */
        if(buttonPressed == false)
        {
            /* Make sure that the CDC driver is ready for a transmission.
             */
            if(mUSBUSARTIsTxTrfReady() == true)
            {
                if (button1pressed) {
                    uint16_t value = ADC_Read10bit(ADC_CHANNEL_1);
                    uint32_t percent = ((uint32_t)100 * value) / 0x03FF;
                    xsprintf(messageADC, "Btn1=%D-%D\r\n", value, percent);
                } else if (button2pressed) {
                    uint16_t value = ADC_Read10bit(ADC_CHANNEL_2);
                    uint32_t percent = ((uint32_t)100 * value) / 0x03FF;
                    xsprintf(messageADC, "Btn2=%D-%D\r\n", value, percent);
                } else {
                    uint16_t value = ADC_Read10bit(ADC_CHANNEL_3);
                    uint32_t percent = ((uint32_t)100 * value) / 0x03FF;
                    xsprintf(messageADC, "Btn3=%D-%D\r\n", value, percent);
                }
                putsUSBUSART(messageADC);
                buttonPressed = true;
            }
        }
    }
    else
    {
        /* If the button is released, we can then allow a new message to be
         * sent the next time the button is pressed.
         */
        buttonPressed = false;
    }

    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
    if( USBUSARTIsTxTrfReady() == true)
    {
        uint8_t i;
        uint8_t numBytesRead;
        uint8_t numBytesWrite = 0;
        uint8_t dataLenght;
        uint16_t crc;
        char *e;

        numBytesRead = getsUSBUSART(readBuffer, sizeof(readBuffer));
        if (numBytesRead > 0) {
            e = memchr(readBuffer, PREAMBLE, numBytesRead);
            if (e == 0) {
                memcpy(writeBuffer, readBuffer, numBytesRead);
                numBytesWrite = numBytesRead;
            } else {
                i = (uint8_t)(e - readBuffer);
                e = memchr(&readBuffer[i], DIRECTION_TO_DEVICE, numBytesRead - i);
                if (e == 0) {
                    memcpy(writeBuffer, readBuffer, numBytesRead);
                    numBytesWrite = numBytesRead;
                } else {
                    i = (uint8_t)(e - readBuffer);
                    dataLenght = 6 + 8;
                    writeBuffer[0] = PREAMBLE;
                    writeBuffer[1] = DIRECTION_FROM_DEVICE;
                    writeBuffer[2] = dataLenght;
                    writeBuffer[3] = 0x00;
                    writeBuffer[4] = ENCRYPTION_OFF;
                    writeBuffer[5] = GET_DEVICE_ID;
                    writeBuffer[6] = 0xFF;
                    writeBuffer[7] = 0xFF;
                    writeBuffer[8] = 0xFF;
                    writeBuffer[9] = 0xFF;
                    writeBuffer[10] = 'K';
                    writeBuffer[11] = 'e';
                    writeBuffer[12] = 'y';
                    writeBuffer[13] = 's';
                    writeBuffer[14] = ' ';
                    writeBuffer[15] = 0 + '0';
                    writeBuffer[16] = 1 + '0';
                    writeBuffer[17] = 1 + '0';
                    crc = crc16_ccitt(&writeBuffer[2], (2 + dataLenght));
                    writeBuffer[18] = (uint8_t)(crc & 0xFF);
                    writeBuffer[19] = (uint8_t)((crc >> 8) & 0xFF);
                    numBytesWrite = 20;
                }
            }

            if (numBytesWrite > 0) {
                putUSBUSART(writeBuffer, numBytesWrite);
            }
        }
    }

    CDCTxService();
}