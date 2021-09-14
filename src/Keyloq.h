#ifndef _Keeloq_h
#define _Keeloq_h


class Keeloq {

	public:
		Keeloq();

		void SetKey(unsigned long keyHigh, unsigned long keyLow);
		unsigned long GetKey(bool HighLow);
		unsigned long Encrypt(unsigned long data);
		unsigned long Decrypt(unsigned long data);
		void NormLearn(unsigned long FixSN);
		unsigned long ReflectPack(unsigned long PackSrc);

	private:
		unsigned long _keyHigh;
		unsigned long _keyLow;
};

#endif