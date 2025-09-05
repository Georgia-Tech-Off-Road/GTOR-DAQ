# Introduction

Welcome to Georgia Tech Offroad's ("GTOR") Binary Buffer library. This library allows bit level storage and modification. The motivation behind the creation of this class was to store sensor data from our vehicle in the most concise way for transmission in an environment where data transfer for may be severly bottled necked (say, behind a hill). What differentiates Binary Buffer from another bit level storage device (say std::vector<bool>) is that it allows easy read and writes with templated function readValue<typename V>() and writeValue<typename V>(V val). In addition, specializations of these functions allow for writing and reading partial slices of a value of any variable size. These functions read from the last bit in storage (LSB) to the first (MSB). This allows for the encoding of only 11 bits of a 16 bit integer (for smaller storage size). For predictable results with integers make sure this code will run on a big-endian system, if not, use boost's endian library. 

# Things to Be Aware Of

## Constness

I have taken pains to ensure that this library is const-correct. However, there is one thing to be aware of. ``readValue<>()`` is const, however, it may modify (through ``readBit()``) two mutable variables: ``bits_read`` and ``read_cursor_pos``. This is done so that ``readValue()`` remains a const function, however, to be very clear, ``readValue<>()`` and ``writeValue<>()`` are *non-idempotent*.

## Big-Endian

This library assumes big-endian storage of integers. This allows for the intended storage of integers in the buffer. For example, if you have a 16-bit integer, but for would like to store 12 bits, the algorithm requires big-endian byte-order to select bits from LSB to MSB. You can acheive this with boost's [endian library](https://www.boost.org/doc/libs/1_88_0/libs/endian/doc/html/endian.html). 

## Testing
This library has extensive unit testing. Unit tests include the intended use with boost's endian library.

## Installation

This library is ready to use with either CMake or Arduino. To use with Arduino, download this library and add it to your libraries folder. For use with cmake, all you need to do is add this as subfolder to your project (using CMake's `add_subdirectory()`).