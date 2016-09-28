/*
 * usb.h
 *
 *  Created on: 25/10/2013
 *      Author: podonoghue
 */

#ifndef USB_H_
#define USB_H_
 /*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "hardware.h"

namespace USBDM {

/**
 * Type definition for USB interrupt call back
 *
 *  @param status Interrupt status e.g. USB_ISTAT_SOFTOK_MASK, USB_ISTAT_STALL_MASK etc
 */
typedef void (*USBCallbackFunction)(uint8_t status);

template <class Info>
class UsbBase_T {

protected:
   static constexpr volatile USB_Type *usb      = Info::usb;
   static constexpr volatile uint32_t *clockReg = Info::clockReg;

public:
   /**
    * Initialise USB to default settings\n
    * Configures all USB pins
    */
   static void enable() {
      *clockReg |= Info::clockMask;
      __DMB();

      Info::initPCRs();

      enableNvicInterrupts();
   }

   /**
    * Enable/disable interrupts in NVIC
    *
    * @param enable true to enable, false to disable
    */
   static void enableNvicInterrupts(bool enable=true) {

      if (enable) {
         // Enable interrupts
         NVIC_EnableIRQ(Info::irqNums[0]);

         // Set priority level
         NVIC_SetPriority(Info::irqNums[0], Info::irqLevel);
      }
      else {
         // Disable interrupts
         NVIC_DisableIRQ(Info::irqNums[0]);
      }
   }

   /**
    * Enable/disable interrupts
    *
    * @param mask Mask of interrupts to enable e.g. USB_INTEN_SOFTOKEN_MASK, USB_INTEN_STALLEN_MASK etc
    */
   static void enableInterrupts(uint8_t mask=0xFF) {
      usb->INTEN = mask;
   }

   /**
    * Enable/disable OTG interrupts
    *
    * @param mask Mask of interrupts to enable e.g. USB_OTGICR_IDEN_MASK, USB_OTGICR_ONEMSECEN_MASK etc
    */
   static void enableOtgInterrupts(uint8_t mask=0xFF) {
      usb->OTGICR = mask;
   }

private:
   static void handleTokenComplete();
   static void handleUSBResume();
   static void handleUSBReset();
   static void handleStallComplete();
   static void handleSOFToken();
   static void handleUSBSuspend();

public:

   static void initialise();
   /**
    * Handler for USB interrupt
    *
    * Determines source and dispatches to appropriate routine.
    */
   static void irqHandler(void);
};

#ifdef USBDM_USB0_IS_DEFINED
/**
 * Class representing USB
 */
class Usb0 : public UsbBase_T<Usb0Info> {
public:
   static void receiveUSBCommand(uint8_t maxSize, uint8_t *buffer);
   static void sendUSBResponse( uint8_t size, const uint8_t *buffer);
};

#endif

} // End namespace USBDM

#endif /* USB_H_ */