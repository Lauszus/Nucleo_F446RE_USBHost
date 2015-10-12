These libraries are necessary for the code to work with the [USB Host Shield library 2.0](https://github.com/felis/USB_Host_Shield_2.0).

The [Print](Print.cpp), [Stream](Stream.cpp), [itoa](itoa.c), [Printable](Printable.h), [WString](WString.cpp), [dtostrf](avr/dtostrf.c), and [pgmspace](avr/pgmspace.h) where all copied from the Arduino Due core.

The [newlib](newlib.c) is copied from: https://github.com/ObKo/stm32-cmake/tree/master/stm32-newlib.

The [SerialClass](SerialClass.cpp) library was created, so Arduino style serial code works.