/**
 * @file    stringFormatter.h  (180.ARM_Peripherals/Project_Headers/console.h)
 * @brief   Basic string formatting routines
 * @date    9 June 2018
 */
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */

#ifndef PROJECT_HEADERS_STRINGFORMATTER_H_
#define PROJECT_HEADERS_STRINGFORMATTER_H_

#include <formatted_io.h>

namespace USBDM {

/**
 * @addtogroup FORMATTED_IO_Group Formatted Input/Output
 * @brief C++ Class allowing input and output of basic types as character streams
 * @{
 */

/**
 * Class for writing formatted information into character buffers (C strings)
 *
 * Example:
 * @code
 *    char buff[100];
 *    StringFormatter formatter(buff, sizeof(buff));
 *
 *    for(int count = 0;;count++) {
 *       formatter.clear().write(count).writeln(": Tick...");
 *       console.write(formatter.toString());
 *    }
 * @endcode
 */
class StringFormatter: public FormattedIO {
protected:
   char * const buff;
   const size_t  size;
   char *ptr    = buff;

public:
   /**
    * Create String Formatter
    *
    * @param[in] buffer      Buffer for characters
    * @param[in] bufferSize  Size of buffer - Note space will be reserved for a terminator
    */
   StringFormatter(char buffer[], size_t bufferSize) : buff(buffer), size(bufferSize-1) {
      usbdm_assert(bufferSize>=1, "Buffer size must be > 1");
   }

   /**
    * Destructor
    */
   virtual ~StringFormatter() {
   }

   /**
    *  Flush output data
    *  Resets buffer to empty
    */
   virtual void flushOutput() override {
      ptr = buff;
   };

   /**
    *  Flush input data - not applicable
    */
   virtual void flushInput() override {
      lookAhead = -1;
   };


   /**
    *  Clear buffer
    *  Resets buffer to empty
    */
   StringFormatter &clear() {
      ptr = buff;
      return *this;
   }

   /**
    * Convert buffer to string.
    *
    * @return String ('\0' terminated)
    */
   const char *toString() {
      // Terminate
      *ptr = '\0';

      // Return ptr to internal buffer
      return buff;
   }

   /**
    * Get length of buffer content.
    */
   unsigned length() const {
      return ptr-buff;
   }

protected:
   /**
    * Check if character is available
    *
    * @return true  Character available i.e. _readChar() will not block
    * @return false No character available
    */
   virtual bool _isCharAvailable() override {
      return false;
   }

   /**
    * Receives a single character - not applicable
    *
    * @return -1
    */
   virtual int _readChar() override {
      return -1;
   }

   /**
    * Writes a character
    * Characters are discarded if buffer is full
    *
    * @param[in]  ch - character to send
    */
   virtual void _writeChar(char ch) override {
      if (ptr < (buff+size)) {
         *ptr++ = ch;
      }
   }

};

/**
 * End FORMATTED_IO_Group
 * @}
 */

} /* end namespace USBDM */

#endif /* PROJECT_HEADERS_STRINGFORMATTER_H_ */
