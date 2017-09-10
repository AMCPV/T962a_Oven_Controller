/**
 * @file     pit.h (180.ARM_Peripherals/Project_Headers/pit-MK.h)
 *
 * @brief    Programmable Interrupt Timer interface
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */

#ifndef INCLUDE_USBDM_PIT_H_
#define INCLUDE_USBDM_PIT_H_
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "derivative.h"
#include "system.h"
#include "hardware.h"

namespace USBDM {

/**
 * @addtogroup PIT_Group PIT, Programmable Interrupt Timer
 * @brief Abstraction for Programmable Interrupt Timer
 * @{
 */
/**
 * Type definition for PIT interrupt call back
 */
typedef void (*PitCallbackFunction)(void);

/**
 * Enable the PIT
 */
enum PitDebugMode {
   PitDebugMode_Run  = PIT_MCR_FRZ(0),  //!< PIT continues to run in debug mode
   PitDebugMode_Stop = PIT_MCR_FRZ(1),  //!< PIT stops in debug mode
};

/**
 * Enable the PIT
 */
enum PitChannelIrq {
   PitChannelIrq_Disable  = PIT_TCTRL_TIE(0),  //!< PIT channel interrupt disabled
   PitChannelIrq_Enable   = PIT_TCTRL_TIE(1),  //!< PIT channel interrupt disabled
};

/**
 * Enable the PIT
 */
enum PitChannelEnable {
   PitChannelEnable_Disable  = PIT_TCTRL_TEN(0),  //!< PIT channel disabled
   PitChannelEnable_Enable   = PIT_TCTRL_TEN(1),  //!< PIT channel enabled
};

/**
 * @brief Class representing a Programmable Interrupt  Timer
 *
 * <b>Example</b>
 * @code
 *
 * @endcode
 */
template<class Info>
class PitBase_T {

private:
   /**
    * This class is not intended to be instantiated
    */
   PitBase_T() = delete;
   PitBase_T(const PitBase_T&) = delete;
   PitBase_T(PitBase_T&&) = delete;

protected:
   /** Default TCTRL value for timer channel */
   static constexpr uint32_t PIT_TCTRL_DEFAULT_VALUE = (PIT_TCTRL_TEN_MASK);

   /** Callback functions for ISRs */
   static PitCallbackFunction callbacks[Info::irqCount];

   /** Callback to catch unhandled interrupt */
   static void unhandledCallback() {
      setAndCheckErrorCode(E_NO_HANDLER);
   }
   
public:
   /** PIT interrupt handler -  Calls PIT0 callback */
   static void irq0Handler() {
      // Clear interrupt flag
      PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
      callbacks[0]();
   }
   /** PIT interrupt handler -  Calls PIT1 callback */
   static void irq1Handler() {
      // Clear interrupt flag
      PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF_MASK;
      callbacks[1]();
   }
   /** PIT interrupt handler -  Calls PIT2 callback */
   static void irq2Handler() {
      // Clear interrupt flag
      PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF_MASK;
      callbacks[2]();
   }
   /** PIT interrupt handler -  Calls PIT3 callback */
   static void irq3Handler() {
      // Clear interrupt flag
      PIT->CHANNEL[3].TFLG = PIT_TFLG_TIF_MASK;
      callbacks[3]();
   }

public:
   /**
    * Enable/disable channel interrupts
    *
    * @param[in]  channel Channel being modified
    * @param[in]  enable  True => enable, False => disable
    */
   static void enableInterrupts(unsigned channel, bool enable=true) {
      if (enable) {
         pit->CHANNEL[channel].TCTRL |= PIT_TCTRL_TIE_MASK;
      }
      else {
         pit->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TIE_MASK;
      }
   }

   /**
    * Set callback for ISR
    *
    * @param[in]  channel  The PIT channel to modify
    * @param[in]  callback The function to call from stub ISR
    */
   static void setCallback(unsigned channel, PitCallbackFunction callback) {
      if (callback == nullptr) {
         callback = unhandledCallback;
         enableInterrupts(channel, false);
      }
      callbacks[channel] = callback;
   }

protected:
   /** Pointer to hardware */
   static constexpr volatile PIT_Type *pit       = Info::pit;

   /** Pointer to clock register */
   static constexpr volatile uint32_t *clockReg  = Info::clockReg;

public:
   /**
    * Basic enable of PIT\n
    * Includes configuring all pins
    */
   static void enable() {
      // Enable clock
      *clockReg |= Info::clockMask;
      __DMB();
   }

   /**
    *  Enables and configures the PIT
    *
    *  @param[in]  pitDebugMode  Determined whether the PIT halts when suspended during debug
    */
   static void configure(PitDebugMode pitDebugMode) {
      enable();
      pit->MCR = pitDebugMode|PIT_MCR_MDIS(0); // MDIS cleared => enabled!
   }

