
#ifndef _Protocol_h
#define _Protocol_h

#include "RCSwitch.h"

/* Format for protocol definitions:
 * {pulselength, Sync bit, "0" bit, "1" bit, invertedSignal}
 * 
 * pulselength: pulse length in microseconds, e.g. 350
 * Sync bit: {1, 31} means 1 high pulse and 31 low pulses
 *     (perceived as a 31*pulselength long pulse, total length of sync bit is
 *     32*pulselength microseconds), i.e:
 *      _
 *     | |_______________________________ (don't count the vertical bars)
 * "0" bit: waveform for a data bit of value "0", {1, 3} means 1 high pulse
 *     and 3 low pulses, total length (1+3)*pulselength, i.e:
 *      _
 *     | |___
 * "1" bit: waveform for a data bit of value "1", e.g. {3,1}:
 *      ___
 *     |   |_
 *
 * These are combined to form Tri-State bits when sending or receiving codes.
 */

/**
 * Description of a single pule, which consists of a high signal
 * whose duration is "high" times the base pulse length, followed
 * by a low signal lasting "low" times the base pulse length.
 * Thus, the pulse overall lasts (high+low)*pulseLength
 */
struct HighLow {
    uint8_t high;
    uint8_t low;
};

/**
 * A "protocol" describes how zero and one bits are encoded into high/low
 * pulses.
 */
struct Protocol {
    /** base pulse length in microseconds, e.g. 350 */
    uint16_t pulseLength;

    HighLow syncFactor;
    HighLow zero;
    HighLow one;

    /**
     * If true, interchange high and low logic levels in all transmissions.
     *
     * By default, RCSwitch assumes that any signals it sends or receives
     * can be broken down into pulses which start with a high signal level,
     * followed by a a low signal level. This is e.g. the case for the
     * popular PT 2260 encoder chip, and thus many switches out there.
     *
     * But some devices do it the other way around, and start with a low
     * signal level, followed by a high signal level, e.g. the HT6P20B. To
     * accommodate this, one can set invertedSignal to true, which causes
     * RCSwitch to change how it interprets any HighLow struct FOO: It will
     * then assume transmissions start with a low signal lasting
     * FOO.high*pulseLength microseconds, followed by a high signal lasting
     * FOO.low*pulseLength microseconds.
     */
    bool invertedSignal;
};

#if defined(ESP8266) || defined(ESP32)
    static const VAR_ISR_ATTR Protocol proto[] = {
#else
    static const Protocol PROGMEM proto[] = {
#endif
    { 350, {  1, 31 }, {  1,  3 }, {  3,  1 }, false },    // protocol 1
    { 650, {  1, 10 }, {  1,  2 }, {  2,  1 }, false },    // protocol 2
    { 100, { 30, 71 }, {  4, 11 }, {  9,  6 }, false },    // protocol 3
    { 380, {  1,  6 }, {  1,  3 }, {  3,  1 }, false },    // protocol 4
    { 500, {  6, 14 }, {  1,  2 }, {  2,  1 }, false },    // protocol 5
    { 450, { 23,  1 }, {  1,  2 }, {  2,  1 }, true },     // protocol 6 (HT6P20B)
    { 150, {  2, 62 }, {  1,  6 }, {  6,  1 }, false },    // protocol 7 (HS2303-PT, i. e. used in AUKEY Remote)
    { 200, {  3, 130}, {  7, 16 }, {  3,  16}, false},     // protocol 8 Conrad RS-200 RX
    { 200, { 130, 7 }, {  16, 7 }, { 16,  3 }, true},      // protocol 9 Conrad RS-200 TX
    { 365, { 18,  1 }, {  3,  1 }, {  1,  3 }, true },     // protocol 10 (1ByOne Doorbell)
    { 270, { 36,  1 }, {  1,  2 }, {  2,  1 }, true },     // protocol 11 (HT12E)
    { 320, { 36,  1 }, {  1,  2 }, {  2,  1 }, true }      // protocol 12 (SM5212)
};

enum {
   numProto = sizeof(proto) / sizeof(proto[0])
};

#endif