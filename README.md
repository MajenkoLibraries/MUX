MUX
===

Task Manager based LED Multiplex library for
chipKIT™.

How it works
------------

The MUX library is a fully static class which ties
into the chipKIT API's *Task Manager* system. 

The *Task Manager* is a system which allows the periodic
execution of a function with fine grained (millisecond)
resolution and accuracy.

The MUX library hooks into this system to update the
display data to a multiplex of LEDs in any arrangement
you require at a frequency of 500Hz.  That is, one
column (anode) of data is rendered ever 2ms.

The library is *anode-based*, in that the pin entries
that are for the sake of convenience called anodes
in the library are scanned in order and the pins
which are nominated as cathodes have the data for the
current anode presented on them.  That doesn't mean
that you *have* to use the anodes as connections to
actual anodes, they could be used to drive cathodes
and the cathode pins could be used to drive anodes. This
is just purely the convention used throughout this
library and its documentation.

Using the library
-----------------

To use the library you must first attach a number of
pins as either anodes or cathodes.  You do this using
the functions:

    MUX::addAnode(pin_number);
    MUX::addCathode(pin_number);

Call the functions as many times as required.  The
order you call the functions define the order in 
which the anodes are scanned and the order the display
data is presented on the cathodes.

By default both the cathodes and the anodes will
be set HIGH when active (that is, they will *source*
current).  If you need either of them to *sink* current
(that is, be set LOW when active) you can invert the
functionality of the pins:

    MUX::invertAnode();
    MUX::invertCathode();

Throughout this document (and the library) any
references to anodes and cathodes as parameters to
a function refer to the *logical* anode or cathode
number in the order that you called the addAnode or
addCathode functions.  The pin referenced in the
first call to addAnode is anode 0.  The pin referenced
in the second call to addAnode is anode 1, and so on.

The library supports up to 16 anodes and up to 16 
cathodes.

Once you have the pins defined you can *start* the
display with the function:

    MUX::start();

At any time you can stop the display refreshing (and
detach the system from the task manager) with:

    MUX::stop();

To get some data into the display you can either do
it an LED at a time with the functions:

    MUX::on(anode, cathode);
    MUX::off(anode, cathode);

If you want to set an entire anode's worth of data at
once you can use:

    MUX::setRow(anode, value);

Numeric display
---------------

It is possible to bind individual LEDs together to
form 7-segment digital displays.  To do this you need
to create the digits in the system.  Calling the
*addDigit()* function will tell the library which pin
combinations (anode and cathode) refer to which of the
seven (or eight if you use the decimal point as well)
LEDs that make up the digit.

For instance, if you have a setup where a 7-segment
display is wired up to have the 7 main segmends A
through G (in standard 7-segment layout) on the pin
combinations (anode,cathode) [(2,2), (2,3), (2,4),
(2,5), (2,6), (2,7), (2,8)] you would call the
addDigit function like this:

    MUX::addDigit(2, 2, 2, 3, 2, 4, 2, 5,
        2, 6, 2, 7, 2, 8);

If you wanted the decimal point as well you would just
add the pin combination that refers to that LED as
the 15th and 16th parameters.

Digits are added in order from left to right.

You can set the content of any one digit using the
*setDigit()* function.  This function takes the digit
number (starting from 0 and progressing in the order
you added them) and the value to display.  The value
is made up of two parts packed into an 8 bit value.

The upper half of the value is called the *mode*.  This
defines how the digit will interpret the lower half
of the value.

There are currently four modes:

* 0. Normal digit display (0-F)
* 1. Decimal point digit display (0.-F.)
* 2. Special character
* 3. Special function

There are three special characters defined for 
mode 2.  The value portion corresponds to:

* 0. Degree sign
* 1. Minus sign
* 2. Equals sign

There is only one special function at the moment for
mode 3:

* 0. Clear the digit

Here are some hexadecimal examples and the results
on a digit:

* 0x04: 4
* 0x0E: E
* 0x1B: b.
* 0x17: 7.
* 0x21: -
* 0x22: =
* 0x30: (all segments off)

There is also a helper function which will take a
numeric value and format it across all the defined
digits.  This takes an integer value (negative or
positive) and displays the data as a decimal value
right justified on all the digits.

    MUX::setValue(value);

