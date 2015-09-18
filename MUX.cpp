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


#include <MUX.h>

p32_ioport *MUX::_anodePort[16] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
uint32_t MUX::_anodePin[16];
p32_ioport *MUX::_cathodePort[16] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
uint32_t MUX::_cathodePin[16];
uint8_t MUX::_row = 0;
uint16_t MUX::_data[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
boolean MUX::_invertAnode = false;
boolean MUX::_invertCathode = false;
int MUX::_pid = -1;
digit MUX::_digits[32][8];
uint8_t MUX::_numDigits = 0;

void MUX::addAnode(uint8_t pin) {
    for (int i = 0; i < 16; i++) {
        if (_anodePort[i] == NULL) {
            pinMode(pin, OUTPUT);
            _anodePort[i] = getPortInformation(pin, &_anodePin[i]);
            return;
        }
    }
}

void MUX::addCathode(uint8_t pin) {
    for (int i = 0; i < 16; i++) {
        if (_cathodePort[i] == NULL) {
            pinMode(pin, OUTPUT);
            _cathodePort[i] = getPortInformation(pin, &_cathodePin[i]);
            return;
        }
    }
}

p32_ioport *MUX::getPortInformation(uint8_t pin, uint32_t *mask) {
    uint32_t portno = digitalPinToPort(pin);
    if (portno == NOT_A_PIN) {
        return NULL;
    }
    if (mask != NULL) {
        *mask = digitalPinToBitMask(pin);
    }
    return (p32_ioport *)portRegisters(portno);
}

void MUX::on(int a, int c) {
    if (a < 16 && c < 16) {
        _data[a & 0xF] |= 1<<(c & 0xF);
    }
}

void MUX::off(int a, int c) {
    if (a < 16 && c < 16) {
        _data[a & 0xF] &= ~(1<<(c & 0xF));
    }
}

void MUX::invertAnode(boolean v) {
    _invertAnode = v;
}

void MUX::invertCathode(boolean v) {
    _invertCathode = v;
}

void MUX::display(int id, void *tptr) {

    // Clear all cathodes first
    if (_invertCathode) {
        for (int i = 0; (i < 16) && (_cathodePort[i] != NULL); i++) {
            _cathodePort[i]->lat.set = _cathodePin[i];
        }
    } else {
        for (int i = 0; (i < 16) && (_cathodePort[i] != NULL); i++) {
            _cathodePort[i]->lat.clr = _cathodePin[i];
        }
    }

    // Then change the anode
    if (_invertAnode) {
        for (int i = 0; (i < 16) && (_anodePort[i] != NULL); i++) {
            if (i == _row) {
                _anodePort[i]->lat.clr = _anodePin[i];
            } else {
                _anodePort[i]->lat.set = _anodePin[i];
            }
        }
    } else {
        for (int i = 0; (i < 16) && (_anodePort[i] != NULL); i++) {
            if (i == _row) {
                _anodePort[i]->lat.set = _anodePin[i];
            } else {
                _anodePort[i]->lat.clr = _anodePin[i];
            }
        }            
    }

    // Finally draw the row into the cathodes
    if (_invertCathode) {
        for (int i = 0; (i < 16) && (_cathodePort[i] != NULL); i++) {
            if (_data[_row] & (1 << i)) {
                _cathodePort[i]->lat.clr = _cathodePin[i];
            } else {
                _cathodePort[i]->lat.set = _cathodePin[i];
            }
        }
    } else {
        for (int i = 0; (i < 16) && (_cathodePort[i] != NULL); i++) {
            if (_data[_row] & (1 << i)) {
                _cathodePort[i]->lat.set = _cathodePin[i];
            } else {
                _cathodePort[i]->lat.clr = _cathodePin[i];
            }
        }
    }

    _row++;
    if (_row == 16 || _anodePort[_row] == NULL) {
        _row = 0;
    }
}

void MUX::start() {
    _pid = createTask(display, 2, TASK_ENABLE, NULL);    
}

void MUX::stop() {
    destroyTask(_pid);
}

void MUX::setRow(int r, uint16_t d) {
    if (r < 16) {
        _data[r] = d;
    }
}

void MUX::addDigit(
    uint8_t aa, uint8_t ac,
    uint8_t ba, uint8_t bc,
    uint8_t ca, uint8_t cc,
    uint8_t da, uint8_t dc,
    uint8_t ea, uint8_t ec,
    uint8_t fa, uint8_t fc,
    uint8_t ga, uint8_t gc,
    uint8_t dpa, uint8_t dpc
) {
    if (_numDigits >= 32) {
        return;
    }
    _digits[_numDigits][0].anode = aa;
    _digits[_numDigits][0].cathode = ac;
    _digits[_numDigits][1].anode = ba;
    _digits[_numDigits][1].cathode = bc;
    _digits[_numDigits][2].anode = ca;
    _digits[_numDigits][2].cathode = cc;
    _digits[_numDigits][3].anode = da;
    _digits[_numDigits][3].cathode = dc;
    _digits[_numDigits][4].anode = ea;
    _digits[_numDigits][4].cathode = ec;
    _digits[_numDigits][5].anode = fa;
    _digits[_numDigits][5].cathode = fc;
    _digits[_numDigits][6].anode = ga;
    _digits[_numDigits][6].cathode = gc;
    _digits[_numDigits][7].anode = dpa;
    _digits[_numDigits][7].cathode = dpc;
    _numDigits++;
}

// Value or code in the lower nibble,
// mode in the upper nibble.
//
// mode:
// 0 = value is a decimal or hexadecimal number
// 1 = value is a decimal or hexadecimal number with DP
// 2 = value is a special character
//    0 = °
//    1 = -
//    2 = =
// 3 = value is a special function
//    0 = clear digit

void MUX::setDigit(int d, int v) {
    if (d >= _numDigits) {
        return;
    }

    uint8_t val = v & 0x0F;
    uint8_t mode = v >> 4;

    switch (mode) {

        case 0:
        case 1:
            // Numbers, with or without DP
            // Turn on the DP if it's wanted
            if (mode == 1) {
                on(_digits[d][7].anode, _digits[d][7].cathode);
            } else {
                off(_digits[d][7].anode, _digits[d][7].cathode);
            }
        
            switch (val) {
                case 0x0:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    off(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x1:
                    off(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    off(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x2:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x3:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x4:
                    off(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x5:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x6:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x7:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    off(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x8:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0x9:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0xA:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0xB:
                    off(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0xC:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    off(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0xD:
                    off(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    on(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0xE:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 0xF:
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    on(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
            }
            break;
        case 2: // Special characters
            switch (val) {
                case 0: // Degree
                    on(_digits[d][0].anode, _digits[d][0].cathode);
                    on(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    on(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    break;
                case 1: // Minus
                    off(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    off(_digits[d][7].anode, _digits[d][7].cathode);
                    break;
                case 2: // Equals
                    off(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    on(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    on(_digits[d][6].anode, _digits[d][6].cathode);
                    off(_digits[d][7].anode, _digits[d][7].cathode);
                    break;
            }
            break;
        case 3: // Special functions
            switch (val) {
                case 0: // Clear
                    off(_digits[d][0].anode, _digits[d][0].cathode);
                    off(_digits[d][1].anode, _digits[d][1].cathode);
                    off(_digits[d][2].anode, _digits[d][2].cathode);
                    off(_digits[d][3].anode, _digits[d][3].cathode);
                    off(_digits[d][4].anode, _digits[d][4].cathode);
                    off(_digits[d][5].anode, _digits[d][5].cathode);
                    off(_digits[d][6].anode, _digits[d][6].cathode);
                    off(_digits[d][7].anode, _digits[d][7].cathode);
                    break;
            }
    }
}

void MUX::setValue(int v) {
    char temp[33];
    snprintf(temp, 32, "%*d", _numDigits, v);
    for (int i = 0; i < _numDigits; i++) {
        switch (temp[i]) {
            case '-': setDigit(i, 0x21); break;
            case ' ': setDigit(i, 0x30); break;
            case '0': setDigit(i, 0x00); break;
            case '1': setDigit(i, 0x01); break;
            case '2': setDigit(i, 0x02); break;
            case '3': setDigit(i, 0x03); break;
            case '4': setDigit(i, 0x04); break;
            case '5': setDigit(i, 0x05); break;
            case '6': setDigit(i, 0x06); break;
            case '7': setDigit(i, 0x07); break;
            case '8': setDigit(i, 0x08); break;
            case '9': setDigit(i, 0x09); break;
        }
    }
}

void MUX::digitalWrite(int a, int c, int v) {
    if (v) {
        on(a, c);
    } else {
        off(a, c);
    }
}
