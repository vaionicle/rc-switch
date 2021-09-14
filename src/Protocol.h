#ifndef _Protocol_h
#define _Protocol_h

#include "RCSwitch.h"

/* Protocol description format
 *
 * {
 *    Pulse length, 
 * 
 *    PreambleFactor,
 *    Preamble {high,low},
 * 
 *    HeaderFactor,
 *    Header {high,low},
 * 
 *    "0" bit {high,low},
 *    "1" bit {high,low},
 * 
 *    Inverted Signal,
 *    Guard time
 * }
 *
 * Pulse length: pulse duration (Te) in microseconds,
 *               for example 350
 * 
 * PreambleFactor: Number of high and low states to send
 *                 (One pulse = 2 states, in orther words, number of pulses is
 *                 ceil(PreambleFactor/2).)
 * Preamble: Pulse shape which defines a preamble bit.
 *           Sent ceil(PreambleFactor/2) times.
 *           For example, {1, 2} with factor 3 would send
 *      _    _   
 *     | |__| |__  (each horizontal bar has a duration of Te, vertical bars are ignored)
 * 
 * HeaderFactor: Number of times to send the header pulse.
 * Header: Pulse shape which defines a header (or "sync"/"clock") pulse.
 *           {1, 31} means one pulse of duration 1 Te high and 31 Te low
 *      _
 *     | |_______________________________ (don't count the vertical bars)
 *
 * "0" bit: pulse shape defining a data bit, which is a logical "0"
 *          {1, 3} means 1 pulse duration Te high level and 3 low
 *      _
 *     | |___
 *
 * "1" bit: pulse shape that defines the data bit, which is a logical "1"
 *          {3, 1} means 3 pulses with a duration of Te high level and 1 low
 *      ___
 *     |   |_
 *
 * (note: to form the state bit Z (Tri-State bit), two codes are combined)
 *
 * Inverted Signal: Signal inversion - if true the signal is inverted
 *                  replacing high to low in a transmitted / received packet
 *
 * Guard time: Separation time between two retries. It will be followed by the 
 *             next preamble of the next packet. In number of Te.
 *             e.g. 39 pulses of duration Te low level
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

	uint8_t PreambleFactor;
	HighLow Preamble;

	uint8_t HeaderFactor;
	HighLow Header;

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
	 * RCSwitch to change how it interprets any HighLow struct
	 * 
	 * FOO: It will then assume transmissions start with a low signal lasting
	 *
	 * FOO.high * pulseLength microseconds, followed by a high signal lasting
	 * FOO.low  * pulseLength microseconds.
	 */
	bool invertedSignal;

	uint16_t Guard;
};

