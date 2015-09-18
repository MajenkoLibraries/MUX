/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MUX_H
#define _MUX_H

#if (ARDUINO >= 100)
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

typedef struct {
    uint8_t anode;
    uint8_t cathode;
} digit;

class MUX {
    private:
        static p32_ioport *_anodePort[16];
        static uint32_t _anodePin[16];
        static p32_ioport *_cathodePort[16];
        static uint32_t _cathodePin[16];
        static digit _digits[32][8];
        static uint8_t _numDigits;
        static uint8_t _row;
        static uint16_t _data[16];
        static boolean _invertAnode;
        static boolean _invertCathode;
        static int _pid;
        static p32_ioport *getPortInformation(uint8_t pin, uint32_t *mask);

    public:

        /*! Add an anode pin to the matrix. */
        static void addAnode(uint8_t pin);
        /*! Add a cathode pin to the matrix. */
        static void addCathode(uint8_t pin);
        /*! Turn on an individual LED in the matrix */
        static void on(int anode, int cathode);
        /*! Turn off an individual LED in the matrix */
        static void off(int anode, int cathode);
        /*! Control an individual LED in the same way as an IO pin */
        static void digitalWrite(int anode, int cathode, int val);
        /*! Set an entire row of data in the internal buffer
         * 
         *  This corresponds to the data for one anode's worth
         *  of display data.
         */
        static void setRow(int row, uint16_t data);
        /*! Invert the anode's output.
         * 
         *  By default all anode pins are *active high* so they
         *  go HIGH when an anode is selected. This will invert
         *  that functionality so idle anodes remain HIGH and the
         *  active anode is switched to LOW.
         */
        static void invertAnode(boolean v = true);
        /*! Invert the cathode's output.
         * 
         *  By default all cathode pins are *active high* so they
         *  go HIGH when a cathode has its bit set. This will invert
         *  that functionality so unset cathodes are set HIGH and the
         *  active cathodes are switched to LOW.
         */
        static void invertCathode(boolean v = true);

        /*! This is an internal function and should never be
         *  called by a user. It is what does the actual displaying
         *  of data on the display. It needs to be public so the
         *  Task Manager has access to it.
         */
        static void display(int id, void *tptr);
        /*! Start displaying data on the matrix */
        static void start();
        /*! Stop displaying data on the matrix */
        static void stop();
        /*! Add a digit mapping to the matrix.
         * 
         *  A digit mapping consists of 7 or 8 pairs of
         *  anode and cathode numbers. These refer to the
         *  combinations required to access the segments
         *  A through G (and optionally the decimal point)
         *  on a 7-segment display.
         */
        static void addDigit(
            uint8_t aa, uint8_t ac,
            uint8_t ba, uint8_t bc,
            uint8_t ca, uint8_t cc,
            uint8_t da, uint8_t dc,
            uint8_t ea, uint8_t ec,
            uint8_t fa, uint8_t fc,
            uint8_t ga, uint8_t gc,
            uint8_t dpa = 255, uint8_t dpc = 255);
        /*! Set an individual digit to a specific value or character.
         * 
         *  The upper nibble of the data (*mode*) defines what the lower
         *  nibble (*value*) does.
         *  
         *  * When *mode* is 0 *value* is a hexadecimal digit (0-F).
         *  * When *mode* is 1 *value* is a hexadecimal digit with
         *    the decimal point turned on (0.-F.)
         *  * When *mode* is 2 *value* is a special character:
         *      * 0 = Degree Sign
         *      * 1 = Minus Sign
         *      * 2 = Equals Sign
         *  * When *mode* is 3 *value* is a special function:
         *      * 0 = Clear the digit
         */
        static void setDigit(int digit, int data);
        /*! Set a numeric value (integer) on the pre-defined digits. */
        static void setValue(int value);
};

#endif
