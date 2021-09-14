# rc-switch

Fork of [RC-SWITCH by @sui77](https://github.com/sui77/rc-switch)

[![arduino-library-badge](https://www.ardu-badge.com/badge/rc-switch.svg?)](https://www.ardu-badge.com/rc-switch)
[![Build Status](https://app.travis-ci.com/vaionicle/rc-switch.svg?branch=master)](https://app.travis-ci.com/vaionicle/rc-switch)

Use your Arduino or [Raspberry Pi](https://github.com/r10r/rcswitch-pi) to operate remote radio controlled devices

## Table of contents

- [rc-switch](#rc-switch)
  - [Table of contents](#table-of-contents)
  - [Download](#download)
  - [Wiki](#wiki)
  - [Info](#info)
    - [Send RC codes](#send-rc-codes)
    - [Receive and decode RC codes](#receive-and-decode-rc-codes)

## Download

https://github.com/vaionicle/rc-switch/releases/latest

rc-switch is also listed in the arduino library manager.

## Wiki

https://github.com/vaionicle/rc-switch/wiki

## Info

### Send RC codes

Use your Arduino or Raspberry Pi to operate remote radio controlled devices.
This will most likely work with all popular low cost power outlet sockets. If
yours doesn't work, you might need to adjust the pulse length.

All you need is a Arduino or Raspberry Pi, a 315/433MHz AM transmitter and one
or more devices with one of the supported chipsets:

- SC5262 / SC5272
- HX2262 / HX2272
- PT2262 / PT2272
- EV1527 / RT1527 / FP1527 / HS1527
- Intertechno outlets
- HT6P20X

### Receive and decode RC codes

Find out what codes your remote is sending. Use your remote to control your
Arduino.

All you need is an Arduino, a 315/433MHz AM receiver (altough there is no
instruction yet, yes it is possible to hack an existing device) and a remote
hand set.

For the Raspberry Pi, clone the https://github.com/ninjablocks/433Utils project to
compile a sniffer tool and transmission commands.
