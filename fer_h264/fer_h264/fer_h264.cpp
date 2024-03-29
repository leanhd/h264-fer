// fer_h264.cpp : Defines the entry point for the console application.
//

#include <string>
#include "stdafx.h"
#include "nal.h"
#include "fileIO.h"
#include "rbsp_decoding.h"
#include "rbsp_IO.h"
#include "h264_globals.h"
#include "residual_tables.h"
#include "ref_frames.h"
#include "expgolomb.h"
#include "rbsp_encoding.h"
#include "openCL_functions.h"

#include "fer_h264.h"

using namespace std;

int trenBytes;
std::string ulaznaDatoteka;
std::string izlaznaDatoteka;
NALunit nu;

void decode()
{
	stream=fopen(ulaznaDatoteka.c_str(),"rb");
	yuvoutput = fopen(izlaznaDatoteka.c_str(),"wb");

	generate_residual_level_tables();
	InitNAL();

	NALunit nu;
	nu.rbsp_byte = new unsigned char[500000];

	unsigned long int ptr=0;
	while(1)
	{
		getNAL(&ptr, nu);

		if (nu.NumBytesInRBSP==0)
		{
			break;
		}

		RBSP_decode(nu);
	}		

	CloseNAL();
	fclose(stream);
	fclose(yuvoutput);
}

void NastaviEncode()
{
	if (frameCount == endFrame) return;
	if (ReadFromY4M() != -1) 
	{
		frameCount++;
		
		printf("Frame #%d\n", frameCount);
		writeToYUV();

		nu.nal_unit_type = selectNALUnitType();
		RBSP_encode(nu);
		
		trenBytes = nu.NumBytesInRBSP;
		writeNAL(nu);

		if (frameCount != endFrame) return;
	}

	CloseCL();
	CloseNAL();
	fclose(stream);
	fclose(yuvinput);
	fclose(yuvoutput);
}

void encode()
{
	stream = fopen(izlaznaDatoteka.c_str(), "wb");
	yuvinput = fopen(ulaznaDatoteka.c_str(), "rb");
	yuvoutput = fopen((izlaznaDatoteka+".yuv").c_str(),"wb");

	generate_residual_level_tables();
	init_expgolomb_UC_codes();
	InitNAL();
	InitCL();

	frameCount = 0;
	nu.rbsp_byte = new unsigned char[500000];

	nu.forbidden_zero_bit = 0;

	LoadY4MHeader();

	// write sequence parameter set:
	nu.nal_ref_idc = 1;			// non-zero for sps
	nu.nal_unit_type = NAL_UNIT_TYPE_SPS;
	RBSP_encode(nu);
	writeNAL(nu);

	// write picture paramater set:
	nu.nal_ref_idc = 1;			// non-zero for pps
	nu.nal_unit_type = NAL_UNIT_TYPE_PPS;
	RBSP_encode(nu);
	writeNAL(nu);

	nu.nal_ref_idc = 1;		// non-zero for reference
	while (ReadFromY4M() != -1)
	{		
		frameCount++;
		if (frameCount == startFrame) break;
	}

	printf("Frame #%d\n", frameCount);
	writeToYUV();

	nu.nal_unit_type = selectNALUnitType();
	RBSP_encode(nu);
	
	trenBytes = nu.NumBytesInRBSP;
	writeNAL(nu);

	if (frameCount != endFrame) return;

	CloseCL();
	CloseNAL();
	fclose(stream);
	fclose(yuvinput);
	fclose(yuvoutput);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//decode();
	encode();

	return 0;
}

inline 
String ^ ToManagedString(const char * pString) { 
 return Marshal::PtrToStringAnsi(IntPtr((char *) pString)); 
} 
 
inline 
const std::string ToStdString(String ^ strString) { 
 IntPtr ptrString = IntPtr::Zero; 
 std::string strStdString; 
 try { 
  ptrString = Marshal::StringToHGlobalAnsi(strString); 
  strStdString = (char *) ptrString.ToPointer(); 
 } 
 finally { 
  if (ptrString != IntPtr::Zero) { 
   Marshal::FreeHGlobal(ptrString); 
  } 
 } 
 return strStdString; 
} 


namespace fer_h264
{

	void Starter::PostaviParametre(int FrameStart, int FrameEnd, int qp, int OsnovnoPredvidanje, int VelicinaProzora, int ToleriranaGreska, int IntraSvakih)
	{
		startFrame = FrameStart;
		endFrame = FrameEnd;
		_qParameter = qp;
		BasicInterEncoding = OsnovnoPredvidanje;
		WindowSize = VelicinaProzora;
		MAXDIFF_SET = ToleriranaGreska;
		IntraEvery = IntraSvakih;
	}

	void Starter::PostaviUlazIzlaz(String ^% ulaz, String ^% izlaz)
	{
		ulaznaDatoteka = ToStdString(ulaz);
		izlaznaDatoteka = ToStdString(izlaz);
	}

	void Starter::PokreniKoder() 
	{
		currFrameCount = 0;
		for (int i = 0; i < 5; i++) brojTipova[i] = 0;
		encode();

	}

	void Starter::NastaviKoder() 
	{
		currFrameCount++;
		for (int i = 0; i < 5; i++) brojTipova[i] = 0;
		NastaviEncode();
	}

	void Starter::DohvatiStatistiku(int % brojTipova1, int % brojTipova2, int % brojTipova3, int % brojTipova4, int % brojTipova5, int % velicina, int % trajanje) 
	{
		brojTipova1 = brojTipova[0];
		brojTipova2 = brojTipova[1];
		brojTipova3 = brojTipova[2];
		brojTipova4 = brojTipova[3];
		brojTipova5 = brojTipova[4];
		velicina = trenBytes;
		trajanje = vrijeme;
	}

	void Starter::PokreniDekoder() 
	{
		decode();
	}
}