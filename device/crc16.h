// Copyright Filbert
// CRC16ccitt
//
//
//
#ifndef CRC16_H
#define	CRC16_H

#include <stdint.h>

#if defined (__PICC18__)
//    #include "Configure.h"
#endif
#define  CRC_CCITT_INIT             0xFFFF
uint16_t crc16_ccitt  (uint8_t *buf, uint16_t len);
void crc16_ccitt_init (uint16_t init_data);
void crc16_ccitt_next (uint8_t next_data);
uint16_t crc16_ccitt_get (void);

#endif	/* CRC16_H */

