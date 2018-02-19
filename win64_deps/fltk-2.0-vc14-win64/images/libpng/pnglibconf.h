
/* libpng STANDARD API DEFINITION */

/* pnglibconf.h - library build configuration */

/* libpng version 1.5.0 - last changed on January 6, 2011 */

/* Copyright (c) 1998-2011 Glenn Randers-Pehrson */

/* This code is released under the libpng license. */
/* For conditions of distribution and use, see the disclaimer */
/* and license in png.h */

/* pnglibconf.h */
/* Machine generated file: DO NOT EDIT */
/* Derived from: scripts/pnglibconf.dfa */
#ifndef PNGLCONF_H
#define PNGLCONF_H
/* settings */
#define PNG_MAX_GAMMA_8 11
#define PNG_CALLOC_SUPPORTED
#define PNG_QUANTIZE_RED_BITS 5
#define PNG_USER_WIDTH_MAX 1000000L
#define PNG_QUANTIZE_GREEN_BITS 5
#define PNG_API_RULE 0
#define PNG_QUANTIZE_BLUE_BITS 5
#define PNG_USER_CHUNK_CACHE_MAX 0
#define PNG_USER_HEIGHT_MAX 1000000L
#define PNG_sCAL_PRECISION 5
#define PNG_COST_SHIFT 3
#define PNG_WEIGHT_SHIFT 8
#define PNG_USER_CHUNK_MALLOC_MAX 0
#define PNG_DEFAULT_READ_MACROS 1
#define PNG_ZBUF_SIZE 8192
#define PNG_GAMMA_THRESHOLD_FIXED 5000
/* end of settings */
/* options */
#define PNG_INFO_IMAGE_SUPPORTED
#define PNG_HANDLE_AS_UNKNOWN_SUPPORTED
#define PNG_POINTER_INDEXING_SUPPORTED
#define PNG_WARNINGS_SUPPORTED
#define PNG_FLOATING_ARITHMETIC_SUPPORTED
#define PNG_WRITE_SUPPORTED
#define PNG_WRITE_INTERLACING_SUPPORTED
#define PNG_WRITE_16BIT_SUPPORTED
#define PNG_EASY_ACCESS_SUPPORTED
#define PNG_ALIGN_MEMORY_SUPPORTED
#define PNG_WRITE_WEIGHTED_FILTER_SUPPORTED
#define PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_USER_LIMITS_SUPPORTED
#define PNG_FIXED_POINT_SUPPORTED
/*#undef PNG_ERROR_NUMBERS_SUPPORTED*/
#define PNG_ERROR_TEXT_SUPPORTED
#define PNG_READ_SUPPORTED
/*#undef PNG_READ_16_TO_8_ACCURATE_SCALE_SUPPORTED*/
#define PNG_BENIGN_ERRORS_SUPPORTED
#define PNG_SETJMP_SUPPORTED
#define PNG_WRITE_FLUSH_SUPPORTED
#define PNG_MNG_FEATURES_SUPPORTED
#define PNG_FLOATING_POINT_SUPPORTED
#define PNG_INCH_CONVERSIONS_SUPPORTED
#define PNG_STDIO_SUPPORTED
#define PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_USER_MEM_SUPPORTED
#define PNG_IO_STATE_SUPPORTED
#define PNG_SET_USER_LIMITS_SUPPORTED
#define PNG_READ_ANCILLARY_CHUNKS_SUPPORTED
#define PNG_WRITE_INT_FUNCTIONS_SUPPORTED
#define PNG_WRITE_ANCILLARY_CHUNKS_SUPPORTED
#define PNG_WRITE_FILTER_SUPPORTED
#define PNG_SET_CHUNK_CACHE_LIMIT_SUPPORTED
#define PNG_WRITE_iCCP_SUPPORTED
#define PNG_READ_TRANSFORMS_SUPPORTED
#define PNG_READ_GAMMA_SUPPORTED
#define PNG_READ_bKGD_SUPPORTED
#define PNG_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_READ_sCAL_SUPPORTED
#define PNG_WRITE_hIST_SUPPORTED
#define PNG_READ_OPT_PLTE_SUPPORTED
#define PNG_SET_CHUNK_MALLOC_LIMIT_SUPPORTED
#define PNG_WRITE_gAMA_SUPPORTED
#define PNG_READ_GRAY_TO_RGB_SUPPORTED
#define PNG_WRITE_pCAL_SUPPORTED
#define PNG_READ_INVERT_ALPHA_SUPPORTED
#define PNG_WRITE_TRANSFORMS_SUPPORTED
#define PNG_READ_sBIT_SUPPORTED
#define PNG_READ_PACK_SUPPORTED
#define PNG_WRITE_SWAP_SUPPORTED
#define PNG_READ_cHRM_SUPPORTED
#define PNG_WRITE_tIME_SUPPORTED
#define PNG_READ_INTERLACING_SUPPORTED
#define PNG_READ_tRNS_SUPPORTED
#define PNG_WRITE_pHYs_SUPPORTED
#define PNG_WRITE_INVERT_SUPPORTED
#define PNG_READ_RGB_TO_GRAY_SUPPORTED
#define PNG_WRITE_sRGB_SUPPORTED
#define PNG_READ_oFFs_SUPPORTED
#define PNG_WRITE_FILLER_SUPPORTED
#define PNG_WRITE_TEXT_SUPPORTED
#define PNG_WRITE_SHIFT_SUPPORTED
#define PNG_PROGRESSIVE_READ_SUPPORTED
#define PNG_READ_SHIFT_SUPPORTED
#define PNG_CONVERT_tIME_SUPPORTED
#define PNG_READ_USER_TRANSFORM_SUPPORTED
#define PNG_READ_INT_FUNCTIONS_SUPPORTED
#define PNG_READ_USER_CHUNKS_SUPPORTED
#define PNG_READ_hIST_SUPPORTED
#define PNG_READ_16BIT_SUPPORTED
#define PNG_READ_SWAP_ALPHA_SUPPORTED
#define PNG_READ_COMPOSITE_NODIV_SUPPORTED
#define PNG_SEQUENTIAL_READ_SUPPORTED
#define PNG_READ_BACKGROUND_SUPPORTED
#define PNG_READ_QUANTIZE_SUPPORTED
#define PNG_READ_iCCP_SUPPORTED
#define PNG_READ_STRIP_ALPHA_SUPPORTED
#define PNG_READ_PACKSWAP_SUPPORTED
#define PNG_READ_sRGB_SUPPORTED
#define PNG_WRITE_tEXt_SUPPORTED
#define PNG_READ_gAMA_SUPPORTED
#define PNG_READ_pCAL_SUPPORTED
#define PNG_READ_EXPAND_SUPPORTED
#define PNG_WRITE_sPLT_SUPPORTED
#define PNG_READ_SWAP_SUPPORTED
#define PNG_READ_tIME_SUPPORTED
#define PNG_READ_pHYs_SUPPORTED
#define PNG_WRITE_SWAP_ALPHA_SUPPORTED
#define PNG_TIME_RFC1123_SUPPORTED
#define PNG_READ_TEXT_SUPPORTED
#define PNG_WRITE_BGR_SUPPORTED
#define PNG_USER_CHUNKS_SUPPORTED
#define PNG_CONSOLE_IO_SUPPORTED
#define PNG_WRITE_PACK_SUPPORTED
#define PNG_READ_FILLER_SUPPORTED
#define PNG_WRITE_bKGD_SUPPORTED
#define PNG_WRITE_tRNS_SUPPORTED
#define PNG_READ_sPLT_SUPPORTED
#define PNG_WRITE_sCAL_SUPPORTED
#define PNG_WRITE_oFFs_SUPPORTED
#define PNG_READ_tEXt_SUPPORTED
#define PNG_WRITE_sBIT_SUPPORTED
#define PNG_READ_INVERT_SUPPORTED
#define PNG_READ_16_TO_8_SUPPORTED
#define PNG_WRITE_cHRM_SUPPORTED
#define PNG_16BIT_SUPPORTED
#define PNG_WRITE_USER_TRANSFORM_SUPPORTED
#define PNG_READ_BGR_SUPPORTED
#define PNG_WRITE_PACKSWAP_SUPPORTED
#define PNG_WRITE_INVERT_ALPHA_SUPPORTED
#define PNG_sCAL_SUPPORTED
#define PNG_WRITE_zTXt_SUPPORTED
#define PNG_sBIT_SUPPORTED
#define PNG_cHRM_SUPPORTED
#define PNG_bKGD_SUPPORTED
#define PNG_tRNS_SUPPORTED
#define PNG_WRITE_iTXt_SUPPORTED
#define PNG_oFFs_SUPPORTED
#define PNG_USER_TRANSFORM_PTR_SUPPORTED
#define PNG_USER_TRANSFORM_INFO_SUPPORTED
#define PNG_hIST_SUPPORTED
#define PNG_iCCP_SUPPORTED
#define PNG_sRGB_SUPPORTED
#define PNG_READ_zTXt_SUPPORTED
#define PNG_gAMA_SUPPORTED
#define PNG_pCAL_SUPPORTED
#define PNG_CHECK_cHRM_SUPPORTED
#define PNG_tIME_SUPPORTED
#define PNG_pHYs_SUPPORTED
#define PNG_READ_iTXt_SUPPORTED
#define PNG_TEXT_SUPPORTED
#define PNG_SAVE_INT_32_SUPPORTED
#define PNG_sPLT_SUPPORTED
#define PNG_tEXt_SUPPORTED
#define PNG_zTXt_SUPPORTED
#define PNG_iTXt_SUPPORTED
/* end of options */
#endif /* PNGLCONF_H */
