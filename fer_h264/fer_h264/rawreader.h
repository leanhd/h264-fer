#pragma once

void initRawReader(unsigned char *RBSP,unsigned int size);
void initRawWriter(unsigned char *RBSP_write, unsigned int size);

unsigned int getRawBit();

bool more_rbsp_data();

void dumpWriteBuffer();

void skipRawBits(int N);
unsigned int peekRawBits(int N);

unsigned int getRawBits(int N);

bool writeRawBits(int N, unsigned int data);
void writeFlag(int flag);
void writeZeros(int N);
void writeOnes(int N);

unsigned int RBSPtoUINT(unsigned char *rbsp, int N);
void UINT_to_RBSP_size_known(unsigned long int uint_number, unsigned int size, unsigned char rbsp_result[4]);

//Decoder
extern unsigned int RBSP_current_byte;
extern unsigned int RBSP_current_bit;
extern unsigned int RBSP_total_size;
extern unsigned char *RBSP_data;

//Coder
extern unsigned int RBSP_write_current_byte;
extern unsigned int RBSP_write_current_bit;
extern unsigned int RBSP_write_total_size;
extern unsigned char *RBSP_write_data;
extern unsigned int bitcount;