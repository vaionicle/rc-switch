
#include "Keyloq.h"

#define KeeLoq_NLF 0x3A5C742EUL

/**
  * Initilize Keeloq
  */
Keeloq::Keeloq() {
	_keyHigh = 0;
	_keyLow = 0;
}

/**
  * Set Keeloq 64 bit cypher key
  */
void Keeloq::SetKey(unsigned long keyHigh, unsigned long keyLow) {
	_keyHigh = keyHigh;
	_keyLow = keyLow;
}

/**
  * Get Key data
  */
unsigned long Keeloq::GetKey(bool HighLow) {
	if (HighLow) {
		return _keyHigh;
	}
	return _keyLow;
}

/**
  * Encrypt Keeloq 32 bit data
  */
unsigned long Keeloq::Encrypt(unsigned long data) {
	unsigned long x = data;
	unsigned long r;
	int keyBitNo, index;
	unsigned long keyBitVal,bitVal;

	for (r = 0; r < 528; r++) {
		keyBitNo = r & 63;
		if (keyBitNo < 32) {
		keyBitVal = bitRead(_keyLow,keyBitNo);
		} else {
		keyBitVal = bitRead(_keyHigh, keyBitNo - 32);
		}
		index = 1 * bitRead(x, 1) + 2 * bitRead(x, 9) + 4 * bitRead(x, 20) + 8 * bitRead(x, 26) + 16 * bitRead(x, 31);
		bitVal = bitRead(x, 0) ^ bitRead(x, 16) ^ bitRead(KeeLoq_NLF, index) ^ keyBitVal;
		x = (x >> 1) ^ bitVal << 31;
	}
	return x;
}

/**
  * Decrypt Keeloq 32 bit data
  */
unsigned long Keeloq::Decrypt(unsigned long data) {
	unsigned long x = data;
	unsigned long r;
	int keyBitNo, index;
	unsigned long keyBitVal,bitVal;

	for (r = 0; r < 528; r++) {
		keyBitNo = (15-r) & 63;
		if(keyBitNo < 32) {
		keyBitVal = bitRead(_keyLow,keyBitNo);
		} else {
		keyBitVal = bitRead(_keyHigh, keyBitNo - 32);
		}
		index = 1 * bitRead(x, 0) + 2 * bitRead(x, 8) + 4 * bitRead(x, 19) + 8 * bitRead(x, 25) + 16 * bitRead(x, 30);
		bitVal = bitRead(x, 31) ^ bitRead(x, 15) ^ bitRead(KeeLoq_NLF, index) ^ keyBitVal;
		x = (x << 1) ^ bitVal;
	}
	return x;
}

/**
  * Set Normal Learning Keeloq key
  */
void Keeloq::NormLearn(unsigned long FixSN) {
	unsigned long tmpFixSN;
	// заготовки для формируемого ключа
	unsigned long NewkeyHigh;
	unsigned long NewkeyLow;

	tmpFixSN = FixSN & 0x0FFFFFFF;
	tmpFixSN |= 0x20000000;
	NewkeyLow = Decrypt(tmpFixSN);
	tmpFixSN = FixSN & 0x0FFFFFFF;
	tmpFixSN |= 0x60000000;
	NewkeyHigh = Decrypt(tmpFixSN);
	_keyHigh = NewkeyHigh;
	_keyLow = NewkeyLow;
}

/**
  * Reflect a 32 bit package
  */
unsigned long Keeloq::ReflectPack(unsigned long PackSrc) {
	unsigned long long PackOut = 0;
	for (byte i = 0; i < 32; i++) {
		PackOut = PackOut << 1;
		if (PackSrc & 1) {
			PackOut = PackOut | 1; 
		}
		PackSrc = PackSrc >> 1;
	}
	return PackOut;
}