   /**
    *  Enable the PIT with default settings.
    *  The channels are enabled with default settings.
    *
    *  @param[in]  mcr       Module Control Register
    */
   static void configure(uint32_t mcr=Info::mcr) {
      enable();

      // Enable timer
      pit->MCR = mcr&~PIT_MCR_MDIS_MASK;
      for (unsigned i=0; i<Info::irqCount; i++) {
         configureChannelInTicks(i, Info::pit_ldval);
         enableNvicInterrupts(i);
      }
   }

   /**
    *   Disable the PIT (all channels)
    */
   static void disable() {
      pit->MCR = PIT_MCR_MDIS_MASK;
      *clockReg &= ~Info::clockMask;
   }

   /**
    * Enable/disable interrupts in NVIC
    *
    * @param[in]  channel Channel being modified
    * @param[in]  enable True => enable, False => disable
    *
    * @return E_NO_ERROR on success
    */
   static ErrorCode enableNvicInterrupts(unsigned channel, bool enable=true) {
      if (channel>=Info::irqCount) {
         setAndCheckErrorCode(E_ILLEGAL_PARAM);
      }
      IRQn_Type irqNum;
      switch(channel) {
         case 0: irqNum = Info::irqNums[0]; break;
         case 1: irqNum = Info::irqNums[1]; break;
         case 2: irqNum = Info::irqNums[2]; break;
         case 3: irqNum = Info::irqNums[3]; break;
         default:
            return setErrorCode(E_ILLEGAL_PARAM);
      }
      if (enable) {
         // Enable interrupts
         NVIC_EnableIRQ(irqNum);

         // Set priority level
         NVIC_SetPriority(irqNum, Info::irqLevel);
      }
      else {
         // Disable interrupts
         NVIC_DisableIRQ(irqNum);
      }
      return E_NO_ERROR;
   }
   /**
    *  Enable/Disable the PIT channel
    *
    *  @param[in]  channel   Channel to enable
    *  @param[in]  enable    Controls whether channel is enabled or disabled
    */
   static void enableChannel(const uint8_t channel, bool enable=true) {
      if (enable) {
         pit->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
      }
      else {
         pit->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;
      }
      enableNvicInterrupts(channel, enable);
   }
   /**
    *  Configure the PIT channel
    *
    *  @param[in]  channel           Channel to configure
    *  @param[in]  interval          Interval in timer ticks (usually bus clock period)
    *  @param[in]  pitChannelIrq     Whether to enable interrupts
    *  @param[in]  pitChannelEnable  Whether to enable channel initially
    */
   static void configureChannelInTicks(
         uint8_t           channel,
         uint32_t          interval,
         PitChannelIrq     pitChannelIrq=PitChannelIrq_Disable,
         PitChannelEnable  pitChannelEnable=PitChannelEnable_Enable) {

      pit->CHANNEL[channel].LDVAL = interval;
      pit->CHANNEL[channel].TCTRL = pitChannelIrq|pitChannelEnable;
      pit->CHANNEL[channel].TFLG  = PIT_TFLG_TIF_MASK;

      enableNvicInterrupts(channel);
   }
   /**
    *  Configure the PIT channel
    *
    *  @param[in]  channel           Channel to configure
    *  @param[in]  interval          Interval in timer ticks (usually bus clock period)
    *  @param[in]  pitChannelIrq     Whether to enable interrupts
    *  @param[in]  pitChannelEnable  Whether to enable channel initially
    */
   static void configureChannel(
         uint8_t           channel,
         float             interval,
         PitChannelIrq     pitChannelIrq=PitChannelIrq_Disable,
         PitChannelEnable  pitChannelEnable=PitChannelEnable_Enable) {

      pit->CHANNEL[channel].LDVAL = round((interval*PitInfo::getClockFrequency())-1);
      pit->CHANNEL[channel].TCTRL = pitChannelIrq|pitChannelEnable;
      pit->CHANNEL[channel].TFLG  = PIT_TFLG_TIF_MASK;

      enableNvicInterrupts(channel);
   }
   /**
    * Set period in seconds
    *
    * @param[in]  channel Channel being modified
    * @param[in]  interval Interval in seconds
    */
   static void setPeriod(unsigned channel, float interval) {
      pit->CHANNEL[channel].LDVAL = round((interval*PitInfo::getClockFrequency())-1);
   }
   /**
    * Set period in seconds
    *
    * @param[in]  channel Channel being modified
    * @param[in]  interval Interval in seconds
    */
   static void setPeriodInTicks(unsigned channel, uint32_t interval) {
      pit->CHANNEL[channel].LDVAL = interval;
   }
   /**
    *   Disable the PIT channel
    *
    *   @param[in]  channel Channel to disable
    */
   static void disableChannel(uint8_t channel) {

      // Disable timer channel
      pit->CHANNEL[channel].TCTRL = 0;

      // Disable timer interrupts
      enableNvicInterrupts(channel, false);
   }
   /**
    *  Use a PIT channel to implement a busy-wait delay
    *
    *  @param[in]  channel   Channel to use
    *  @param[in]  interval  Interval to wait in timer ticks (usually bus clock period)
    *
    *  @note Function doesn't return until interval has expired
    */
   static void delayInTicks(uint8_t channel, uint32_t interval) {
      configureChannelInTicks(channel, interval);
      while (pit->CHANNEL[channel].TFLG == 0) {
         __NOP();
      }
      disableChannel(channel);
   }
   /**
    *  Use a PIT channel to implement a busy-wait delay
    *
    *  @param[in]  channel   Channel to use
    *  @param[in]  interval  Interval to wait as a float
    *
    *  @note Function doesn't return until interval has expired
    */
   static void delay(uint8_t channel, float interval) {
      configureChannel(channel, interval);
      while (pit->CHANNEL[channel].TFLG == 0) {
         __NOP();
      }
      disableChannel(channel);
   }

};

/**
 * Callback table for programmatically set handlers
 */
template<class Info> PitCallbackFunction PitBase_T<Info>::callbacks[] = {
   unhandledCallback,
   unhandledCallback,
   unhandledCallback,
   unhandledCallback
};

template <class Info, int channel>
class PitChannel_T : public PitBase_T<Info> {

public:

