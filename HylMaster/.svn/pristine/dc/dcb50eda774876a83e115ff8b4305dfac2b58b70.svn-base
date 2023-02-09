/** @file
  This file contains OemPCIeDevPlatLib Use to Support OemPCIeDev Module
  
  Author:Kangmm
  Date:  20210120
  

  
Copyright (C) 2020 - 2021 Cancon Industry Co.,LTD.All rights reserved.<BR> 

**/

#ifndef _BASE_GET_RB_BUS_LIB_H_
#define _BASE_GET_RB_BUS_LIB_H_


BOOLEAN IsSOCSSP ();
BOOLEAN IsSOCGn ();

UINT8 GetCPURbBusNumAndFabricId(
    UINT8            SocketNum,
    UINT8            DieNum,
    UINT8            *FabricId
  );
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit +>
UINT8 GetCPURbBusNumLimitAndFabricId(
    UINT8            SocketNum,
    UINT8            DieNum,
    UINT8            *FabricId
  );
#endif
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit ->
