/*
 * spi.cpp (derived from spi-MK.cpp)
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */
#include <stddef.h>
#include <stdint.h>
#include "system.h"
#include "derivative.h"
#include "utilities.h"
#include "hardware.h"
#include "spi.h"
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
namespace USBDM {

static const uint16_t pbrFactors[] = {2,3,5,7};
static const uint16_t brFactors[]  = {2,4,6,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};

/**
 * Calculate communication speed factors for SPI
 *
 * @param frequency      => Communication frequency in Hz
 * @param clockFrequency => Clock frequency of SPI in Hz
 *
 * @return CTAR register value only including SPI_CTAR_BR, SPI_CTAR_PBR fields
 *
 * Note: Chooses the highest speed that is not greater than frequency.
 * Note: Only has effect from when the CTAR value is next changed
 */
uint32_t Spi::calculateSpeed(uint32_t clockFrequency, uint32_t frequency) {
   int bestPBR = 0;
   int bestBR  = 0;
   uint32_t difference = -1;
   for (int pbr = 3; pbr >= 0; pbr--) {
      for (int br = 15; br >= 0; br--) {
         uint32_t calculatedFrequency = clockFrequency/(pbrFactors[pbr]*brFactors[br]);
         if (calculatedFrequency > frequency) {
            // Too high stop looking here
            break;
         }
         if ((frequency - calculatedFrequency) < difference) {
            // New "best value"
            difference = (frequency - calculatedFrequency);
            bestBR  = br;
            bestPBR = pbr;
         }
      }
   }
   return SPI_CTAR_BR(bestBR)|SPI_CTAR_PBR(bestPBR);
}

/**
 * Calculate Delay factors
 * Used for ASC, DT and CSSCK
 *
 * @param delay          => Desired delay in seconds
 * @param clockFrequency => Clock frequency of SPI in Hz
 * @param bestPrescale   => Best prescaler value (0=>/1, 1=>/3, 2=/5, 3=>/7)
 * @param bestDivider    => Best divider value (N=>/(2**(N+1)))
 *
 * @return true
 *
 * Note: Determines bestPrescaler and bestDivider for the smallest delay that is not less than delay.
 */
void Spi::calculateDelay(float clockFrequency, float delay, int &bestPrescale, int &bestDivider) {
   const float clockPeriod = (1/clockFrequency);
   float difference = MAXFLOAT;

   bestPrescale = -1;
   bestDivider  = -1;
   for (int prescale = 3; prescale >= 0; prescale--) {
      for (int divider = 15; divider >= 0; divider--) {
         float calculatedDelay = clockPeriod*((prescale<<1)+1)*(1UL<<(divider+1));
         if (calculatedDelay < delay) {
            // Too short - stop looking here
            break;
         }
         if ((calculatedDelay - delay) < difference) {
            // New "best delay"
            difference = (calculatedDelay - delay);
            bestPrescale = prescale;
            bestDivider  = divider;
         }
      }
   }
}

/**
 * Transmit and receive a value over SPI
 *
 * @param data - Data to send (8-16 bits) <br>
 *               May include other control bits
 *
 * @return Data received
 */
uint32_t Spi::txRx(uint32_t data) {
   spi->MCR &= ~SPI_MCR_HALT_MASK;
   spi->PUSHR = data;
   while ((spi->SR & SPI_SR_TCF_MASK)==0) {
      __asm__("nop");
   }
   spi->SR = SPI_SR_TCF_MASK|SPI_SR_EOQF_MASK;
   return spi->POPR;  // Return read data
}

/**
 *  Transmit and receive a series of 4 to 8-bit values
 *
 *  @param dataSize  Number of values to transfer
 *  @param dataOut   Transmit bytes (may be NULL for Rx only)
 *  @param dataIn    Receive byte buffer (may be NULL for Tx only)
 */
void Spi::txRxBytes(uint32_t dataSize, const uint8_t *dataOut, uint8_t *dataIn) {
   while(dataSize-->0) {
      uint32_t sendData = 0xFF;
      if (dataOut != 0) {
         sendData = *dataOut++;
      }
      if (dataSize == 0) {
         sendData |= SPI_PUSHR_EOQ_MASK;
      }
      else {
         sendData |= SPI_PUSHR_CONT_MASK;
      }
      uint32_t data = txRx(sendData|pushrMask);
      if (dataIn != 0) {
         *dataIn++ = data;
      }
   }
   spi->MCR |= SPI_MCR_HALT_MASK;
   while ((spi->SR&SPI_SR_TXRXS_MASK)) {
      __asm__("nop");
   }
}

/**
 *  Transmit and receive a series of 9 to 16-bit values
 *
 *  @param dataSize  Number of values to transfer
 *  @param dataOut   Transmit values (may be NULL for Rx only)
 *  @param dataIn    Receive buffer (may be NULL for Tx only)
 */
void Spi::txRxWords(uint32_t dataSize, const uint16_t *dataOut, uint16_t *dataIn) {
   while(dataSize-->0) {
      uint32_t sendData = 0xFFFF;
      if (dataOut != 0) {
         sendData = *dataOut++;
      }
      if (dataSize == 0) {
         sendData |= SPI_PUSHR_EOQ_MASK;
      }
      else {
         sendData |= SPI_PUSHR_CONT_MASK;
      }
      uint32_t data = txRx(sendData|pushrMask);
      if (dataIn != 0) {
         *dataIn++ = data;
      }
   }
   spi->MCR |= SPI_MCR_HALT_MASK;
   while ((spi->SR&SPI_SR_TXRXS_MASK)) {
      __asm__("nop");
   }
}

} // End namespace USBDM