   /** Timer channel number */
   static constexpr int CHANNEL = channel;

   /**
    *  Configure the PIT channel
    *
    *  @param[in]  interval          Interval in timer ticks (usually bus clock period)
    *  @param[in]  pitChannelIrq     Whether to enable interrupts
    *  @param[in]  pitChannelEnable  Whether to enable channel initially
    */
   static void __attribute__((always_inline)) configureInTicks(
         uint32_t          interval,
         PitChannelIrq     pitChannelIrq=PitChannelIrq_Disable,
         PitChannelEnable  pitChannelEnable=PitChannelEnable_Enable) {

      PitBase_T<Info>::configureChannelInTicks(channel, interval, pitChannelIrq, pitChannelEnable);
      PitBase_T<Info>::setCallback(PitBase_T<Info>::unhandledCallback);
   }
   /**
    *  Configure the PIT channel
    *
    *  @param[in]  interval          Interval in timer ticks (usually bus clock period)
    *  @param[in]  pitChannelIrq     Whether to enable interrupts
    *  @param[in]  pitChannelEnable  Whether to enable channel initially
    */
   static void __attribute__((always_inline)) configure(
         float             interval,
         PitChannelIrq     pitChannelIrq=PitChannelIrq_Disable,
         PitChannelEnable  pitChannelEnable=PitChannelEnable_Enable) {

      PitBase_T<Info>::configureChannel(channel, interval, pitChannelIrq, pitChannelEnable);
   }
   /**
    * Set period in seconds
    *
    * @param[in]  interval Interval in seconds
    */
   static void __attribute__((always_inline)) setPeriod(float interval) {
      PitBase_T<Info>::setPeriod(channel, interval);
   }
   /**
    * Set period in seconds
    *
    * @param[in]  interval Interval in seconds
    */
   static void __attribute__((always_inline)) setPeriodInTicks(uint32_t interval) {
      PitBase_T<Info>::setPeriodInTicks(channel, interval);
   }
   /**
    *   Disable the PIT channel
    */
   static void __attribute__((always_inline)) disable() {
      PitBase_T<Info>::disableChannel(channel);
   }
   /**
    * Set callback for channel ISR
    *
    * @param[in]  callback The function to call from stub ISR
    */
   static void __attribute__((always_inline)) setCallback(PitCallbackFunction callback) {
      PitBase_T<Info>::setCallback(channel, callback);
   }
   /**
    * Enable/disable channel interrupts
    *
    * @param[in]  enable  True => enable, False => disable
    */
   static void __attribute__((always_inline)) enableInterrupts(bool enable=true) {
      PitBase_T<Info>::enableInterrupts(channel, enable);
   }
   /**
    * Enable/disable interrupts in NVIC
    *
    * @param[in]  enable True => enable, False => disable
    *
    * @return E_NO_ERROR on success
    */
   static ErrorCode __attribute__((always_inline)) enableNvicInterrupts(bool enable=true) {
      return PitBase_T<Info>::enableNvicInterrupts(channel, enable);
   }

};

#ifdef PIT
/**
 * @brief class representing the PIT
 */
using Pit = PitBase_T<PitInfo>;
/**
 * @brief class representing the PIT channel 0
 */
using PitChannel0 = PitChannel_T<PitInfo, 0>;
/**
 * @brief class representing the PIT channel 1
 */
using PitChannel1 = PitChannel_T<PitInfo, 1>;
/**
 * @brief class representing the PIT channel 2
 */
using PitChannel2 = PitChannel_T<PitInfo, 2>;
/**
 * @brief class representing the PIT channel 3
 */
using PitChannel3 = PitChannel_T<PitInfo, 3>;
#endif

/**
 * @}
 */

} // End namespace USBDM

#endif /* INCLUDE_USBDM_PIT_H_ */
