/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <xc.h>
#include "mcu/mcu.h"
#include "mcu/pps.h"

#define MCU_GPIO_UNDEF                  (0xFF)
#define PPS_BASE_ADDRESS                (0xBF801500)
#define PPS_PORT_SPACING                (0x40)

static volatile uint32_t *input_regs[4][16] = {
    {
        &INT3R,
        &T2CKR,
        &T6CKR,
        &IC3R,
        &IC7R,
        &U1RXR,
        &U2CTSR,
        &U5RXR,
        &U6CTSR,
        &SDI1R,
        &SDI3R,
        &SDI5R,
        &SS6R,
        &REFCLKI1R
    },
    {
        &INT4R,
        &T5CKR,
        &T7CKR,
        &IC4R,
        &IC8R,
        &U3RXR,
        &U4CTSR,
        &SDI2R,
        &SDI4R,
        NULL,
        &REFCLKI4R
    },
    {
        &INT2R,
        &T3CKR,
        &T8CKR,
        &IC2R,
        &IC5R,
        &IC9R,
        &U1CTSR,
        &SS1R,
        &SS3R,
        &SS4R,
        &SS5R,
        NULL
    },
    {
        &INT1R,
        &T4CKR,
        &T9CKR,
        &IC1R,
        &IC6R,
        &U3CTSR,
        &U4RXR,
        &U6RXR,
        &SS2R,
        &SDI6R,
        &OCFAR,
        &REFCLKI3R
    }
};

static const uint8_t output_pins[4][16] = {
    {
        MCU_GPIO_PORTD(2),
        MCU_GPIO_PORTG(8),
        MCU_GPIO_PORTF(4),
        MCU_GPIO_PORTD(10),
        MCU_GPIO_PORTF(1),
        MCU_GPIO_PORTB(9),
        MCU_GPIO_PORTB(10),
        MCU_GPIO_PORTC(14),
        MCU_GPIO_PORTB(5),
        MCU_GPIO_UNDEF,
        MCU_GPIO_PORTC(1),
        MCU_GPIO_PORTD(14),
        MCU_GPIO_PORTG(1),
        MCU_GPIO_PORTA(14),
        MCU_GPIO_UNDEF,
        MCU_GPIO_UNDEF
    },
    {
        MCU_GPIO_PORTD(3),
        MCU_GPIO_PORTG(7),
        MCU_GPIO_PORTF(5),
        MCU_GPIO_PORTD(11),
        MCU_GPIO_PORTF(0),
        MCU_GPIO_PORTB(1),
        MCU_GPIO_PORTE(5),
        MCU_GPIO_PORTC(13),
        MCU_GPIO_PORTB(3),
        MCU_GPIO_UNDEF,
        MCU_GPIO_PORTC(4),
        MCU_GPIO_PORTD(15),
        MCU_GPIO_PORTG(0),
        MCU_GPIO_PORTA(15),
        MCU_GPIO_UNDEF,
        MCU_GPIO_UNDEF
    },
    {
        MCU_GPIO_PORTD(9),
        MCU_GPIO_PORTG(6),
        MCU_GPIO_PORTB(8),
        MCU_GPIO_PORTB(15),
        MCU_GPIO_PORTD(4),
        MCU_GPIO_PORTB(0),
        MCU_GPIO_PORTE(3),
        MCU_GPIO_PORTB(7),
        MCU_GPIO_UNDEF,
        MCU_GPIO_PORTF(12),
        MCU_GPIO_PORTD(12),
        MCU_GPIO_PORTF(8),
        MCU_GPIO_PORTC(3),
        MCU_GPIO_PORTE(9),
        MCU_GPIO_UNDEF,
        MCU_GPIO_UNDEF
    },
    {
        MCU_GPIO_PORTD(1),
        MCU_GPIO_PORTG(9),
        MCU_GPIO_PORTB(14),
        MCU_GPIO_PORTD(0),
        MCU_GPIO_UNDEF,
        MCU_GPIO_PORTB(6),
        MCU_GPIO_PORTD(5),
        MCU_GPIO_PORTB(2),
        MCU_GPIO_PORTF(3),
        MCU_GPIO_PORTF(13),
        MCU_GPIO_UNDEF,
        MCU_GPIO_PORTF(2),
        MCU_GPIO_PORTC(2),
        MCU_GPIO_PORTE(8),
        MCU_GPIO_UNDEF,
        MCU_GPIO_UNDEF
    }
};

int
pps_configure_output(uint8_t pin, uint8_t func)
{
    uint32_t port = pin >> 4;
    uint32_t index = pin & 0xF;
    volatile uint32_t *ptr;

    if (func >= 16) {
        return -1;
    }

    ptr = (volatile uint32_t *) (PPS_BASE_ADDRESS +
                                 (port * PPS_PORT_SPACING) + index * 4);

    *ptr = func;
    return 0;
}

int
pps_configure_input(uint8_t pin, uint8_t func)
{
    uint8_t index = func >> 4;
    uint8_t val;

    if (index > 3) {
        return -1;
    }

    func &= 0xF;
    if (input_regs[index][func]  == NULL) {
        return -1;
    }

    for (val = 0; val < 16; ++val) {
        if (output_pins[index][val] == pin) {
            break;
        }
    }
    if (val == 16) {
        return -1;
    }

    *input_regs[index][func] = val;
    return 0;
}