/** @file
Header file for PcdValue structure definition.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCD_VALUE_COMMON_H
#define _PCD_VALUE_COMMON_H

#include <Common/UefiBaseTypes.h>
#include <Common/UefiInternalFormRepresentation.h>

#define __FIELD_SIZE(TYPE, Field) (sizeof((TYPE *)0)->Field)
#define __ARRAY_ELEMENT_SIZE(TYPE, Field) (sizeof((TYPE *)0)->Field[0])
#define __OFFSET_OF(TYPE, Field) ((UINT32)(size_t) &(((TYPE *)0)->Field))
#define __FLEXIBLE_SIZE(Size, TYPE, Field, MaxIndex)   if (__FIELD_SIZE(TYPE, Field) == 0) Size = MAX((__OFFSET_OF(TYPE, Field) + __ARRAY_ELEMENT_SIZE(TYPE, Field) * (MaxIndex)), Size)
#define __ARRAY_SIZE(Array) (sizeof(Array)/sizeof(Array[0]))

#if defined(_MSC_EXTENSIONS)
#define __STATIC_ASSERT static_assert
#else
#define __STATIC_ASSERT _Static_assert
#endif

/**
  Main function updates PCD values. It is auto generated by Build
**/
VOID
PcdEntryPoint (
  VOID
  )

;

/**
  Main function updates PCD values.

  @param argc            Number of command line parameters.
  @param argv            Array of pointers to parameter strings.

  @retval EXIT_SUCCESS
**/
int
PcdValueMain (
  int   argc,
  char  *argv[]
  )
;

/**
  Set PCD value

  @param SkuName               SkuName String
  @param DefaultValueName      DefaultValueName String
  @param TokenSpaceGuidName    TokenSpaceGuidName String
  @param TokenName             TokenName String
  @param Value                 PCD value to be set
**/
VOID
__PcdSet (
  CHAR8   *SkuName             OPTIONAL,
  CHAR8   *DefaultValueName    OPTIONAL,
  CHAR8   *TokenSpaceGuidName,
  CHAR8   *TokenName,
  UINT64  Value
  )
;

/**
  Get PCD value

  @param SkuName               SkuName String
  @param DefaultValueName      DefaultValueName String
  @param TokenSpaceGuidName    TokenSpaceGuidName String
  @param TokenName             TokenName String

  @return PCD value
**/
UINT64
__PcdGet (
  CHAR8  *SkuName             OPTIONAL,
  CHAR8  *DefaultValueName    OPTIONAL,
  CHAR8  *TokenSpaceGuidName,
  CHAR8  *TokenName
  )
;

/**
  Get PCD value buffer

  @param SkuName               SkuName String
  @param DefaultValueName      DefaultValueName String
  @param TokenSpaceGuidName    TokenSpaceGuidName String
  @param TokenName             TokenName String
  @param Size                  Size of PCD value buffer

  @return PCD value buffer
**/
VOID *
__PcdGetPtr (
  CHAR8   *SkuName             OPTIONAL,
  CHAR8   *DefaultValueName    OPTIONAL,
  CHAR8   *TokenSpaceGuidName,
  CHAR8   *TokenName,
  UINT32  *Size
  )
;

/**
  Set PCD value buffer

  @param SkuName               SkuName String
  @param DefaultValueName      DefaultValueName String
  @param TokenSpaceGuidName    TokenSpaceGuidName String
  @param TokenName             TokenName String
  @param Size                  Size of PCD value
  @param Value                 Pointer to the updated PCD value buffer
**/
VOID
__PcdSetPtr (
  CHAR8   *SkuName             OPTIONAL,
  CHAR8   *DefaultValueName    OPTIONAL,
  CHAR8   *TokenSpaceGuidName,
  CHAR8   *TokenName,
  UINT32  Size,
  UINT8   *Value
  )
;

#define PcdGet(A, B, C, D)  __PcdGet(#A, #B, #C, #D)
#define PcdSet(A, B, C, D, Value)  __PcdSet(#A, #B, #C, #D, Value)
#define PcdGetPtr(A, B, C, D, Size)  __PcdGetPtr(#A, #B, #C, #D, Size)
#define PcdSetPtr(A, B, C, D, Size, Value)  __PcdSetPtr(#A, #B, #C, #D, Size, Value)

#endif
