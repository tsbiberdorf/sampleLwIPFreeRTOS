/*
 * crcInterface.h
 *
 *  Created on: May 13, 2020
 *      Author: ThreeBoysTech
 */

#ifndef PERIPHERAL_CRC_CRCINTERFACE_H_
#define PERIPHERAL_CRC_CRCINTERFACE_H_

void crcWriteData(const uint8_t *data, size_t dataSize);
SemaphoreHandle_t InitCrc16();
uint16_t crcGet16bitResult();
void crcReleaseSemaphore(SemaphoreHandle_t CRCSemaphore);

#endif /* PERIPHERAL_CRC_CRCINTERFACE_H_ */
