#include "residual.h"
#include "rbsp_IO.h"
#include "h264_globals.h"
#include "headers_and_parameter_sets.h"
#include "h264_math.h"
#include "expgolomb.h"
#include "residual_tables.h"

//Coefficient levels
int i16x16DClevel[16];
int i16x16AClevel[16][16];
int Intra16x16DCLevel[16];
int Intra16x16ACLevel[16][16];
int level[16][16], LumaLevel[16][16];

int ChromaDCLevel[2][4];
int ChromaACLevel[2][4][16];

const int subMBNeighbours[16][2] = {
{ 5, 10}, { 0, 11}, { 7,  0}, { 2,  1},
{ 1, 14}, { 4, 15}, { 3,  4}, { 6,  5},
{13,  2}, { 8,  3}, {15,  8}, {10,  9},
{ 9,  6}, {12,  7}, {11, 12}, {14, 13}};

const int subMBNeighboursChroma[4][2] = {
{1, 2}, {0, 3}, {3, 0}, {2, 1}};

void clear_residual_structures()
{
	for (int i=0;i<16;i++)
	{
		i16x16DClevel[i]=0;
		Intra16x16DCLevel[i]=0;
		LumaDCLevel[i] = 0;
		for (int j=0;j<16;j++)
		{
			i16x16AClevel[i][j]=0;
			Intra16x16ACLevel[i][j]=0;
			level[i][j]=0;
			LumaLevel[i][j]=0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		ChromaDCLevel[0][i] = 0;
		ChromaDCLevel[1][i] = 0;
	}
}

int luma4x4BlkIdx, cb4x4BlkIdx;
int iCbCr;
int NumC8x8;
int i8x8, i4x4;

unsigned int CAVLC_level_tresholds[6]=
{
	0,	3,	6,	12,	24,	48
};

int inverse_block_mapping[16]=
{
	0, 1, 4, 5,
	2, 3, 6, 7,
	8, 9, 12, 13,
	10, 11, 14, 15
};

//TODO: Niz tipa mb_width*mb_height*16 integera NETOCNO, izrazeni u transform/colour blokovima
//Number of non-zero coefficients in luma and chroma blocks (integer from 0 to 16)
int **TotalCoeff_luma_array;
int **TotalCoeff_chroma_array[2];
void write_run_before_values_for_zerosLeft_7_to_max(int run_before)
{
	if (run_before<7)
	{
		writeRawBits(3, 7-run_before);
	}
	else
	{
		writeZeros(run_before-4);
		writeOnes(1);
	}
}

// Helper function for the process described in 9.2.1 step 6
bool allNeighbouringZero(int mbAddrN, int blkN)
{
	if (!invoked_for_ChromaACLevel && !invoked_for_ChromaDCLevel)
	{
		if ((CodedBlockPatternLumaArray[mbAddrN] & (1 << (blkN/4))) == 0)
		{
			return true;
		}
	}
	else
	{
		// See table 7-15
		if ((CodedBlockPatternChromaArray[mbAddrN] & 2) == 0)
		{
			return true;
		}
	}
	
	return false;
}

//Calculate the nC parameter as required when decoding residual data.
//TODO: Cb/Cr channel ambiguity with "luma_or_select_chroma"

int get_nC(int x, int y, int luma_or_select_chroma)
{
	int nA, nB;

	if (luma_or_select_chroma==0)
	{
		nA=((x-4)<0 || y<0)?-1:TotalCoeff_luma_array[x-4][y];
		nB=((x)<0 || (y-4)<0)?-1:TotalCoeff_luma_array[x][y-4];
	}
	else
	{
		nA=(((x-4)<0 || y<0))?-1:TotalCoeff_chroma_array[luma_or_select_chroma-1][x-4][y];
		nB=(((x)<0 || (y-4)<0))?-1:TotalCoeff_chroma_array[luma_or_select_chroma-1][x][y-4];
	}

	if(nA<0 && nB<0)
	{
		return 0;
	}
	
	if(nA>=0 && nB>=0)
	{
		return (nA+nB+1)/2;
	}
  
	if(nA>=0)
	{
		return nA;
	}
    else
	{
		return nB;
	}
}

void derivation_process_for_4x4_chroma_block_indices(int x, int y, int *chroma4x4BlkIdx)
{
	//Protiv norme!
	*chroma4x4BlkIdx = 2 * ( y / 4 ) + ( x / 4 );
}

void derivation_process_for_4x4_luma_block_indices(int x, int y, int *luma4x4BlkIdx)
{
	*luma4x4BlkIdx = 8 * ( y / 8 ) + 4 * ( x / 8 ) + 2 * ( ( y % 8 ) / 4 ) + ( ( x % 8 ) / 4 );
}


void derivation_process_for_neighbouring_locations(int xN, int yN, int *mbAddrN, int *xW, int *yW, int luma_or_chroma)
{
	int maxW, maxH;
	if (luma_or_chroma==LUMA)
	{
		maxW=maxH=16;
	}
	else
	{
		maxW = MbWidthC;
		maxH = MbHeightC;
	}

	if (xN<0 && yN>=0 && yN<maxH)
	{
		//return block A
		*mbAddrN=(CurrMbAddr - 1);
	}
	else if (yN<0 && xN>=0 && xN<maxW)
	{
		//return block B
		(*mbAddrN)= CurrMbAddr - sps.PicWidthInMbs;
	}
	else if (xN>=maxW && yN>=0 && yN<maxH)
	{
		// N/A
		*mbAddrN=-1;
	}
	else if (yN>=maxH)
	{
		// N/A
		*mbAddrN=-1;
	}
	else if (xN>=0 && xN<maxW && yN>=0 && yN<maxH)
	{
		*mbAddrN=CurrMbAddr;
	}
	else
	{
		printf("ERROR\n");
	}

	*xW = ( xN + maxW ) % maxW;
	*yW = ( yN + maxH ) % maxH;
}

void derivation_process_for_neighbouring_4x4_chroma_blocks(int chroma4x4BlkIdx, int *mbAddrA, int *mbAddrB, int *chroma4x4BlkIdxA, int *chroma4x4BlkIdxB)
{
	// A:
	if ((chroma4x4BlkIdx == 0) || (chroma4x4BlkIdx == 2))
	{
		if (CurrMbAddr % PicWidthInMbs == 0)
		{
			*mbAddrA = -1;
			*chroma4x4BlkIdxA = -1;
		}
		else
		{
			*mbAddrA = CurrMbAddr - 1;
			*chroma4x4BlkIdxA = subMBNeighboursChroma[chroma4x4BlkIdx][0];
		}
	}		
	else
	{
		*mbAddrA = CurrMbAddr;
		*chroma4x4BlkIdxA = subMBNeighboursChroma[chroma4x4BlkIdx][0];
	}
	
	// B:
	if (chroma4x4BlkIdx < 2)
	{
		if (CurrMbAddr < PicWidthInMbs)
		{
			*mbAddrB = -1;
			*chroma4x4BlkIdxB = -1;
		}
		else
		{
			*mbAddrB = CurrMbAddr - PicWidthInMbs;
			*chroma4x4BlkIdxB = subMBNeighboursChroma[chroma4x4BlkIdx][1];
		}
	}		
	else
	{
		*mbAddrB = CurrMbAddr;
		*chroma4x4BlkIdxB = subMBNeighboursChroma[chroma4x4BlkIdx][1];
	}
}





void derivation_process_for_neighbouring_4x4_luma_blocks(int luma4x4BlkIdx, int *mbAddrA, int *luma4x4BlkIdxA, int *mbAddrB, int *luma4x4BlkIdxB)
{
	// A:
	if ((luma4x4BlkIdx == 0) || (luma4x4BlkIdx == 2) ||
		(luma4x4BlkIdx == 8) || (luma4x4BlkIdx == 10))
	{
		if (CurrMbAddr % PicWidthInMbs == 0)
		{
			*mbAddrA = -1;
			*luma4x4BlkIdxA = -1;
		}
		else
		{
			*mbAddrA = CurrMbAddr - 1;
			*luma4x4BlkIdxA = subMBNeighbours[luma4x4BlkIdx][0];
		}
	}		
	else
	{
		*mbAddrA = CurrMbAddr;
		*luma4x4BlkIdxA = subMBNeighbours[luma4x4BlkIdx][0];
	}
	
	// B:
	if ((luma4x4BlkIdx == 0) || (luma4x4BlkIdx == 1) ||
		(luma4x4BlkIdx == 4) || (luma4x4BlkIdx == 5))
	{
		if (CurrMbAddr < PicWidthInMbs)
		{
			*mbAddrB = -1;
			*luma4x4BlkIdxB = -1;
		}
		else
		{
			*mbAddrB = CurrMbAddr - PicWidthInMbs;
			*luma4x4BlkIdxB = subMBNeighbours[luma4x4BlkIdx][1];
		}
	}		
	else
	{
		*mbAddrB = CurrMbAddr;
		*luma4x4BlkIdxB = subMBNeighbours[luma4x4BlkIdx][1];
	}
}

//Residual encoding



void residual_write()
{

	int startIdx=0;
	int endIdx=15;

	residual_luma_write(Intra16x16DCLevel, Intra16x16ACLevel, LumaLevel, startIdx, endIdx);

	//if( ChromaArrayType = = 1 | | ChromaArrayType = = 2 )
	//The only supported ChromaArrayType is 1

	NumC8x8 = 4 / (SubWidthC * SubHeightC );
	for (iCbCr=0; iCbCr<2; iCbCr++)
	{
		//Chroma DC residual present
		if ((CodedBlockPatternChroma & 3) && startIdx==0)
		{
			invoked_for_ChromaDCLevel=1;
			residual_block_cavlc_write(ChromaDCLevel[iCbCr],0,4*NumC8x8-1, 4*NumC8x8);
			invoked_for_ChromaDCLevel=0;
		}
	}

	for (iCbCr=0;iCbCr<2;iCbCr++)
	{
		for (i8x8=0;i8x8<NumC8x8;i8x8++)
		{
			for (cb4x4BlkIdx=0; cb4x4BlkIdx<4; cb4x4BlkIdx++)
			{
				//Chroma AC residual present
				if ((CodedBlockPatternChroma & 2) && endIdx>0)
				{
					invoked_for_ChromaACLevel=1;
					residual_block_cavlc_write(ChromaACLevel[iCbCr][i8x8*4+cb4x4BlkIdx],((startIdx-1)>0?(startIdx-1):0),endIdx-1,15);
					invoked_for_ChromaACLevel=0;
				}
			}
		}
	}
}

void residual_luma_write(int i16x16DClevel[16], int i16x16AClevel[16][16], int level[16][16], int startIdx, int endIdx)
{
	if (startIdx == 0 && MbPartPredMode(mb_type, 0) == Intra_16x16)
	{
		invoked_for_Intra16x16DCLevel=1;
		residual_block_cavlc_write(i16x16DClevel, 0, 15, 16);
		invoked_for_Intra16x16DCLevel=0;
	}

	
	for (i8x8=0;i8x8<4;i8x8++)
	{
		for (i4x4=0;i4x4<4;i4x4++)
		{
			if (CodedBlockPatternLuma & (1<<i8x8))
			{
				if (endIdx>0 && MbPartPredMode(mb_type, 0) == Intra_16x16)
				{
					invoked_for_Intra16x16ACLevel=1;
					residual_block_cavlc_write(i16x16AClevel[i8x8*4+i4x4],(((startIdx-1)>0)?(startIdx-1):0),endIdx-1,15);
					invoked_for_Intra16x16ACLevel=0;
				}
				else
				{
					invoked_for_LumaLevel=1;
					residual_block_cavlc_write(level[i8x8*4 + i4x4],startIdx,endIdx,16);
					invoked_for_LumaLevel=0;
				}
			}
		}
	}
}

void residual_block_cavlc_write(int coeffLevel[16], int startIdx, int endIdx, int maxNumCoeff)
{
	//Some variables here are redundant or leftovers from the decoder. TODO: Clear up the code
	int TotalCoeff, TrailingOnes, zerosLeft;
	int level[16], run[16], nC;
	int total_zeros;

	//Preparing the additional data
	TotalCoeff=0;
	TrailingOnes=0;
	total_zeros=0;

	bool only_ones=true;

	for (int i=maxNumCoeff-1;i>=0;i--)
	{
		if (coeffLevel[i]!=0)
		{

			run[TotalCoeff]=0;
			for (int j=i-1;j>=0;j--)
			{
				if (coeffLevel[j]==0)
				{
					run[TotalCoeff]++;
				}
				else
				{
					break;
				}
			}

			if ((coeffLevel[i]==1 || coeffLevel[i]==-1) && TrailingOnes<3 && only_ones==true)
			{
				TrailingOnes++;
			}
			else
			{
				only_ones=false;
			}

			level[TotalCoeff] = coeffLevel[i];
			TotalCoeff++;
		}
		else
		{
			if (TotalCoeff>0)
			{
				total_zeros++;
			}
		}
	}
	
	int nA, nB;
	int mbAddrA, mbAddrB, luma4x4BlkIdxA, luma4x4BlkIdxB;
	int chroma4x4BlkIdxA, chroma4x4BlkIdxB;
	int blkA, blkB;


	if (invoked_for_ChromaDCLevel==1)
	{
		nC=-1;
	}
	//All other "level types" require normal calculation of nC
	else
	{
		//Determine the exact luma4x4BlkIdx for the current luma block.
		//If this isn't a luma block, no harm is done by editing this value. 
		if (invoked_for_Intra16x16DCLevel==1)
		{
			luma4x4BlkIdx=0;
		}
		else
		{
			//Luma blocks are being received in a specific "intra4x4 scan order". 
			//This does not apply to chroma blocks.
			luma4x4BlkIdx=i8x8*4+i4x4;//to_4x4_luma_block[i8x8*4+i4x4];
		}

		if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
		{
			derivation_process_for_neighbouring_4x4_luma_blocks(luma4x4BlkIdx, &mbAddrA, &luma4x4BlkIdxA, &mbAddrB, &luma4x4BlkIdxB);
			blkA = luma4x4BlkIdxA;
			blkB = luma4x4BlkIdxB;
		}
		else if (invoked_for_ChromaACLevel)
		{
			derivation_process_for_neighbouring_4x4_chroma_blocks(cb4x4BlkIdx, &mbAddrA, &mbAddrB, &chroma4x4BlkIdxA, &chroma4x4BlkIdxB);
			blkA = chroma4x4BlkIdxA;
			blkB = chroma4x4BlkIdxB;
		}

		int availableFlagA=1;
		int availableFlagB=1;

		if (mbAddrA<0)
		{
			availableFlagA=0;
		}
		else if ((mb_type_array[mbAddrA]==P_Skip) || allNeighbouringZero(mbAddrA, blkA))
		{
			nA=0;
		}
		else
		{
			if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
			{
				nA=totalcoeff_array_luma[mbAddrA][luma4x4BlkIdxA];
			}
			else
			{
				nA=totalcoeff_array_chroma[iCbCr][mbAddrA][chroma4x4BlkIdxA];
			}
		}

		if (mbAddrB<0)
		{
			availableFlagB=0;
		}
		else if ((mb_type_array[mbAddrB]==P_Skip) || allNeighbouringZero(mbAddrB, blkB))
		{
			nB=0;
		}
		else
		{
			if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
			{
				nB=totalcoeff_array_luma[mbAddrB][luma4x4BlkIdxB];
			}
			else
			{
				nB=totalcoeff_array_chroma[iCbCr][mbAddrB][chroma4x4BlkIdxB];
			}
		}

		if (availableFlagA==1 && availableFlagB==1)
		{
			nC=( nA + nB + 1 )>>1;
		}
		else if (availableFlagA==1 && availableFlagB==0)
		{
			nC=nA;
		}
		else if (availableFlagA==0 && availableFlagB==1)
		{
			nC=nB;
		}
		else
		{
			nC=0;
		}
	}


	//Save the number of AC coefficients in the array (luma4x4BlkIdx has already been transformed)
	if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
	{
		totalcoeff_array_luma[CurrMbAddr][luma4x4BlkIdx]=TotalCoeff;
	}
	else
	{
		//Do not remember values for Chroma DC level (or any other DC values)
		if (invoked_for_ChromaDCLevel==0)
		{
			totalcoeff_array_chroma[iCbCr][CurrMbAddr][cb4x4BlkIdx]=TotalCoeff;
		}
	}

	//Write coeff_token
	//Inputs to this process: TotalCoeff, TrailingOnes
	if (nC==-1)
	{
		writeRawBits(CoeffTokenCodeTableCoder_ChromaDC_length[TotalCoeff][TrailingOnes], CoeffTokenCodeTableCoder_ChromaDC_data_int[TotalCoeff][TrailingOnes]);
	}
	else if (nC==0 || nC==1)
	{
		writeRawBits(CoeffTokenCodesCoder_nC_0_to_2_length[TotalCoeff][TrailingOnes],CoeffTokenCodesCoder_nC_0_to_2_data_int[TotalCoeff][TrailingOnes]);
	}
	else if (nC==2 || nC==3)
	{
		writeRawBits(CoeffTokenCodesCoder_nC_2_to_4_length[TotalCoeff][TrailingOnes],CoeffTokenCodesCoder_nC_2_to_4_data_int[TotalCoeff][TrailingOnes]);
	}
	else if (nC>3 && nC<8)
	{
		writeRawBits(CoeffTokenCodesCoder_nC_4_to_8_length[TotalCoeff][TrailingOnes],CoeffTokenCodesCoder_nC_4_to_8_data_int[TotalCoeff][TrailingOnes]);
	}
	else
	{
		writeRawBits(CoeffTokenCodesCoder_nC_8_to_max_length[TotalCoeff][TrailingOnes],CoeffTokenCodesCoder_nC_8_to_max_data_int[TotalCoeff][TrailingOnes]);
	}

	if (TotalCoeff>0)
	{

		//Write trailing_ones_sign_flags and levels
		//Input to this process: array of levels[16], TotalCoeff, TrailingOnes
		unsigned int currentVLC=0, tresholdCounter=0;

		int suffixLength;
		int levelCode;
		if ((TotalCoeff > 10) && (TrailingOnes < 3))
		{
			suffixLength = 1;
		}
		else
		{
			suffixLength = 0;
		}

		for (int i=0;i<TotalCoeff;i++)
		{
			if (i<TrailingOnes)
			{
				int trailing_ones_sign_flag = (1-level[i]) >> 1;
				writeFlag(trailing_ones_sign_flag);
			}
			else
			{				
				if (level[i] < 0)
				{
					levelCode = -(level[i] << 1) - 1;
				}
				else
				{
					levelCode = (level[i] << 1) - 2;
				}

				if ((i == TrailingOnes) && (TrailingOnes < 3))
				{
					levelCode -= 2;
				}

				writeZeros(levelcode_to_outputstream[levelCode][suffixLength][1]);
				writeOnes(1);
				if ((suffixLength > 0) || (levelcode_to_outputstream[levelCode][suffixLength][1] >= 14))
				{
					writeRawBits(levelcode_to_outputstream[levelCode][suffixLength][2], levelcode_to_outputstream[levelCode][suffixLength][3]);
				}

				if (suffixLength == 0)
				{
					suffixLength = 1;
				}
				if ((ABS(level[i]) > (3 << (suffixLength - 1))) && (suffixLength < 6))
				{
					++suffixLength;
				}				
			}
		}

		//Write total_zeros
		//Input to this process: total_zeros, TotalCoeff
		//Output is zerosLeft (either 0 or total_zeros)
		if(TotalCoeff < (endIdx - startIdx + 1))
		{
			
			if (invoked_for_ChromaDCLevel==0)
			{
				writeRawBits(TotalZerosCodeTableCoder_4x4_length[TotalCoeff-1][total_zeros], TotalZerosCodeTableCoder_4x4_data_int[TotalCoeff-1][total_zeros]);
			}
			else
			{
				writeRawBits(TotalZerosCodeTableCoder_ChromaDC_length[TotalCoeff-1][total_zeros],TotalZerosCodeTableCoder_ChromaDC_data_int[TotalCoeff-1][total_zeros]);
			}
			zerosLeft=total_zeros;
		}
		else
		{
			zerosLeft=0;
		}


		//Write run_before
		//Inputs to this process: zerosLeft, run[16]
		for(int j=0; j<TotalCoeff-1; j++)
		{
			if (zerosLeft>0)
			{
			  if (zerosLeft>6)
			  {
				  //Umjesto tablice run_before za zerosLeft>6
				  write_run_before_values_for_zerosLeft_7_to_max(run[j]);
			  }
			  else
			  {
				  writeRawBits(RunBeforeCodeTableCoder_length[zerosLeft-1][run[j]],RunBeforeCodeTableCoder_data_int[zerosLeft-1][run[j]]);
			  }
			}

			zerosLeft=zerosLeft-run[j];
		}
	}
}

/*
	This function is simulating the process of residual writing in order to measure the number of bits written to the
	output bitstream.
*/

unsigned int residual_block_cavlc_size(int coeffLevel[16], int startIdx, int endIdx, int maxNumCoeff)
{

	unsigned int residualSize=0;

	int TotalCoeff, TrailingOnes, zerosLeft;
	int level[16], run[16], nC;
	int total_zeros;

	TotalCoeff=0;
	TrailingOnes=0;
	total_zeros=0;

	bool only_ones=true;

	for (int i=maxNumCoeff-1;i>=0;i--)
	{
		if (coeffLevel[i]!=0)
		{

			run[TotalCoeff]=0;
			for (int j=i-1;j>=0;j--)
			{
				if (coeffLevel[j]==0)
				{
					run[TotalCoeff]++;
				}
				else
				{
					break;
				}
			}

			if ((coeffLevel[i]==1 || coeffLevel[i]==-1) && TrailingOnes<3 && only_ones==true)
			{
				TrailingOnes++;
			}
			else
			{
				only_ones=false;
			}

			level[TotalCoeff] = coeffLevel[i];
			TotalCoeff++;
		}
		else
		{
			if (TotalCoeff>0)
			{
				total_zeros++;
			}
		}
	}
	
	int nA, nB;
	int mbAddrA, mbAddrB, luma4x4BlkIdxA, luma4x4BlkIdxB;
	int chroma4x4BlkIdxA, chroma4x4BlkIdxB;
	int blkA, blkB;


	if (invoked_for_ChromaDCLevel==1)
	{
		nC=-1;
	}
	else
	{
		if (invoked_for_Intra16x16DCLevel==1)
		{
			luma4x4BlkIdx=0;
		}
		else
		{
			luma4x4BlkIdx=i8x8*4+i4x4;//to_4x4_luma_block[i8x8*4+i4x4];
		}

		if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
		{
			derivation_process_for_neighbouring_4x4_luma_blocks(luma4x4BlkIdx, &mbAddrA, &luma4x4BlkIdxA, &mbAddrB, &luma4x4BlkIdxB);
			blkA = luma4x4BlkIdxA;
			blkB = luma4x4BlkIdxB;
		}
		else if (invoked_for_ChromaACLevel)
		{
			derivation_process_for_neighbouring_4x4_chroma_blocks(cb4x4BlkIdx, &mbAddrA, &mbAddrB, &chroma4x4BlkIdxA, &chroma4x4BlkIdxB);
			blkA = chroma4x4BlkIdxA;
			blkB = chroma4x4BlkIdxB;
		}

		int availableFlagA=1;
		int availableFlagB=1;

		if (mbAddrA<0)
		{
			availableFlagA=0;
		}
		else if ((mb_type_array[mbAddrA]==P_Skip) || allNeighbouringZero(mbAddrA, blkA))
		{
			nA=0;
		}
		else
		{
			if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
			{
				nA=totalcoeff_array_luma[mbAddrA][luma4x4BlkIdxA];
			}
			else
			{
				nA=totalcoeff_array_chroma[iCbCr][mbAddrA][chroma4x4BlkIdxA];
			}
		}

		if (mbAddrB<0)
		{
			availableFlagB=0;
		}
		else if ((mb_type_array[mbAddrB]==P_Skip) || allNeighbouringZero(mbAddrB, blkB))
		{
			nB=0;
		}
		else
		{
			if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
			{
				nB=totalcoeff_array_luma[mbAddrB][luma4x4BlkIdxB];
			}
			else
			{
				nB=totalcoeff_array_chroma[iCbCr][mbAddrB][chroma4x4BlkIdxB];
			}
		}

		if (availableFlagA==1 && availableFlagB==1)
		{
			nC=( nA + nB + 1 )>>1;
		}
		else if (availableFlagA==1 && availableFlagB==0)
		{
			nC=nA;
		}
		else if (availableFlagA==0 && availableFlagB==1)
		{
			nC=nB;
		}
		else
		{
			nC=0;
		}
	}
	if (nC==-1)
	{
		residualSize+=CoeffTokenCodeTableCoder_ChromaDC_length[TotalCoeff][TrailingOnes];
	}
	else if (nC==0 || nC==1)
	{
		residualSize+=CoeffTokenCodesCoder_nC_0_to_2_length[TotalCoeff][TrailingOnes];
	}
	else if (nC==2 || nC==3)
	{
		residualSize+=CoeffTokenCodesCoder_nC_2_to_4_length[TotalCoeff][TrailingOnes];
	}
	else if (nC>3 && nC<8)
	{
		residualSize+=CoeffTokenCodesCoder_nC_4_to_8_length[TotalCoeff][TrailingOnes];
	}
	else
	{
		residualSize+=CoeffTokenCodesCoder_nC_8_to_max_length[TotalCoeff][TrailingOnes];
	}

	//Save the number of AC coefficients in the array (luma4x4BlkIdx has already been transformed)
	if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
	{
		totalcoeff_array_luma[CurrMbAddr][luma4x4BlkIdx]=TotalCoeff;
	}
	else
	{
		//Do not remember values for Chroma DC level (or any other DC values)
		if (invoked_for_ChromaDCLevel==0)
		{
			totalcoeff_array_chroma[iCbCr][CurrMbAddr][cb4x4BlkIdx]=TotalCoeff;
		}
	}

	if (TotalCoeff>0)
	{
		unsigned int currentVLC=0, tresholdCounter=0;

		int suffixLength;
		int levelCode;
		if ((TotalCoeff > 10) && (TrailingOnes < 3))
		{
			suffixLength = 1;
		}
		else
		{
			suffixLength = 0;
		}

		for (int i=0;i<TotalCoeff;i++)
		{
			if (i<TrailingOnes)
			{
				int trailing_ones_sign_flag = (1-level[i]) >> 1;
				residualSize++;
			}
			else
			{				
				if (level[i] < 0)
				{
					levelCode = -(level[i] << 1) - 1;
				}
				else
				{
					levelCode = (level[i] << 1) - 2;
				}

				if ((i == TrailingOnes) && (TrailingOnes < 3))
				{
					levelCode -= 2;
				}

				residualSize+=levelcode_to_outputstream[levelCode][suffixLength][1]+1;
				if ((suffixLength > 0) || (levelcode_to_outputstream[levelCode][suffixLength][1] >= 14))
				{
					residualSize+=levelcode_to_outputstream[levelCode][suffixLength][2];
				}

				if (suffixLength == 0)
				{
					suffixLength = 1;
				}
				if ((ABS(level[i]) > (3 << (suffixLength - 1))) && (suffixLength < 6))
				{
					++suffixLength;
				}				
			}
		}

		if(TotalCoeff < (endIdx - startIdx + 1))
		{
			
			if (invoked_for_ChromaDCLevel==0)
			{
				residualSize+=TotalZerosCodeTableCoder_4x4_length[TotalCoeff-1][total_zeros];
			}
			else
			{
				residualSize+=TotalZerosCodeTableCoder_ChromaDC_length[TotalCoeff-1][total_zeros];
			}
			zerosLeft=total_zeros;
		}
		else
		{
			zerosLeft=0;
		}

		for(int j=0; j<TotalCoeff-1; j++)
		{
			if (zerosLeft>0)
			{
			  if (zerosLeft>6)
			  {
					if (run[j]<7)
					{
						residualSize+=3;
					}
					else
					{
						residualSize+=run[j]-4+1;
					}
			  }
			  else
			  {
				  residualSize+=RunBeforeCodeTableCoder_length[zerosLeft-1][run[j]];
			  }
			}

			zerosLeft=zerosLeft-run[j];
		}
	}

	return residualSize;
}

//Residual decoding by the norm

void residual(int startIdx, int endIdx)
{
	residual_luma(i16x16DClevel, i16x16AClevel, level, startIdx, endIdx);

	for (int i=0;i<16;i++)
	{
		Intra16x16DCLevel[i]=i16x16DClevel[i];
		for (int j=0;j<16;j++)
		{
			LumaLevel[i][j] = level[i][j];
			Intra16x16ACLevel[i][j]=i16x16AClevel[i][j];
		}
	}

	//if( ChromaArrayType = = 1 | | ChromaArrayType = = 2 )
	//The only supported ChromaArrayType is 1

	NumC8x8 = 4 / (SubWidthC * SubHeightC );
	for (iCbCr=0; iCbCr<2; iCbCr++)
	{
		//Chroma DC residual present
		if ((CodedBlockPatternChroma & 3) && startIdx==0)
		{
			invoked_for_ChromaDCLevel=1;
			residual_block_cavlc(ChromaDCLevel[iCbCr],0,4*NumC8x8-1, 4*NumC8x8);
			invoked_for_ChromaDCLevel=0;
		}
		else
		{
			for (int i=0;i<4*NumC8x8;i++)
			{
				ChromaDCLevel[iCbCr][i]=0;
			}
		}
	}

	for (iCbCr=0;iCbCr<2;iCbCr++)
	{
		for (i8x8=0;i8x8<NumC8x8;i8x8++)
		{
			for (cb4x4BlkIdx=0; cb4x4BlkIdx<4; cb4x4BlkIdx++)
			{
				//Chroma AC residual present
				if ((CodedBlockPatternChroma & 2) && endIdx>0)
				{
					invoked_for_ChromaACLevel=1;
					residual_block_cavlc(ChromaACLevel[iCbCr][i8x8*4+cb4x4BlkIdx],((startIdx-1)>0?(startIdx-1):0),endIdx-1,15);
					invoked_for_ChromaACLevel=0;
				}
				else
				{
					totalcoeff_array_chroma[iCbCr][CurrMbAddr][cb4x4BlkIdx]=0;
					for (int i=0;i<15;i++)
					{
						ChromaACLevel[iCbCr][i8x8*4+cb4x4BlkIdx][i]=0;						
					}
				}
			}
		}
	}
}

void residual_luma(int i16x16DClevel[16], int i16x16AClevel[16][16], int level[16][16], int startIdx, int endIdx)
{
	if (startIdx == 0 && MbPartPredMode(mb_type, 0) == Intra_16x16)
	{
		invoked_for_Intra16x16DCLevel=1;
		residual_block_cavlc(i16x16DClevel, 0, 15, 16);
		invoked_for_Intra16x16DCLevel=0;
	}

	for (i8x8=0;i8x8<4;i8x8++)
	{
		for (i4x4=0;i4x4<4;i4x4++)
		{
			if (CodedBlockPatternLuma & (1<<i8x8))
			{
				if (endIdx>0 && MbPartPredMode(mb_type, 0) == Intra_16x16)
				{
					invoked_for_Intra16x16ACLevel=1;
					residual_block_cavlc(i16x16AClevel[i8x8*4+i4x4],(((startIdx-1)>0)?(startIdx-1):0),endIdx-1,15);
					invoked_for_Intra16x16ACLevel=0;
				}
				else
				{
					invoked_for_LumaLevel=1;
					residual_block_cavlc(level[i8x8*4 + i4x4],startIdx,endIdx,16);
					invoked_for_LumaLevel=0;
				}
			}
			else if (MbPartPredMode(mb_type, 0)==Intra_16x16)
			{
				totalcoeff_array_luma[CurrMbAddr][i8x8*4+i4x4]=0;
				for (int i=0;i<15;i++)
				{
					i16x16AClevel[i8x8*4 + i4x4][i] = 0;
				}
			}
			else
			{
				totalcoeff_array_luma[CurrMbAddr][i8x8*4+i4x4]=0;
				for (int i=0;i<16;i++)
				{
					level[i8x8*4 + i4x4][i] = 0;
				}
			}
		}
	}
}

void residual_block_cavlc(int coeffLevel[16], int startIdx, int endIdx, int maxNumCoeff)
{

	int TotalCoeff, TrailingOnes, level_suffix, levelCode, zerosLeft;
	int coeff_token,suffixLength, trailing_ones_sign_flag, level[16],run_before, run[16], coeffNum, nC;

	for (int i=0;i<maxNumCoeff;i++)
	{
		coeffLevel[i]=0;
	}

	int nA, nB;
	int mbAddrA, mbAddrB, luma4x4BlkIdxA, luma4x4BlkIdxB;
	int chroma4x4BlkIdxA, chroma4x4BlkIdxB;
	int blkA, blkB;


	/*
	– If the CAVLC parsing process is invoked for ChromaDCLevel, nC is derived as follows.
	– If ChromaArrayType is equal to 1, nC is set equal to −1,
	*/
	if (invoked_for_ChromaDCLevel==1)
	{
		nC=-1;
	}
	//All other "level types" require normal calculation of nC
	else
	{
		//Determine the exact luma4x4BlkIdx for the current luma block.
		//If this isn't a luma block, no harm is done by editing this value. 
		if (invoked_for_Intra16x16DCLevel==1)
		{
			luma4x4BlkIdx=0;
		}
		else
		{
			//Luma blocks are being received in a specific "intra4x4 scan order". 
			//This does not apply to chroma blocks.
			luma4x4BlkIdx=i8x8*4+i4x4;//to_4x4_luma_block[i8x8*4+i4x4];
		}

		if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
		{
			derivation_process_for_neighbouring_4x4_luma_blocks(luma4x4BlkIdx, &mbAddrA, &luma4x4BlkIdxA, &mbAddrB, &luma4x4BlkIdxB);
			blkA = luma4x4BlkIdxA;
			blkB = luma4x4BlkIdxB;
		}
		else if (invoked_for_ChromaACLevel)
		{
			derivation_process_for_neighbouring_4x4_chroma_blocks(cb4x4BlkIdx, &mbAddrA, &mbAddrB, &chroma4x4BlkIdxA, &chroma4x4BlkIdxB);
			blkA = chroma4x4BlkIdxA;
			blkB = chroma4x4BlkIdxB;
		}

		int availableFlagA=1;
		int availableFlagB=1;

		if (mbAddrA<0)
		{
			availableFlagA=0;
		}
		else if ((mb_type_array[mbAddrA]==P_Skip) || allNeighbouringZero(mbAddrA, blkA))
		{
			nA=0;
		}
		else
		{
			if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
			{
				nA=totalcoeff_array_luma[mbAddrA][luma4x4BlkIdxA];
			}
			else
			{
				nA=totalcoeff_array_chroma[iCbCr][mbAddrA][chroma4x4BlkIdxA];
			}
		}

		if (mbAddrB<0)
		{
			availableFlagB=0;
		}
		else if ((mb_type_array[mbAddrB]==P_Skip) || allNeighbouringZero(mbAddrB, blkB))
		{
			nB=0;
		}
		else
		{
			if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
			{
				nB=totalcoeff_array_luma[mbAddrB][luma4x4BlkIdxB];
			}
			else
			{
				nB=totalcoeff_array_chroma[iCbCr][mbAddrB][chroma4x4BlkIdxB];
			}
		}

		if (availableFlagA==1 && availableFlagB==1)
		{
			nC=( nA + nB + 1 )>>1;
		}
		else if (availableFlagA==1 && availableFlagB==0)
		{
			nC=nA;
		}
		else if (availableFlagA==0 && availableFlagB==1)
		{
			nC=nB;
		}
		else
		{
			nC=0;
		}
	}

	//nC Value as defined by the h264_vlc.pdf document:
	/*
		If blocks U and L are available (i.e. in the same coded slice), N = (Nu + NL)/2
		If only block U is available, N=NU ; if only block L is available, N=NL ; if neither is available, N=0.
	*/
	//We are using nA and nB (symbols used in norm, page 240) for Nu and NL, and nC for N.
	//The following code decodes the coeff_token symbol using CAVLC tables which were generated at the beginning of the program.

	if (nC==-1)
	{
		coeff_token=cavlc_table_decode(CoeffTokenCodeTable_ChromaDC);
	}
	else if (nC==0 || nC==1)
	{
		coeff_token=cavlc_table_decode(CoeffTokenCodeTable[0]); 
	}
	else if (nC==2 || nC==3)
	{
		coeff_token=cavlc_table_decode(CoeffTokenCodeTable[1]);
	}
	else if (nC>3 && nC<8)
	{
		coeff_token=cavlc_table_decode(CoeffTokenCodeTable[2]);
	}
	else
	{
		coeff_token=cavlc_table_decode(CoeffTokenCodeTable[3]);
	}

	//Extracting TotalCoeff and TrailingOnes from coeff_token symbol.
	//This operation is defined by the norm. No it isn't.
	TotalCoeff=coeff_token/4;
	TrailingOnes=coeff_token&3;

	if (TotalCoeff > 16)
	{
		printf("Fatal error: Unexpected TotalCoeff value (%d)\n", TotalCoeff);
		printf("Frame #%d, CurrMbAddr = %d\n", frameCount, CurrMbAddr);
		system("pause");
		exit(1);
	}

	//Save the number of AC coefficients in the array (luma4x4BlkIdx has already been transformed)
	if (invoked_for_Intra16x16DCLevel || invoked_for_Intra16x16ACLevel || invoked_for_LumaLevel)
	{
		totalcoeff_array_luma[CurrMbAddr][luma4x4BlkIdx]=TotalCoeff;
	}
	else
	{
		//Do not remember values for Chroma DC level (or any other DC values)
		if (invoked_for_ChromaDCLevel==0)
		{
			totalcoeff_array_chroma[iCbCr][CurrMbAddr][cb4x4BlkIdx]=TotalCoeff;
		}
	}


if (TotalCoeff>0)
	{
		if (TotalCoeff>10 && TrailingOnes<3)
		{
			suffixLength=1;
		}
		else
		{
			suffixLength=0;
		}

		for (int i=0;i<TotalCoeff;i++)
		{
			if (i<TrailingOnes)
			{
				trailing_ones_sign_flag=getRawBit();
				level[i]=1-2*trailing_ones_sign_flag;
			}
			else
			{
				int level_prefix;
				for(level_prefix=0; getRawBit()==0; ++level_prefix);
				if (level_prefix > 15)
				{
					printf("Non-fatal error: Unexpected level_prefix value (%d)\n", level_prefix);
					printf("Frame #%d, CurrMbAddr = %d\n", frameCount, CurrMbAddr);
					system("pause");
				}

				int levelSuffixSize;

				if (level_prefix==14 && suffixLength==0)
				{
					levelSuffixSize=4;
				}
				else if (level_prefix>=15)
				{
					levelSuffixSize=(level_prefix-3);
				}
				else
				{
					levelSuffixSize=suffixLength;
				}

				//3. Reading level_suffix
				if (levelSuffixSize>0 || level_prefix>=14)
				{
					level_suffix=getRawBits(levelSuffixSize);
				}
				else
				{
					level_suffix=0;
				}

				levelCode=inputstream_to_levelcode[level_prefix][suffixLength][level_suffix];

				if (i==TrailingOnes && TrailingOnes<3)
				{
					levelCode += 2;
				}

				//8. Calculating level[i] from levelCode

				if ((levelCode & 0x1)==0)
				{
					level[i]=(levelCode+2)>>1;
				}
				else
				{
					level[i]=(-levelCode-1)>>1;
				}

				//9. , 10. suffixLength corrections

				if (suffixLength==0)
				{
					suffixLength=1;
				}

				if ((ABS(level[i])>(3<<(suffixLength-1))) && suffixLength<6)
				{
					suffixLength++;
				}
			}
		}

		if(TotalCoeff < (endIdx - startIdx + 1))
		{
			int total_zeros;
			
			if (invoked_for_ChromaDCLevel==0)
			{
				total_zeros=cavlc_table_decode(TotalZerosCodeTable_4x4[TotalCoeff-1]);
			}
			else
			{
				total_zeros=cavlc_table_decode(TotalZerosCodeTable_ChromaDC[TotalCoeff-1]);
			}
			zerosLeft=total_zeros;
		}
		else
		{
			zerosLeft=0;
		}


		for(int j=0; j<TotalCoeff-1; j++)
		{
			if (zerosLeft>0)
			{
				if (zerosLeft>6)
				{
					//Umjesto tablice run_before za zerosLeft>6
					run_before=7-getRawBits(3);
					if (run_before==7)
					{
						while(getRawBit()==0)
						{
							++run_before;
						}
					}
				}
				else
				{
					run_before=cavlc_table_decode(RunBeforeCodeTable[zerosLeft-1]);
				}

				run[j]=run_before;
			}
			else
			{
			  run[j]=0;
			}

			zerosLeft=zerosLeft-run[j];
		}

		run[TotalCoeff-1]=zerosLeft;

		coeffNum=-1;
		for(int i=TotalCoeff-1; i>=0; i--)
		{
			coeffNum+=run[i]+1;
			coeffLevel[startIdx + coeffNum]=level[i];
		}
	}
}