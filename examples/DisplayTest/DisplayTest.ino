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

// This program configures a chipKIT board to talk to
// an LED display I ripped out of an old dishwasher.
// It can be adapted to any display by changing all the
// pin numbers.
//
// It configures four digits on the display and
// displays the value from pin A0 offset by -512
// to give a +/- 512 value.

#include <MUX.h>

void setup() {
    // First add all your anode pins
    MUX::addAnode(26);
    MUX::addAnode(27);
    MUX::addAnode(28);
    MUX::addAnode(29);
    MUX::addAnode(30);
    MUX::addAnode(31);

    // Then add all your cathode pins
    MUX::addCathode(2);
    MUX::addCathode(3);
    MUX::addCathode(32);
    MUX::addCathode(33);
    MUX::addCathode(34);
    MUX::addCathode(35);
    MUX::addCathode(36);
    MUX::addCathode(37);
    MUX::addCathode(38);
    MUX::addCathode(39);
    MUX::addCathode(40);
    MUX::addCathode(41);

    // For this one I need the cathodes to sink
    // current from the display. The anodes will
    // source current by default.
    MUX::invertCathode();

    // Now to set up the four digits using the
    // anode/cathode pairs for each segment.
    MUX::addDigit(
        5, 4, // a
        5, 5, // b
        5, 10, // c
        5, 9, // d
        5, 8, // e
        5, 6, // f
        5, 7 // g
        // no DP
    );

    MUX::addDigit(
        4, 4, // a
        4, 5, // b
        4, 10, // c
        4, 9, // d
        4, 8, // e
        4, 6, // f
        4, 7 // g
        // no DP
    );

    MUX::addDigit(
        3, 4, // a
        3, 5, // b
        3, 10, // c
        3, 9, // d
        3, 8, // e
        3, 6, // f
        3, 7 // g
        // no DP
    );

    MUX::addDigit(
        2, 4, // a
        2, 5, // b
        2, 10, // c
        2, 9, // d
        2, 8, // e
        2, 6, // f
        2, 7 // g
        // no DP
    );

    // Turn the display on
    MUX::start();
}

void loop() {
    MUX::setValue(analogRead(0) - 512);
}

