
/*!
 ***********************************************************************
 *  \file
 *     configfile.h
 *  \brief
 *     Prototypes for configfile.c and definitions of used structures.
 ***********************************************************************
 */

#include "fmo.h"

#ifndef _CONFIGFILE_H_
#define _CONFIGFILE_H_

#define DEFAULTCONFIGFILENAME "encoder.cfg"

#define PROFILE_IDC     88
#define LEVEL_IDC       21


//! Maps parameter name to its address, type etc.
typedef struct {
  char *TokenName;    //!< name
  void *Place;        //!< address
  int Type;           //!< type:  0-int, 1-char[], 2-double
  double Default;     //!< default value
  int param_limits;   //!< 0: no limits, 1: both min and max, 2: only min (i.e. no negatives), 3: special case for QPs since min needs bitdepth_qp_scale
  double min_limit;
  double max_limit;
  int    char_size;   //!< Dimension of type char[]
} Mapping;

InputParameters cfgparams;


#ifdef INCLUDED_BY_CONFIGFILE_C
// Mapping_Map Syntax:
// {NAMEinConfigFile,  &cfgparams.VariableName, Type, InitialValue, LimitType, MinLimit, MaxLimit, CharSize}
// Types : {0:int, 1:text, 2: double}
// LimitType: {0:none, 1:both, 2:minimum, 3: QP based}
// We could separate this based on types to make it more flexible and allow also defaults for text types.
Mapping Map[] = {
    {"ProfileIDC",               &cfgparams.ProfileIDC,                   0,   (double) PROFILE_IDC,      0,  0.0,              0.0,                             },
    {"IntraProfile",             &cfgparams.IntraProfile,                 0,   0.0,                       1,  0.0,              1.0,                             }, 
    {"LevelIDC",                 &cfgparams.LevelIDC,                     0,   (double) LEVEL_IDC,        0,  0.0,              0.0,                             },
    {"FrameRate",                &cfgparams.source.frame_rate,            2,   (double) INIT_FRAME_RATE,  1,  0.0,            100.0,                             },
    {"ResendSPS",                &cfgparams.ResendSPS,                    0,   0.0,                       1,  0.0,              3.0,                             },
    {"StartFrame",               &cfgparams.start_frame,                  0,   0.0,                       2,  0.0,              0.0,                             },
    {"IntraPeriod",              &cfgparams.intra_period,                 0,   0.0,                       2,  0.0,              0.0,                             },
    {"IDRPeriod",                &cfgparams.idr_period,                   0,   0.0,                       2,  0.0,              0.0,                             },
    {"IntraDelay",               &cfgparams.intra_delay,                  0,   0.0,                       2,  0.0,              0.0,                             },
    {"AdaptiveIntraPeriod",      &cfgparams.adaptive_intra_period,        0,   0.0,                       1,  0.0,              1.0,                             },
    {"AdaptiveIDRPeriod",        &cfgparams.adaptive_idr_period,          0,   0.0,                       1,  0.0,              2.0,                             },
    {"EnableOpenGOP",            &cfgparams.EnableOpenGOP,                0,   0.0,                       1,  0.0,              1.0,                             },
    {"EnableIDRGOP",             &cfgparams.EnableIDRGOP,                 0,   0.0,                       1,  0.0,              1.0,                             },    
    {"FramesToBeEncoded",        &cfgparams.no_frames,                    0,   1.0,                       2, -1.0,              0.0,                             },
    {"QPISlice",                 &cfgparams.qp[I_SLICE],                  0,   24.0,                      3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"QPPSlice",                 &cfgparams.qp[P_SLICE],                  0,   24.0,                      3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"QPBSlice",                 &cfgparams.qp[B_SLICE],                  0,   24.0,                      3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"QPSPSlice",                &cfgparams.qp[SP_SLICE],                 0,   24.0,                      3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"QPSISlice",                &cfgparams.qp[SI_SLICE],                 0,   24.0,                      3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"ChangeQPFrame",            &cfgparams.qp2frame,                     0,   0.0,                       2,  0.0,              0.0,                             },
    {"ChangeQPI",                &cfgparams.qp2off[I_SLICE],              0,   0.0,                       0,  (double) -MAX_QP,  (double) MAX_QP,                 },
    {"ChangeQPP",                &cfgparams.qp2off[P_SLICE],              0,   0.0,                       0,  (double) -MAX_QP,  (double) MAX_QP,                 },
    {"ChangeQPB",                &cfgparams.qp2off[B_SLICE],              0,   0.0,                       0,  (double) -MAX_QP,  (double) MAX_QP,                 },
    {"ChangeQPSP",               &cfgparams.qp2off[SP_SLICE],             0,   0.0,                       0,  (double) -MAX_QP,  (double) MAX_QP,                 },
    {"ChangeQPSI",               &cfgparams.qp2off[SI_SLICE],             0,   0.0,                       0,  (double) -MAX_QP,  (double) MAX_QP,                 },
    {"QPSP2Slice",               &cfgparams.qpsp,                         0,   0.0,                       3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"FrameSkip",                &cfgparams.frame_skip,                   0,   0.0,                       2,  0.0,              0.0,                             },
    {"DisableSubpelME",          &cfgparams.DisableSubpelME,              0,   0.0,                       1,  0.0,              1.0,                             },
    {"SearchRange",              &cfgparams.search_range,                 0,   16.0,                      2,  0.0,              0.0,                             },
    {"NumberReferenceFrames",    &cfgparams.num_ref_frames,               0,   1.0,                       1,  0.0,             16.0,                             },
    {"PList0References",         &cfgparams.P_List0_refs,                 0,   0.0,                       1,  0.0,             16.0,                             },
    {"BList0References",         &cfgparams.B_List0_refs,                 0,   0.0,                       1,  0.0,             16.0,                             },
    {"BList1References",         &cfgparams.B_List1_refs,                 0,   1.0,                       1,  0.0,             16.0,                             },
    {"Log2MaxFNumMinus4",        &cfgparams.Log2MaxFNumMinus4,            0,   0.0,                       1, -1.0,             12.0,                             },
    {"Log2MaxPOCLsbMinus4",      &cfgparams.Log2MaxPOCLsbMinus4,          0,   2.0,                       1, -1.0,             12.0,                             },
    {"GenerateMultiplePPS",      &cfgparams.GenerateMultiplePPS,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"ResendPPS",                &cfgparams.ResendPPS,                    0,   0.0,                       1,  0.0,              1.0,                             },
    {"SendAUD",                  &cfgparams.SendAUD,                      0,   0.0,                       1,  0.0,              1.0,                             },
    {"SourceWidth",              &cfgparams.source.width,                 0,   176.0,                     2,  0.0,              0.0,                             },
    {"SourceHeight",             &cfgparams.source.height,                0,   144.0,                     2,  0.0,              0.0,                             },
    {"SourceResize",             &cfgparams.src_resize,                   0,   0.0,                       1,  0.0,              1.0,                             },
    {"OutputWidth",              &cfgparams.output.width,                 0,   176.0,                     2, 16.0,              0.0,                             },
    {"OutputHeight",             &cfgparams.output.height,                0,   144.0,                     2, 16.0,              0.0,                             },
    {"Grayscale",                &cfgparams.grayscale,                    0,   0.0,                       0,  0.0,              1.0,                             },
    {"MbLineIntraUpdate",        &cfgparams.intra_upd,                    0,   0.0,                       1,  0.0,              1.0,                             },
    {"SliceMode",                &cfgparams.slice_mode,                   0,   0.0,                       1,  0.0,              3.0,                             },
    {"SliceArgument",            &cfgparams.slice_argument,               0,   1.0,                       2,  1.0,              1.0,                             },
    {"UseConstrainedIntraPred",  &cfgparams.UseConstrainedIntraPred,      0,   0.0,                       1,  0.0,              1.0,                             },
    {"InputFile1",               &cfgparams.input_file1.fname,            1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"InputFile",                &cfgparams.input_file1.fname,            1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"InputHeaderLength",        &cfgparams.infile_header,                0,   0.0,                       2,  0.0,              1.0,                             },
    {"OutputFile",               &cfgparams.outfile,                      1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"ReconFile",                &cfgparams.ReconFile,                    1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"TraceFile",                &cfgparams.TraceFile,                    1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"StatsFile",                &cfgparams.StatsFile,                    1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"DisposableP",              &cfgparams.DisposableP,                  0,   0.0,                       1,  0.0,              1.0,                             },
    {"SetFirstAsLongTerm",       &cfgparams.SetFirstAsLongTerm,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"MultiSourceData",          &cfgparams.MultiSourceData,              0,   0.0,                       0,  0.0,              2.0,                             },
    {"InputFile2",               &cfgparams.input_file2.fname,            1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"InputFile3",               &cfgparams.input_file3.fname,            1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },

    {"ProcessInput",             &cfgparams.ProcessInput,                 0,   0.0,                       1,  0.0,              4.0,                             },    
    {"DispPQPOffset",            &cfgparams.DispPQPOffset,                0,   0.0,                       0,-51.0,             51.0,                             },
    {"NumberBFrames",            &cfgparams.NumberBFrames,                0,   0.0,                       2,  0.0,              0.0,                             },
    {"PReplaceBSlice",           &cfgparams.PReplaceBSlice,               0,   0.0,                       1,  0.0,              1.0,                             },
    {"BRefPicQPOffset",          &cfgparams.qpBRSOffset,                  0,   0.0,                       0,-51.0,             51.0,                             },
    {"DirectModeType",           &cfgparams.direct_spatial_mv_pred_flag,  0,   0.0,                       1,  0.0,              1.0,                             },
    {"DirectInferenceFlag",      &cfgparams.directInferenceFlag,          0,   1.0,                       1,  0.0,              1.0,                             },
    {"SPPicturePeriodicity",     &cfgparams.sp_periodicity,               0,   0.0,                       2,  0.0,              0.0,                             },        
    {"SI_FRAMES",                &cfgparams.si_frame_indicator,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"SP_output",                &cfgparams.sp_output_indicator,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"SP_output_name",           &cfgparams.sp_output_filename,           1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"SPSwitchPeriod",           &cfgparams.sp_switch_period,             0,   0.0,                       2,  0.0,              0.0,                             },
    {"SP2_FRAMES",               &cfgparams.sp2_frame_indicator,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"SP2_input_name1",          &cfgparams.sp2_input_filename1,          1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"SP2_input_name2",          &cfgparams.sp2_input_filename2,          1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"SymbolMode",               &cfgparams.symbol_mode,                  0,   0.0,                       1,  (double) CAVLC,  (double) CABAC,                   },
    {"OutFileMode",              &cfgparams.of_mode,                      0,   0.0,                       1,  0.0,              1.0,                             },
    {"PartitionMode",            &cfgparams.partition_mode,               0,   0.0,                       1,  0.0,              1.0,                             },
    {"PSliceSkip",               &cfgparams.InterSearch[0][0],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"PSliceSearch16x16",        &cfgparams.InterSearch[0][1],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"PSliceSearch16x8",         &cfgparams.InterSearch[0][2],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"PSliceSearch8x16",         &cfgparams.InterSearch[0][3],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"PSliceSearch8x8",          &cfgparams.InterSearch[0][4],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"PSliceSearch8x4",          &cfgparams.InterSearch[0][5],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"PSliceSearch4x8",          &cfgparams.InterSearch[0][6],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"PSliceSearch4x4",          &cfgparams.InterSearch[0][7],            0,   1.0,                       1,  0.0,              1.0,                             },
    
    // B slice partition modes.
    {"BSliceDirect",             &cfgparams.InterSearch[1][0],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"BSliceSearch16x16",        &cfgparams.InterSearch[1][1],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"BSliceSearch16x8",         &cfgparams.InterSearch[1][2],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"BSliceSearch8x16",         &cfgparams.InterSearch[1][3],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"BSliceSearch8x8",          &cfgparams.InterSearch[1][4],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"BSliceSearch8x4",          &cfgparams.InterSearch[1][5],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"BSliceSearch4x8",          &cfgparams.InterSearch[1][6],            0,   1.0,                       1,  0.0,              1.0,                             },
    {"BSliceSearch4x4",          &cfgparams.InterSearch[1][7],            0,   1.0,                       1,  0.0,              1.0,                             },
    //Bipredicting Motion Estimation parameters
    {"BiPredMotionEstimation",   &cfgparams.BiPredMotionEstimation,       0,   0.0,                       1,  0.0,              1.0,                             },
    {"BiPredSearch16x16",        &cfgparams.BiPredSearch[0],              0,   1.0,                       1,  0.0,              1.0,                             },
    {"BiPredSearch16x8",         &cfgparams.BiPredSearch[1],              0,   0.0,                       1,  0.0,              1.0,                             },
    {"BiPredSearch8x16",         &cfgparams.BiPredSearch[2],              0,   0.0,                       1,  0.0,              1.0,                             },
    {"BiPredSearch8x8",          &cfgparams.BiPredSearch[3],              0,   0.0,                       1,  0.0,              1.0,                             },
    
    {"BiPredMERefinements",      &cfgparams.BiPredMERefinements,          0,   0.0,                       1,  0.0,              5.0,                             },
    {"BiPredMESearchRange",      &cfgparams.BiPredMESearchRange,          0,   8.0,                       2,  0.0,              0.0,                             },
    {"BiPredMESubPel",           &cfgparams.BiPredMESubPel,               0,   1.0,                       1,  0.0,              2.0,                             },

    {"DisableIntraInInter",      &cfgparams.DisableIntraInInter,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"IntraDisableInterOnly",    &cfgparams.IntraDisableInterOnly,        0,   0.0,                       1,  0.0,              1.0,                             },
    {"DisableIntra4x4",          &cfgparams.DisableIntra4x4,              0,   0.0,                       1,  0.0,              1.0,                             },       
    {"DisableIntra16x16",        &cfgparams.DisableIntra16x16,            0,   0.0,                       1,  0.0,              1.0,                             },   
    {"Intra4x4ParDisable",       &cfgparams.Intra4x4ParDisable,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"Intra4x4DiagDisable",      &cfgparams.Intra4x4DiagDisable,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"Intra4x4DirDisable",       &cfgparams.Intra4x4DirDisable,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"Intra16x16ParDisable",     &cfgparams.Intra16x16ParDisable,         0,   0.0,                       1,  0.0,              1.0,                             },
    {"Intra16x16PlaneDisable",   &cfgparams.Intra16x16PlaneDisable,       0,   0.0,                       1,  0.0,              1.0,                             },
    {"EnableIPCM",               &cfgparams.EnableIPCM,                   0,   0.0,                       1,  0.0,              2.0,                             },
    {"ChromaIntraDisable",       &cfgparams.ChromaIntraDisable,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDOptimization",           &cfgparams.rdopt,                        0,   0.0,                       1,  0.0,              3.0,                             },
    {"SubMBCodingState",         &cfgparams.subMBCodingState,             0,   2.0,                       1,  0.0,              2.0,                             },
    {"I16RDOpt",                 &cfgparams.I16rdo,                       0,   0.0,                       1,  0.0,              1.0,                             },
    {"DistortionSSIM",           &cfgparams.Distortion[SSIM],             0,   0.0,                       1,  0.0,              1.0,                             },
    {"DistortionMS_SSIM",        &cfgparams.Distortion[MS_SSIM],          0,   0.0,                       1,  0.0,              1.0,                             },
    {"SSIMOverlapSize",          &cfgparams.SSIMOverlapSize,              0,   1.0,                       2,  1.0,              1.0,                             },
    {"DistortionYUVtoRGB",       &cfgparams.DistortionYUVtoRGB,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"CtxAdptLagrangeMult",      &cfgparams.CtxAdptLagrangeMult,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"FastCrIntraDecision",      &cfgparams.FastCrIntraDecision,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"DisableThresholding",      &cfgparams.disthres,                     0,   0.0,                       1,  0.0,              1.0,                             },
    {"DisableBSkipRDO",          &cfgparams.nobskip,                      0,   0.0,                       1,  0.0,              1.0,                             },
    {"ForceTrueRateRDO",         &cfgparams.ForceTrueRateRDO,             0,   0.0,                       1,  0.0,              2.0,                             },    
    {"LossRateA",                &cfgparams.LossRateA,                    0,   0.0,                       2,  0.0,              0.0,                             },
    {"LossRateB",                &cfgparams.LossRateB,                    0,   0.0,                       2,  0.0,              0.0,                             },
    {"LossRateC",                &cfgparams.LossRateC,                    0,   0.0,                       2,  0.0,              0.0,                             },
    {"NumberOfDecoders",         &cfgparams.NoOfDecoders,                 0,   0.0,                       2,  0.0,              0.0,                             },
    {"ErrorConcealment",         &cfgparams.ErrorConcealment,             0,   0.0,                       2,  0.0,              0.0,                             },
    {"RestrictRefFrames",        &cfgparams.RestrictRef ,                 0,   0.0,                       1,  0.0,              1.0,                             },
#ifdef _LEAKYBUCKET_
    {"NumberofLeakyBuckets",     &cfgparams.NumberLeakyBuckets,           0,   2.0,                       1,  2.0,              255.0,                           },
    {"LeakyBucketRateFile",      &cfgparams.LeakyBucketRateFile,          1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"LeakyBucketParamFile",     &cfgparams.LeakyBucketParamFile,         1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
#endif
    {"PicInterlace",             &cfgparams.PicInterlace,                 0,   0.0,                       1,  0.0,              2.0,                             },
    {"MbInterlace",              &cfgparams.MbInterlace,                  0,   0.0,                       1,  0.0,              3.0,                             },

    {"IntraBottom",              &cfgparams.IntraBottom,                  0,   0.0,                       1,  0.0,              1.0,                             },

    {"NumFramesInELayerSubSeq",  &cfgparams.NumFramesInELSubSeq,          0,   0.0,                       2,  0.0,              0.0,                             },
    {"RandomIntraMBRefresh",     &cfgparams.RandomIntraMBRefresh,         0,   0.0,                       2,  0.0,              0.0,                             },
    {"WeightedPrediction",       &cfgparams.WeightedPrediction,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"WeightedBiprediction",     &cfgparams.WeightedBiprediction,         0,   0.0,                       1,  0.0,              2.0,                             },
    {"WPMethod",                 &cfgparams.WPMethod,                     0,   0.0,                       1,  0.0,              1.0,                             }, 
    {"WPIterMC",                 &cfgparams.WPIterMC,                     0,   0.0,                       1,  0.0,              1.0,                             },     
    {"ChromaWeightSupport",      &cfgparams.ChromaWeightSupport,          0,   0.0,                       1,  0.0,              1.0,                             },    
    {"EnhancedBWeightSupport",   &cfgparams.EnhancedBWeightSupport,       0,   0.0,                       1,  0.0,              1.0,                             },    
    {"UseWeightedReferenceME",   &cfgparams.UseWeightedReferenceME,       0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDPictureDecision",        &cfgparams.RDPictureDecision,            0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDPictureIntra",           &cfgparams.RDPictureIntra,               0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDPSliceWeightOnly",       &cfgparams.RDPSliceWeightOnly,           0,   1.0,                       1,  0.0,              2.0,                             },
    {"RDPSliceBTest",            &cfgparams.RDPSliceBTest,                0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDBSliceWeightOnly",       &cfgparams.RDBSliceWeightOnly,           0,   0.0,                       1,  0.0,              2.0,                             },

    {"SkipIntraInInterSlices",   &cfgparams.SkipIntraInInterSlices,       0,   0.0,                       1,  0.0,              1.0,                             },
    {"BReferencePictures",       &cfgparams.BRefPictures,                 0,   0.0,                       1,  0.0,              2.0,                             },
    {"HierarchicalCoding",       &cfgparams.HierarchicalCoding,           0,   0.0,                       1,  0.0,              3.0,                             },
    {"HierarchyLevelQPEnable",   &cfgparams.HierarchyLevelQPEnable,       0,   0.0,                       1,  0.0,              1.0,                             },
    {"ExplicitHierarchyFormat",  &cfgparams.ExplicitHierarchyFormat,      1,   0.0,                       0,  0.0,              0.0,             INPUT_TEXT_SIZE,},
    {"ExplicitSeqCoding",        &cfgparams.ExplicitSeqCoding,            0,   0.0,                       1,  0.0,              3.0,                             },
    {"ExplicitSeqFile",          &cfgparams.ExplicitSeqFile,              1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"LowDelay",                 &cfgparams.LowDelay,                     0,   0.0,                       1,  0.0,              1.0,                             },
    {"ReferenceReorder",         &cfgparams.ReferenceReorder,             0,   0.0,                       1,  0.0,              2.0,                             },
    {"MemoryManagement",         &cfgparams.MemoryManagement,          0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFParametersFlag",         &cfgparams.DFSendParameters,             0,   0.0,                       1,  0.0,              1.0,                             },
    {"DFDisableRefISlice",       &cfgparams.DFDisableIdc[1][I_SLICE],     0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableNRefISlice",      &cfgparams.DFDisableIdc[0][I_SLICE],     0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableRefPSlice",       &cfgparams.DFDisableIdc[1][P_SLICE],     0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableNRefPSlice",      &cfgparams.DFDisableIdc[0][P_SLICE],     0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableRefBSlice",       &cfgparams.DFDisableIdc[1][B_SLICE],     0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableNRefBSlice",      &cfgparams.DFDisableIdc[0][B_SLICE],     0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableRefSPSlice",      &cfgparams.DFDisableIdc[1][SP_SLICE],    0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableNRefSPSlice",     &cfgparams.DFDisableIdc[0][SP_SLICE],    0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableRefSISlice",      &cfgparams.DFDisableIdc[1][SI_SLICE],    0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFDisableNRefSISlice",     &cfgparams.DFDisableIdc[0][SI_SLICE],    0,   0.0,                       1,  0.0,              2.0,                             },
    {"DFAlphaRefISlice",         &cfgparams.DFAlpha[1][I_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaNRefISlice",        &cfgparams.DFAlpha[0][I_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaRefPSlice",         &cfgparams.DFAlpha[1][P_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaNRefPSlice",        &cfgparams.DFAlpha[0][P_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaRefBSlice",         &cfgparams.DFAlpha[1][B_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaNRefBSlice",        &cfgparams.DFAlpha[0][B_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaRefSPSlice",        &cfgparams.DFAlpha[1][SP_SLICE],         0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaNRefSPSlice",       &cfgparams.DFAlpha[0][SP_SLICE],         0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaRefSISlice",        &cfgparams.DFAlpha[1][SI_SLICE],         0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFAlphaNRefSISlice",       &cfgparams.DFAlpha[0][SI_SLICE],         0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaRefISlice",          &cfgparams.DFBeta[1][I_SLICE],           0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaNRefISlice",         &cfgparams.DFBeta[0][I_SLICE],           0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaRefPSlice",          &cfgparams.DFBeta[1][P_SLICE],           0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaNRefPSlice",         &cfgparams.DFBeta[0][P_SLICE],           0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaRefBSlice",          &cfgparams.DFBeta[1][B_SLICE],           0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaNRefBSlice",         &cfgparams.DFBeta[0][B_SLICE],           0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaRefSPSlice",         &cfgparams.DFBeta[1][SP_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaNRefSPSlice",        &cfgparams.DFBeta[0][SP_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaRefSISlice",         &cfgparams.DFBeta[1][SI_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },
    {"DFBetaNRefSISlice",        &cfgparams.DFBeta[0][SI_SLICE],          0,   0.0,                       1, -6.0,              6.0,                             },

    {"SparePictureOption",       &cfgparams.SparePictureOption,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"SparePictureDetectionThr", &cfgparams.SPDetectionThreshold,         0,   0.0,                       2,  0.0,              0.0,                             },
    {"SparePicturePercentageThr",&cfgparams.SPPercentageThreshold,        0,   0.0,                       2,  0.0,            100.0,                             },

    {"num_slice_groups_minus1",  &cfgparams.num_slice_groups_minus1,      0,   0.0,                       1,  0.0,  (double)MAXSLICEGROUPIDS - 1                 },
    {"slice_group_map_type",     &cfgparams.slice_group_map_type,         0,   0.0,                       1,  0.0,              6.0,                             },
    {"slice_group_change_direction_flag", &cfgparams.slice_group_change_direction_flag, 0,   0.0,         1,  0.0,              2.0,                             },
    {"slice_group_change_rate_minus1",    &cfgparams.slice_group_change_rate_minus1,    0,   0.0,         2,  0.0,              1.0,                             },
    {"SliceGroupConfigFileName", &cfgparams.SliceGroupConfigFileName,     1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },

    {"UseRedundantPicture",      &cfgparams.redundant_pic_flag,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"NumRedundantHierarchy",    &cfgparams.NumRedundantHierarchy,        0,   0.0,                       1,  0.0,              4.0,                             },
    {"PrimaryGOPLength",         &cfgparams.PrimaryGOPLength,             0,   1.0,                       1,  1.0,              16.0,                            },
    {"NumRefPrimary",            &cfgparams.NumRefPrimary,                0,   1.0,                       1,  1.0,              16.0,                            },

    {"PicOrderCntType",          &cfgparams.pic_order_cnt_type,           0,   0.0,                       1,  0.0,              2.0,                             },

    {"ContextInitMethod",        &cfgparams.context_init_method,          0,   0.0,                       1,  0.0,              1.0,                             },
    {"FixedModelNumber",         &cfgparams.model_number,                 0,   0.0,                       1,  0.0,              2.0,                             },

    {"ReportFrameStats",         &cfgparams.ReportFrameStats,             0,   0.0,                       1,  0.0,              1.0,                             },
    {"DisplayEncParams",         &cfgparams.DisplayEncParams,             0,   0.0,                       1,  0.0,              1.0,                             },
    {"Verbose",                  &cfgparams.Verbose,                      0,   1.0,                       1,  0.0,              4.0,                             },
    {"SkipGlobalStats",          &cfgparams.skip_gl_stats,                0,   0.0,                       1,  0.0,              1.0,                             },
    {"ChromaMCBuffer",           &cfgparams.ChromaMCBuffer,               0,   0.0,                       1,  0.0,              1.0,                             },
    {"ChromaMEEnable",           &cfgparams.ChromaMEEnable,               0,   0.0,                       1,  0.0,              2.0,                             },
    {"ChromaMEWeight",           &cfgparams.ChromaMEWeight,               0,   1.0,                       2,  1.0,              0.0,                             },
    {"MEDistortionFPel",         &cfgparams.MEErrorMetric[F_PEL],         0,   0.0,                       1,  0.0,              3.0,                             },
    {"MEDistortionHPel",         &cfgparams.MEErrorMetric[H_PEL],         0,   0.0,                       1,  0.0,              3.0,                             },
    {"MEDistortionQPel",         &cfgparams.MEErrorMetric[Q_PEL],         0,   2.0,                       1,  0.0,              3.0,                             },
    {"MDDistortion",             &cfgparams.ModeDecisionMetric,           0,   2.0,                       1,  0.0,              2.0,                             },
    {"SkipDeBlockNonRef",        &cfgparams.SkipDeBlockNonRef,            0,   0.0,                       1,  0.0,              1.0,                             },

    // Rate Control
    {"RateControlEnable",        &cfgparams.RCEnable,                     0,   0.0,                       1,  0.0,              1.0,                             },
    {"Bitrate",                  &cfgparams.bit_rate,                     0,   0.0,                       2,  0.0,              0.0,                             },
    {"InitialQP",                &cfgparams.SeinitialQP,                  0,   0.0,                       3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"BasicUnit",                &cfgparams.basicunit,                    0,   0.0,                       2,  0.0,              0.0,                             },
    {"ChannelType",              &cfgparams.channel_type,                 0,   0.0,                       1,  0.0,              1.0,                             },
    {"RCUpdateMode",             &cfgparams.RCUpdateMode,                 0,   0.0,                       1,  0.0,              4.0,                             },
    {"RCISliceBitRatio",         &cfgparams.RCISliceBitRatio,             2,   1.0,                       1,  0.0,              20.0,                            },
    {"RCBSliceBitRatio0",        &cfgparams.RCBSliceBitRatio[0],          2,   0.5,                       1,  0.0,              20.0,                            },
    {"RCBSliceBitRatio1",        &cfgparams.RCBSliceBitRatio[1],          2,   0.25,                      1,  0.0,              20.0,                            },
    {"RCBSliceBitRatio2",        &cfgparams.RCBSliceBitRatio[2],          2,   0.25,                      1,  0.0,              20.0,                            },
    {"RCBSliceBitRatio3",        &cfgparams.RCBSliceBitRatio[3],          2,   0.25,                      1,  0.0,              20.0,                            },
    {"RCBSliceBitRatio4",        &cfgparams.RCBSliceBitRatio[4],          2,   0.25,                      1,  0.0,              20.0,                            },
    {"RCBoverPRatio",            &cfgparams.RCBoverPRatio,                2,   0.45,                      1,  0.0,              1000.0,                          },
    {"RCIoverPRatio",            &cfgparams.RCIoverPRatio,                2,   3.80,                      1,  0.0,              1000.0,                          },
    {"RCMinQPPSlice",            &cfgparams.RCMinQP[P_SLICE],             0,   (double) MIN_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMaxQPPSlice",            &cfgparams.RCMaxQP[P_SLICE],             0,   (double) MAX_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMinQPBSlice",            &cfgparams.RCMinQP[B_SLICE],             0,   (double) MIN_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMaxQPBSlice",            &cfgparams.RCMaxQP[B_SLICE],             0,   (double) MAX_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMinQPISlice",            &cfgparams.RCMinQP[I_SLICE],             0,   (double) MIN_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMaxQPISlice",            &cfgparams.RCMaxQP[I_SLICE],             0,   (double) MAX_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMinQPSPSlice",           &cfgparams.RCMinQP[SP_SLICE],            0,   (double) MIN_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMaxQPSPSlice",           &cfgparams.RCMaxQP[SP_SLICE],            0,   (double) MAX_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMinQPSISlice",           &cfgparams.RCMinQP[SI_SLICE],            0,   (double) MIN_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMaxQPSISlice",           &cfgparams.RCMaxQP[SI_SLICE],            0,   (double) MAX_QP,           3,  (double) MIN_QP,  (double) MAX_QP,                 },
    {"RCMaxQPChange",            &cfgparams.RCMaxQPChange,                0,   4.0,                       1,  0.0,              10.0,                            },

    // Q_Matrix
    {"QmatrixFile",              &cfgparams.QmatrixFile,                  1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"ScalingMatrixPresentFlag", &cfgparams.ScalingMatrixPresentFlag,     0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag0",  &cfgparams.ScalingListPresentFlag[0],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag1",  &cfgparams.ScalingListPresentFlag[1],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag2",  &cfgparams.ScalingListPresentFlag[2],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag3",  &cfgparams.ScalingListPresentFlag[3],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag4",  &cfgparams.ScalingListPresentFlag[4],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag5",  &cfgparams.ScalingListPresentFlag[5],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag6",  &cfgparams.ScalingListPresentFlag[6],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag7",  &cfgparams.ScalingListPresentFlag[7],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag8",  &cfgparams.ScalingListPresentFlag[8],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag9",  &cfgparams.ScalingListPresentFlag[9],    0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag10", &cfgparams.ScalingListPresentFlag[10],   0,   0.0,                       1,  0.0,              3.0,                             },
    {"ScalingListPresentFlag11", &cfgparams.ScalingListPresentFlag[11],   0,   0.0,                       1,  0.0,              3.0,                             },

    // Chroma QP Offset
    {"ChromaQPOffset",           &cfgparams.chroma_qp_index_offset,       0,   0.0,                       1,-51.0,             51.0,                             },

    // Fidelity Range Extensions
// New Input bit-depth entries
    {"SourceBitDepthLuma",       &cfgparams.source.bit_depth[0],          0,   8.0,                       1,  8.0,             14.0,                             },
    {"SourceBitDepthChroma",     &cfgparams.source.bit_depth[1],          0,   8.0,                       1,  8.0,             14.0,                             },
    {"SourceBitDepthRescale",    &cfgparams.src_BitDepthRescale,          0,   0.0,                       1,  0.0,              1.0,                             },
// Old variables replaced from BitDepthLuma and BitDepthChroma. Considered only if SourceBitDepthRescale is 1.
    {"OutputBitDepthLuma",       &cfgparams.output.bit_depth[0],          0,   8.0,                       1,  8.0,             14.0,                             },
    {"OutputBitDepthChroma",     &cfgparams.output.bit_depth[1],          0,   8.0,                       1,  8.0,             14.0,                             },

    {"YUVFormat",                &cfgparams.yuv_format,                   0,   1.0,                       1,  0.0,              3.0,                             },
    {"RGBInput",                 &cfgparams.source.color_model,               0,   0.0,                       1,  0.0,              1.0,                             },
    {"Interleaved",              &cfgparams.input_file1.is_interleaved ,  0,   0.0,                       1,  0.0,              1.0,                             },    
    {"CbQPOffset",               &cfgparams.cb_qp_index_offset,           0,   0.0,                       1,-51.0,             51.0,                             },
    {"CrQPOffset",               &cfgparams.cr_qp_index_offset,           0,   0.0,                       1,-51.0,             51.0,                             },
    {"Transform8x8Mode",         &cfgparams.Transform8x8Mode,             0,   0.0,                       1,  0.0,              2.0,                             },
    // Lossless Coding
    {"LosslessCoding",           &cfgparams.LosslessCoding,               0,   0.0,                       1,  0.0,              1.0,                             },

    // Explicit Lambda Parameters for RDO
    {"UseExplicitLambdaParams",  &cfgparams.UseExplicitLambdaParams,      0,   0.0,                       1,  0.0,              3.0,                             },
    {"UpdateLambdaChromaME",     &cfgparams.UpdateLambdaChromaME,         0,   0.0,                       1,  0.0,              3.0,                             },    
    {"FixedLambdaPSlice",        &cfgparams.FixedLambda[P_SLICE],         2,   0.1,                       2,  0.0,              0.0,                             },
    {"FixedLambdaBSlice",        &cfgparams.FixedLambda[B_SLICE],         2,   0.1,                       2,  0.0,              0.0,                             },
    {"FixedLambdaISlice",        &cfgparams.FixedLambda[I_SLICE],         2,   0.1,                       2,  0.0,              0.0,                             },
    {"FixedLambdaSPSlice",       &cfgparams.FixedLambda[SP_SLICE],        2,   0.1,                       2,  0.0,              0.0,                             },
    {"FixedLambdaSISlice",       &cfgparams.FixedLambda[SI_SLICE],        2,   0.1,                       2,  0.0,              0.0,                             },
    {"FixedLambdaRefBSlice",     &cfgparams.FixedLambda[5],               2,   0.1,                       2,  0.0,              0.0,                             },
    {"LambdaWeightPSlice",       &cfgparams.LambdaWeight[P_SLICE],        2,   0.68,                      2,  0.0,              0.0,                             },
    {"LambdaWeightBSlice",       &cfgparams.LambdaWeight[B_SLICE],        2,   2.00,                      2,  0.0,              0.0,                             },
    {"LambdaWeightISlice",       &cfgparams.LambdaWeight[I_SLICE],        2,   0.65,                      2,  0.0,              0.0,                             },
    {"LambdaWeightSPSlice",      &cfgparams.LambdaWeight[SP_SLICE],       2,   1.50,                      2,  0.0,              0.0,                             },
    {"LambdaWeightSISlice",      &cfgparams.LambdaWeight[SI_SLICE],       2,   0.65,                      2,  0.0,              0.0,                             },
    {"LambdaWeightRefBSlice",    &cfgparams.LambdaWeight[5],              2,   1.50,                      2,  0.0,              0.0,                             },
    // Usage of explicit "initial" offsets for quantization
    {"QOffsetMatrixFile",        &cfgparams.QOffsetMatrixFile,            1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },
    {"OffsetMatrixPresentFlag",  &cfgparams.OffsetMatrixPresentFlag,      0,   0.0,                       1,  0.0,              1.0,                             },

    // Adaptive rounding technique based on JVT-N011
    {"AdaptiveRounding",         &cfgparams.AdaptiveRounding,             0,   0.0,                       1,  0.0,              1.0,                             },
    {"AdaptRoundingFixed",       &cfgparams.AdaptRoundingFixed,           0,   1.0,                       1,  0.0,              1.0,                             },    
    {"AdaptRndPeriod",           &cfgparams.AdaptRndPeriod,               0,  16.0,                       2,  1.0,              0.0,                             },
    {"AdaptRndChroma",           &cfgparams.AdaptRndChroma,               0,   0.0,                       1,  0.0,              1.0,                             },
    {"AdaptRndWFactorIRef",      &cfgparams.AdaptRndWFactor[1][I_SLICE],  0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndWFactorPRef",      &cfgparams.AdaptRndWFactor[1][P_SLICE],  0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndWFactorBRef",      &cfgparams.AdaptRndWFactor[1][B_SLICE],  0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndWFactorINRef",     &cfgparams.AdaptRndWFactor[0][I_SLICE],  0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndWFactorPNRef",     &cfgparams.AdaptRndWFactor[0][P_SLICE],  0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndWFactorBNRef",     &cfgparams.AdaptRndWFactor[0][B_SLICE],  0,   4.0,                       1,  0.0,           4096.0,                             },

    {"AdaptRndCrWFactorIRef",    &cfgparams.AdaptRndCrWFactor[1][I_SLICE],0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndCrWFactorPRef",    &cfgparams.AdaptRndCrWFactor[1][P_SLICE],0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndCrWFactorBRef",    &cfgparams.AdaptRndCrWFactor[1][B_SLICE],0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndCrWFactorINRef",   &cfgparams.AdaptRndCrWFactor[0][I_SLICE],0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndCrWFactorPNRef",   &cfgparams.AdaptRndCrWFactor[0][P_SLICE],0,   4.0,                       1,  0.0,           4096.0,                             },
    {"AdaptRndCrWFactorBNRef",   &cfgparams.AdaptRndCrWFactor[0][B_SLICE],0,   4.0,                       1,  0.0,           4096.0,                             },

    // Prediction Structure
    {"PreferDispOrder",          &cfgparams.PreferDispOrder,              0,   1.0,                       1,  0.0,              1.0,                             },
    {"PreferPowerOfTwo",         &cfgparams.PreferPowerOfTwo,             0,   0.0,                       1,  0.0,              1.0,                             },
    {"FrmStructBufferLength",    &cfgparams.FrmStructBufferLength,        0,  16.0,                       1,  1.0,            128.0,                             },

    // Fast Mode Decision
    {"EarlySkipEnable",          &cfgparams.EarlySkipEnable,              0,   0.0,                       1,  0.0,              1.0,                             },
    {"SelectiveIntraEnable",     &cfgparams.SelectiveIntraEnable,         0,   0.0,                       1,  0.0,              1.0,                             },

    //================================
    // Motion Estimation (ME) Parameters
    //================================
    {"RestrictSearchRange",      &cfgparams.full_search,                  0,   2.0,                       1,  0.0,              2.0,                             },
    // ME Limits
    {"UseMVLimits",              &cfgparams.UseMVLimits,                  0,   0.0,                       1,  0.0,              1.0,                             },
    {"SetMVXLimit",              &cfgparams.SetMVXLimit,                  0,   0.0,                       1,  0.0,           2048.0,                             },
    {"SetMVYLimit",              &cfgparams.SetMVYLimit,                  0,   0.0,                       1,  0.0,            512.0,                             },
    // Fast ME enable
    {"SearchMode",               &cfgparams.SearchMode,                   0,   0.0,                       1, -1.0,              3.0,                             },
    // Parameters for UMHEX control
    {"UMHexDSR",                 &cfgparams.UMHexDSR,                     0,   1.0,                       1,  0.0,              1.0,                             },
    {"UMHexScale",               &cfgparams.UMHexScale,                   0,   1.0,                       0,  0.0,              0.0,                             },
    // Parameters for EPZS control
    {"EPZSPattern",              &cfgparams.EPZSPattern,                  0,   2.0,                       1,  0.0,              5.0,                             },
    {"EPZSDualRefinement",       &cfgparams.EPZSDual,                     0,   3.0,                       1,  0.0,              6.0,                             },
    {"EPZSFixedPredictors",      &cfgparams.EPZSFixed,                    0,   2.0,                       1,  0.0,              2.0,                             },
    {"EPZSTemporal",             &cfgparams.EPZSTemporal,                 0,   1.0,                       1,  0.0,              1.0,                             },
    {"EPZSSpatialMem",           &cfgparams.EPZSSpatialMem,               0,   1.0,                       1,  0.0,              1.0,                             },
    {"EPZSBlockType",            &cfgparams.EPZSBlockType,                0,   1.0,                       1,  0.0,              1.0,                             },
    {"EPZSMinThresScale",        &cfgparams.EPZSMinThresScale,            0,   0.0,                       0,  0.0,              0.0,                             },
    {"EPZSMaxThresScale",        &cfgparams.EPZSMaxThresScale,            0,   2.0,                       0,  0.0,              0.0,                             },
    {"EPZSMedThresScale",        &cfgparams.EPZSMedThresScale,            0,   1.0,                       0,  0.0,              0.0,                             },
    {"EPZSSubPelME",             &cfgparams.EPZSSubPelME,                 0,   1.0,                       1,  0.0,              1.0,                             },
    {"EPZSSubPelMEBiPred",       &cfgparams.EPZSSubPelMEBiPred,           0,   1.0,                       1,  0.0,              1.0,                             },
    {"EPZSSubPelGrid",           &cfgparams.EPZSSubPelGrid,               0,   0.0,                       1,  0.0,              1.0,                             },
    {"EPZSSubPelThresScale",     &cfgparams.EPZSSubPelThresScale,         0,   2.0,                       0,  0.0,              0.0,                             },

    // Tone mapping SEI cfg file
    {"ToneMappingSEIPresentFlag",&cfgparams.ToneMappingSEIPresentFlag,    0,   0.0,                       1,  0.0,              1.0,                             },
    {"ToneMappingFile",          &cfgparams.ToneMappingFile,              1,   0.0,                       0,  0.0,              0.0,             FILE_NAME_SIZE, },

    {"SeparateColourPlane",      &cfgparams.separate_colour_plane_flag,   0,   0.0,                       1,  0.0,              1.0,                             },
    {"WeightY",                  &cfgparams.WeightY,                      2,   1.00,                      1,  0.0,              4.0,                             },
    {"WeightCb",                 &cfgparams.WeightCb,                     2,   1.00,                      1,  0.0,              4.0,                             },
    {"WeightCr",                 &cfgparams.WeightCr,                     2,   1.00,                      1,  0.0,              4.0,                             },
    {"WPMCPrecision",            &cfgparams.WPMCPrecision,                0,   0.0,                       1,  0.0,              2.0,                             },
    {"WPMCPrecFullRef",          &cfgparams.WPMCPrecFullRef,              0,   0.0,                       1,  0.0,              1.0,                             },
    {"WPMCPrecBSlice",           &cfgparams.WPMCPrecBSlice,               0,   1.0,                       1,  0.0,              2.0,                             },
    // Trellis based quantization
    {"UseRDOQuant",              &cfgparams.UseRDOQuant,                  0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDOQ_DC",                  &cfgparams.RDOQ_DC,                      0,   1.0,                       1,  0.0,              1.0,                             },
    {"RDOQ_CR",                  &cfgparams.RDOQ_CR,                      0,   1.0,                       1,  0.0,              1.0,                             },
    {"RDOQ_DC_CR",               &cfgparams.RDOQ_DC_CR,                   0,   1.0,                       1,  0.0,              1.0,                             },
    {"RDOQ_QP_Num",              &cfgparams.RDOQ_QP_Num,                  0,   1.0,                       1,  1.0,              9.0,                             },
    {"RDOQ_CP_Mode",             &cfgparams.RDOQ_CP_Mode,                 0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDOQ_CP_MV",               &cfgparams.RDOQ_CP_MV,                   0,   0.0,                       1,  0.0,              1.0,                             },
    {"RDOQ_Fast",                &cfgparams.RDOQ_Fast,                    0,   0.0,                       1,  0.0,              1.0,                             },
    // VUI parameters
    {"GenerateSEIMessage",       &cfgparams.GenerateSEIMessage,           0,   0.0,                       1,  0.0,              1.0,                             },
    {"EnableVUISupport",         &cfgparams.EnableVUISupport,             0,   0.0,                       1,  0.0,              1.0,                             },
    {"VUI_aspect_ratio_info_present_flag",     &cfgparams.VUI.aspect_ratio_info_present_flag,     0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_aspect_ratio_idc",                   &cfgparams.VUI.aspect_ratio_idc,                   0,   0.0,                       1,  0.0,            255.0,     },
    {"VUI_sar_width",                          &cfgparams.VUI.sar_width,                          0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_sar_height",                         &cfgparams.VUI.sar_height,                         0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_overscan_info_present_flag",         &cfgparams.VUI.overscan_info_present_flag,         0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_overscan_appropriate_flag",          &cfgparams.VUI.overscan_appropriate_flag,          0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_video_signal_type_present_flag",     &cfgparams.VUI.video_signal_type_present_flag,     0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_video_format",                       &cfgparams.VUI.video_format,                       0,   5.0,                       1,  0.0,              7.0,     },
    {"VUI_video_full_range_flag",              &cfgparams.VUI.video_full_range_flag,              0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_colour_description_present_flag",    &cfgparams.VUI.colour_description_present_flag,    0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_colour_primaries",                   &cfgparams.VUI.colour_primaries,                   0,   2.0,                       1,  0.0,            255.0,     },
    {"VUI_transfer_characteristics",           &cfgparams.VUI.transfer_characteristics,           0,   2.0,                       1,  0.0,            255.0,     },
    {"VUI_matrix_coefficients",                &cfgparams.VUI.matrix_coefficients,                0,   2.0,                       1,  0.0,            255.0,     },
    {"VUI_chroma_location_info_present_flag",  &cfgparams.VUI.chroma_location_info_present_flag,  0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_chroma_sample_loc_type_top_field",   &cfgparams.VUI.chroma_sample_loc_type_top_field,   0,   0.0,                       1,  0.0,              5.0,     },
    {"VUI_chroma_sample_loc_type_bottom_field",&cfgparams.VUI.chroma_sample_loc_type_bottom_field,0,   0.0,                       1,  0.0,              5.0,     },
    {"VUI_timing_info_present_flag",           &cfgparams.VUI.timing_info_present_flag,           0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_num_units_in_tick",                  &cfgparams.VUI.num_units_in_tick,                  0,1000.0,                       2,  0.0,              0.0,     },
    {"VUI_time_scale",                         &cfgparams.VUI.time_scale,                         0,60000.0,                      2,  0.0,              0.0,     },
    {"VUI_fixed_frame_rate_flag",              &cfgparams.VUI.fixed_frame_rate_flag,              0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_nal_hrd_parameters_present_flag",    &cfgparams.VUI.nal_hrd_parameters_present_flag,    0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_nal_cpb_cnt_minus1",                 &cfgparams.VUI.nal_cpb_cnt_minus1,                 0,   0.0,                       1,  0.0,             31.0,     },
    {"VUI_nal_bit_rate_scale",                 &cfgparams.VUI.nal_bit_rate_scale,                 0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_nal_cpb_size_scale",                 &cfgparams.VUI.nal_cpb_size_scale,                 0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_nal_bit_rate_value_minus1",          &cfgparams.VUI.nal_bit_rate_value_minus1,          0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_nal_cpb_size_value_minus1",          &cfgparams.VUI.nal_cpb_size_value_minus1,          0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_nal_vbr_cbr_flag",                   &cfgparams.VUI.nal_vbr_cbr_flag,                   0,   1.0,                       1,  0.0,              1.0,     },
    {"VUI_nal_initial_cpb_removal_delay_length_minus1", &cfgparams.VUI.nal_initial_cpb_removal_delay_length_minus1,  0, 23.0,     2,  0.0,              0.0,     },
    {"VUI_nal_cpb_removal_delay_length_minus1",&cfgparams.VUI.nal_cpb_removal_delay_length_minus1,0,  23.0,                       2,  0.0,              0.0,     },
    {"VUI_nal_dpb_output_delay_length_minus1", &cfgparams.VUI.nal_dpb_output_delay_length_minus1, 0,  23.0,                       2,  0.0,              0.0,     },
    {"VUI_nal_time_offset_length",             &cfgparams.VUI.nal_time_offset_length,             0,  24.0,                       2,  0.0,              0.0,     },
    {"VUI_vcl_hrd_parameters_present_flag",    &cfgparams.VUI.vcl_hrd_parameters_present_flag,    0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_vcl_cpb_cnt_minus1",                 &cfgparams.VUI.vcl_cpb_cnt_minus1,                 0,   0.0,                       1,  0.0,             31.0,     },
    {"VUI_vcl_bit_rate_scale",                 &cfgparams.VUI.vcl_bit_rate_scale,                 0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_vcl_cpb_size_scale",                 &cfgparams.VUI.vcl_cpb_size_scale,                 0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_vcl_bit_rate_value_minus1",          &cfgparams.VUI.vcl_bit_rate_value_minus1,          0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_vcl_cpb_size_value_minus1",          &cfgparams.VUI.vcl_cpb_size_value_minus1,          0,   0.0,                       2,  0.0,              0.0,     },
    {"VUI_vcl_vbr_cbr_flag",                   &cfgparams.VUI.vcl_vbr_cbr_flag,                   0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_vcl_initial_cpb_removal_delay_length_minus1", &cfgparams.VUI.vcl_initial_cpb_removal_delay_length_minus1,  0,  23.0,    2,  0.0,              0.0,     },
    {"VUI_vcl_cpb_removal_delay_length_minus1",&cfgparams.VUI.vcl_cpb_removal_delay_length_minus1,0,  23.0,                       2,  0.0,              0.0,     },
    {"VUI_vcl_dpb_output_delay_length_minus1", &cfgparams.VUI.vcl_dpb_output_delay_length_minus1, 0,  23.0,                       2,  0.0,              0.0,     },
    {"VUI_vcl_time_offset_length",             &cfgparams.VUI.vcl_time_offset_length,             0,  24.0,                       2,  0.0,              0.0,     },
    {"VUI_low_delay_hrd_flag",                 &cfgparams.VUI.low_delay_hrd_flag,                 0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_pic_struct_present_flag",            &cfgparams.VUI.pic_struct_present_flag,            0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_bitstream_restriction_flag",         &cfgparams.VUI.bitstream_restriction_flag,         0,   0.0,                       1,  0.0,              1.0,     },
    {"VUI_motion_vectors_over_pic_boundaries_flag", &cfgparams.VUI.motion_vectors_over_pic_boundaries_flag,      0,   1.0,        1,  0.0,              1.0,     },
    {"VUI_max_bytes_per_pic_denom",            &cfgparams.VUI.max_bytes_per_pic_denom,            0,   2.0,                       1,  0.0,             16.0,     },
    {"VUI_max_bits_per_mb_denom",              &cfgparams.VUI.max_bits_per_mb_denom,              0,   1.0,                       1,  0.0,             16.0,     },
    {"VUI_log2_max_mv_length_vertical",        &cfgparams.VUI.log2_max_mv_length_vertical,        0,  16.0,                       1,  0.0,             16.0,     },
    {"VUI_log2_max_mv_length_horizontal",      &cfgparams.VUI.log2_max_mv_length_horizontal,      0,  16.0,                       1,  0.0,             16.0,     },
    {"VUI_num_reorder_frames",                 &cfgparams.VUI.num_reorder_frames,                 0,  16.0,                       1,  0.0,             16.0,     },
    {"VUI_max_dec_frame_buffering",            &cfgparams.VUI.max_dec_frame_buffering,            0,  16.0,                       1,  0.0,             16.0,     },
    {"SEIMessageText",           &cfgparams.SEIMessageText,               1,   0.0,                       0,  0.0,              0.0,             INPUT_TEXT_SIZE,},
    {NULL,                       NULL,                                   -1,   0.0,                       0,  0.0,              0.0,                             },
};


#endif

#ifndef INCLUDED_BY_CONFIGFILE_C
extern Mapping Map[];
#endif

extern void Configure            (VideoParameters *p_Vid, InputParameters *p_Inp, int ac, char *av[]);
extern void getNumberOfFrames    (InputParameters *p_Inp, VideoDataFile *input_file);
extern void read_slice_group_info(VideoParameters *p_Vid, InputParameters *p_Inp);

#endif

