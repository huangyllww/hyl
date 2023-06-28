/** @file
  This file contains hooks declaration for PeiPciEnumeration Module
  
  $Revision:0A
  $Date:  Machong 2017/07/22
  
Copyright (C) 2016 - 2017 Dawning Information Industry Co.,LTD.All rights reserved.<BR>
 
**/

#ifndef __PEI_PCI_ENUMHOOK__H__
#define __PEI_PCI_ENUMHOOK__H__
#ifdef __cplusplus
extern "C" {
#endif

extern VOID ASTInitFunc(EFI_PHYSICAL_ADDRESS);
extern VOID ASTInitBridgeFunc(EFI_PHYSICAL_ADDRESS);
extern VOID AmdBridgeInitFunc(EFI_PHYSICAL_ADDRESS);

//<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>
extern VOID AmdRootPortMemoryAlign(UINT64*);
extern VOID AstRootMemoryAlign(UINT64*);
extern VOID AstVgaMemoryAlign(UINT64*);
//<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