#if defined(ESP8266) || defined(ESP32)
	static const VAR_ISR_ATTR Protocol proto[] = {
#else
	static const Protocol PROGMEM proto[] = {
#endif
	{ 350,  0, { 0, 0 }, 1, {   1, 31 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 01 (Princeton, PT-2240)
	{ 650,  0, { 0, 0 }, 1, {   1, 10 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 02
	{ 100,  0, { 0, 0 }, 1, {  30, 71 }, { 4, 11 }, { 9, 6 }, false,  0 }, // 03
	{ 380,  0, { 0, 0 }, 1, {   1,  6 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 04
	{ 500,  0, { 0, 0 }, 1, {   6, 14 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 05
	{ 450,  0, { 0, 0 }, 1, {  23,  1 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 06 (HT6P20B)
	{ 150,  0, { 0, 0 }, 1, {   2, 62 }, { 1,  6 }, { 6, 1 }, false,  0 }, // 07 (HS2303-PT, i. e. used in AUKEY Remote)
	{ 320,  0, { 0, 0 }, 1, {  36,  1 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 08 (Came 12bit, HT12E)
	{ 700,  0, { 0, 0 }, 1, {  32,  1 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 09 (Nice_Flo 12bit)
	{ 420,  0, { 0, 0 }, 1, {  60,  6 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 10 (V2 phoenix)
	{ 500,  2, { 3, 3 }, 0, {   0,  0 }, { 1,  2 }, { 2, 1 }, false, 37 }, // 11 (Nice_FloR-S 52bit)
	{ 400, 23, { 1, 1 }, 1, {   0,  9 }, { 2,  1 }, { 1, 2 }, false, 39 }, // 12 (Keeloq 64/66)
	{ 300,  6, { 2, 2 }, 3, {   8,  3 }, { 2,  2 }, { 3, 3 }, false,  0 }, // 13 test (CFM)
	{ 250, 12, { 4, 4 }, 0, {   0,  0 }, { 1,  1 }, { 2, 2 }, false,  0 }, // 14 test (StarLine)
	{ 500,  0, { 0, 0 }, 0, { 100,  1 }, { 1,  2 }, { 2, 1 }, false, 35 }, // 15
	{ 361,  0, { 0, 0 }, 1, {  52,  1 }, { 1,  3 }, { 3, 1 }, true,   0 }, // 16 (Einhell)
	{ 500,  0, { 0, 0 }, 1, {   1, 23 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 17 (InterTechno PAR-1000)
	{ 180,  0, { 0, 0 }, 1, {   1, 15 }, { 1,  1 }, { 1, 8 }, false,  0 }, // 18 (Intertechno ITT-1500)
	{ 350,  0, { 0, 0 }, 1, {   1,  2 }, { 0,  2 }, { 3, 2 }, false,  0 }, // 19 (Murcury)
	{ 150,  0, { 0, 0 }, 1, {  34,  3 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 20 (AC114)
	{ 360,  0, { 0, 0 }, 1, {  13,  4 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 21 (DC250)
	{ 650,  0, { 0, 0 }, 1, {   1, 10 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 22 (Mandolyn/Lidl TR-502MSV/RC-402/RC-402DX)
	{ 641,  0, { 0, 0 }, 1, { 115,  1 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 23 (Lidl TR-502MSV/RC-402 - Flavien)
	{ 620,  0, { 0, 0 }, 1, {   0, 64 }, { 0,  1 }, { 1, 0 }, false,  0 }, // 24 (Lidl TR-502MSV/RC701)
	{ 560,  0, { 0, 0 }, 1, {  16,  8 }, { 1,  1 }, { 1, 3 }, false,  0 }, // 25 (NEC)
	{ 385,  0, { 0, 0 }, 1, {   1, 17 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 26 (Arlec RC210)
	{ 188,  0, { 0, 0 }, 1, {   1, 31 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 27 (Zap, FHT-7901)
	{ 700,  1, { 0, 1 }, 1, { 116,  0 }, { 1,  2 }, { 2, 1 }, true,   0 }, // 28 (Quigg GT-7000) from @Tho85 https://github.com/sui77/rc-switch/pull/115
	{ 220,  0, { 0, 0 }, 1, {   1, 46 }, { 1,  6 }, { 1, 1 }, false,  2 }, // 29 (NEXA)
	{ 260,  0, { 0, 0 }, 1, {   1,  8 }, { 1,  4 }, { 4, 1 }, true,   0 }, // 30 (Anima)

	{ 400,  0, { 0, 0 }, 1, {   1,  1 }, { 1,  2 }, { 2, 1 }, false, 43 }, // 31 (Mertik Maxitrol G6R-H4T1)
	{ 365,  0, { 0, 0 }, 1, {  18,  1 }, { 3,  1 }, { 1, 3 }, true,   0 }, // 32 (1ByOne Doorbell) from @Fatbeard https://github.com/sui77/rc-switch/pull/277
	{ 340,  0, { 0, 0 }, 1, {  14,  4 }, { 1,  2 }, { 2, 1 }, false,  0 }, // 33 (Dooya Control DC2708L)
	{ 120,  0, { 0, 0 }, 1, {   1, 28 }, { 1,  3 }, { 3, 1 }, false,  0 }, // 34 DIGOO SD10
	{ 275,  0, { 0, 0 }, 1, {   1, 10 }, { 1,  1 }, { 1, 5 }, false, 37 }, // 35 (SmartWares/HomeEasy)
	{  16,  0, { 0, 0 }, 1, {  59, 19 }, { 4, 10 }, { 9, 3 }, false,  0 }  // 36 BREL Motor Blinds (DC-104)

	// { 300,  0, { 1, 33 }, { 1, 1 }, { 1, 4 }, { 1, 9 }, false, 0 }, // protocol 8 Home easy
	// {200, {3, 130}, {7, 16}, {3, 16}, false}, // protocol 8 Conrad RS-200 RX
	// {200, {130, 7}, {16, 7}, {16, 3}, true},  // protocol 9 Conrad RS-200 TX
	// {365, {18, 1}, {3, 1}, {1, 3}, true},     // protocol 10 (1ByOne Doorbell)
	// {270, {36, 1}, {1, 2}, {2, 1}, true},     // protocol 11 (HT12E)
	// {320, {36, 1}, {1, 2}, {2, 1}, true}      // protocol 12 (SM5212)
	// { 300, { 1, 19  }, {  1,  3 }, {  3,  1 }, false},     // protocol 13 (ft1211r, fan motor)
	// { 520, { 17, 1  }, {  7,  1 }, {  3,  1 }, true},      // protocol 14
	// //{ 520, { 16, 1  }, {  3,  1 }, {  7,  1 }, false}      // protocol 15
	// { 150, {  1, 31 }, {  1,  3 }, {  3,  1 }, false }     // protocol 8 (FHT-7901, uses type E encoding hence different from protocol 1)
	// { 365, { 18,  1 }, {  3,  1 }, {  1,  3 }, true },     // protocol 10 (1ByOne Doorbell)
};

enum {
   numProto = sizeof(proto) / sizeof(proto[0])
};

#endif