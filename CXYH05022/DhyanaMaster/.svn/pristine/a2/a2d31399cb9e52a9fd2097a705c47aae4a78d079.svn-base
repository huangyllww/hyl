#include "AmdCpmCommon.h"


HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE0 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE0) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE
    //      Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue...
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x10e21000,0xFFFFFFFF,8,15,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00e61000,0xFFFFFFFF,8,15,TRUE,5),            
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_DX,0,0x10e21000,0xFFFFFFFF,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_DX,0,0x00e61000,0xFFFFFFFF,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_FORCE,pcie_gen4_ttx_cursor_before_training,CPU_HYGON_CX,0,0xFFFFFFFF,0xFFFFFFFF,8,15,TRUE,TX_EQ(3,44,1)),
    PCIE_EQ(0xFF,FLAG_FORCE,pcie_gen4_ttx_cursor,CPU_HYGON_DX,0,0xFFFFFFFF,0xFFFFFFFF,8,15,TRUE,TX_EQ(0,24,0)),
    PCIE_PHY_PARA_OVERRIDE(FLAG_FORCE,pcie_phy_para_override,CPU_HYGON_CX,0,0xFFFFFFFF,0xFFFFFFFF,8,15,0xac, 0x18, 0xb6, 0x7d, 0x07, 0x48, 0x11, 0x01, 0x01),    
    
    //SGPZT#7125-Improting Hygon preset data -S
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x005d1000,0x93611000,8,15,TRUE,3),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x30901000,8,15,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00971000,8,15,TRUE,4),
    

    

    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00CE1000,0x93711000,8,15,TRUE,4),
    


    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00981000,8,15,TRUE,4),



    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x10E21000,0x40101000,8,15,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x10E21000,0x40001000,8,15,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00E61000,0x40501000,8,15,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00E61000,0x40601000,8,15,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x005d1000,0x93611000,8,15,TRUE,3),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00971000,8,15,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00981000,8,15,TRUE,4),
    //SGPZT#7125-Improting Hygon preset data -E----18

    //SGPZT#7125-Improting Hygon preset data 2 -S
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x005D1000,0x93611000,8,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x30901000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00971000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00CE1000,0x93711000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00981000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x10E21000,0x40101000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x10E21000,0x40001000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00E61000,0x40501000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00E61000,0x40601000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x005D1000,0x93611000,8,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00971000,8,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x00971000,0x00981000,8,15,TRUE,4),
    //SGPZT#7125-Improting Hygon preset data 2 -E

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,0,0x20A11EB6,0x21281EB6,8,15,TRUE,4), //MEZZ 24003226 P4

    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0}}
  }       
};

HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE1 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE

    //SGPZT#7125-Improting Hygon preset data -S
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,1,0x101D15B3,0x000615B3,8,15,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,1,0x101915B3,0x006715B3,8,15,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,1,0x101715B3,0x005215B3,8,15,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,1,0x101715B3,0x006815B3,8,15,TRUE,4),//OCP 24001883 P4

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,1,0x101f15b3,0x000415b3,8,15,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,1,0x101515b3,0x004715b3,8,15,TRUE,7),
    //SGPZT#7125-Improting Hygon preset data -E

    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0}}
  }       
};

HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE2 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE
    //      Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue...
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101915b3,0xFFFFFFFF,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101915b3,0xFFFFFFFF,24,31,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x102115B3,0x002215B3,16,31,TRUE,7),//Add PCIe device preset from 2023-Q1
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101D15b3,0x003915B3,16,31,TRUE,8),//MLX CX6 1 +
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101D15b3,0x001615B3,16,31,TRUE,6),//MLX CX6 2 +

    //SGPZT#7125-Improting Hygon preset data -S
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x005d1000,0x93611000,16,23,TRUE,6),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x005d1000,0x93611000,24,31,TRUE,6),
    
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x30901000,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x30901000,24,31,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00971000,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00971000,24,31,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101515B3,0x000415B3,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101515B3,0x000415B3,24,31,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101515B3,0x000315B3,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101515B3,0x000315B3,24,31,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00CE1000,0x93711000,16,23,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00CE1000,0x93711000,24,31,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101715B3,0x002015B3,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101715B3,0x002015B3,24,31,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00981000,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00981000,24,31,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101f15b3,0x000115b3,16,23,TRUE,1), //Riser A 24002228 Slot1 P1
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101f15b3,0x000115b3,24,31,TRUE,2), //Riser A 24002228 Slot2 P2

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x10E21000,0x40101000,16,23,TRUE,0),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x10E21000,0x40101000,24,31,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x10E21000,0x40001000,16,23,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x10E21000,0x40001000,24,31,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00E61000,0x40501000,16,23,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00E61000,0x40501000,24,31,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00E61000,0x40601000,16,23,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00E61000,0x40601000,24,31,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x005d1000,0x93611000,16,23,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x005d1000,0x93611000,24,31,TRUE,5),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00971000,16,23,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00971000,24,31,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00981000,16,23,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00981000,24,31,TRUE,4),
    //SGPZT#7125-Improting Hygon preset data -E

    //SGPZT#7125-Improting Hygon preset data 2 -S
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x005D1000,0x93611000,16,31,TRUE,5),//
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x30901000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00971000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00CE1000,0x93711000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00981000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x10E21000,0x40101000,16,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x10E21000,0x40001000,16,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00E61000,0x40501000,16,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00E61000,0x40601000,16,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x005D1000,0x93611000,16,31,TRUE,4),//
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00971000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x00971000,0x00981000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101515B3,0x000415B3,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101515B3,0x000315B3,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101715B3,0x002015B3,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101F15B3,0x000115B3,16,31,TRUE,5), //Riser A 24002816 Slot1 P5
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101D15B3,0x001115B3,16,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101B15B3,0x000815B3,16,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101D15B3,0x101815B3,16,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x101B15B3,0x000715B3,16,31,TRUE,7),
    //SGPZT#7125-Improting Hygon preset data 2 -E

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x20A11EB6,0x21201EB6,16,31,TRUE,4), //Riser A 24002816  24003225 Slot1 P4

    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0}}
  }       
};

HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE3 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE3) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE
    //      Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue...
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_DX,3,0x23b71d94,0xFFFFFFFF,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x102115B3,0x002215B3,24,31,TRUE,7),//Add PCIe device preset from 2023-Q1

    //SGPZT#7125-Improting Hygon preset data -S
    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x005d1000,0x93611000,24,31,TRUE,5),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x30901000,24,31,TRUE,4),
  
    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00971000,24,31,TRUE,3),
    
    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101515B3,0x000415B3,24,31,TRUE,4),
    
    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101515B3,0x000315B3,24,31,TRUE,4),
    
    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00CE1000,0x93711000,24,31,TRUE,4),
    
    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101715B3,0x002015B3,24,31,TRUE,4),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00981000,24,31,TRUE,4),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101f15b3,0x000115b3,24,31,TRUE,4), //Riser A 24002228 Slot3 P4

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x10E21000,0x40101000,24,31,TRUE,7),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x10E21000,0x40001000,24,31,TRUE,7),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00E61000,0x40501000,24,31,TRUE,7),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00E61000,0x40601000,24,31,TRUE,7),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x005d1000,0x93611000,24,31,TRUE,5),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00971000,24,31,TRUE,4),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00981000,24,31,TRUE,4),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101715B3,0x000715B3,24,31,TRUE,4),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101715B3,0x000615B3,24,31,TRUE,4),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101B15B3,0x000615B3,24,31,TRUE,6),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101D15B3,0x001115B3,24,31,TRUE,5),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101B15B3,0x000815B3,24,31,TRUE,5),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101D15B3,0x001815B3,24,31,TRUE,5),

    PCIE_EQ(0x19,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101B15B3,0x000715B3,24,31,TRUE,5),

    //SGPZT#7125-Improting Hygon preset data 1 -E

    //SGPZT#7125-Improting Hygon preset data 2 -S
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x005D1000,0x93611000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x30901000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00971000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00CE1000,0x93711000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00981000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x10E21000,0x40101000,24,31,TRUE,7),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x10E21000,0x40001000,24,31,TRUE,7),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00E61000,0x40501000,24,31,TRUE,7),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00E61000,0x40601000,24,31,TRUE,8),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x005D1000,0x93611000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00971000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x00971000,0x00981000,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101515B3,0x000415B3,24,31,TRUE,5),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101515B3,0x000315B3,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101715B3,0x002015B3,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101F15B3,0x000115B3,24,31,TRUE,3),//Riser A 24002816 Slot2 P3
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101D15B3,0x001115B3,24,31,TRUE,5),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101B15B3,0x000815B3,24,31,TRUE,4),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101D15B3,0x101815B3,24,31,TRUE,5),
    PCIE_EQ(0x1A,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,3,0x101B15B3,0x000715B3,24,31,TRUE,5),
    //SGPZT#7125-Improting Hygon preset data 2 -E
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,2,0x20A11EB6,0x21201EB6,16,31,TRUE,4), //Riser A 24002816|24002228  24003225 Slot2|Slot3 P4 
    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0}}
  }       
};

HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE4 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE4) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE
    //      Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue...
    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0} }
  }       
};

HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE5 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE5) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE
    //      Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue...

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x23b71d94,0xFFFFFFFF,0,15,TRUE,8),
    PCIE_PHY_PARA_OVERRIDE(FLAG_FORCE,pcie_phy_para_override,CPU_HYGON_CX,5,0x23b71d94,0xFFFFFFFF,0,15,0xac, 0x18, 0xb6, 0x7d, 0x07, 0x48, 0x55, 0x01, 0x01),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_DX,5,0x23b71d94,0xFFFFFFFF,0,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x102115B3,0x002215B3,0,15,TRUE,5),//Add PCIe device preset from 2023-Q1
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101D15b3,0x003915B3,0,15,TRUE,8),//MLX CX6 1 +
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101D15b3,0x001615B3,0,15,TRUE,4),//MLX CX6 2 +

    //SGPZT#7125-Improting Hygon preset data -S
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x005d1000,0x93611000,0,7,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x005d1000,0x93611000,8,15,TRUE,5),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x30901000,0,7,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x30901000,8,15,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00971000,0,7,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00971000,8,15,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101515B3,0x000415B3,0,7,TRUE,1),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101515B3,0x000415B3,8,15,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101515B3,0x000315B3,0,7,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101515B3,0x000315B3,8,15,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00CE1000,0x93711000,0,7,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00CE1000,0x93711000,8,15,TRUE,4),
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101715B3,0x002015B3,0,7,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101715B3,0x002015B3,8,15,TRUE,3),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00981000,0,7,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00981000,8,15,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101f15b3,0x000115b3,0,7,TRUE,5),  //Riser B 24002228 Slot1 P5
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101f15b3,0x000115b3,8,15,TRUE,5), //Riser B 24002228 Slot2 P5

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x10E21000,0x40101000,0,7,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x10E21000,0x40101000,8,15,TRUE,0),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x10E21000,0x40001000,0,7,TRUE,6),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x10E21000,0x40001000,8,15,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00E61000,0x40501000,0,7,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00E61000,0x40501000,8,15,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00E61000,0x40601000,0,7,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00E61000,0x40601000,8,15,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x005d1000,0x93611000,0,7,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x005d1000,0x93611000,8,15,TRUE,5),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00971000,0,7,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00971000,8,15,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00981000,0,7,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00981000,8,15,TRUE,4),
    //SGPZT#7125-Improting Hygon preset data -E

    //SGPZT#7125-Improting Hygon preset data 2 -S
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x005D1000,0x93611000,0,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x30901000,0,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00971000,0,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00CE1000,0x93711000,0,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00981000,0,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x10E21000,0x40101000,0,15,TRUE,8),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x10E21000,0x40001000,0,15,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00E61000,0x40501000,0,15,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00E61000,0x40601000,0,15,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x005D1000,0x93611000,0,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00971000,0,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x00971000,0x00981000,0,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101515B3,0x000415B3,0,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101515B3,0x000315B3,0,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101715B3,0x002015B3,0,15,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101F15B3,0x000115B3,0,15,TRUE,2),//Riser B 24002816 Slot1 P2
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101D15B3,0x001115B3,0,15,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101B15B3,0x000815B3,0,15,TRUE,6),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101D15B3,0x101815B3,0,15,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,5,0x101B15B3,0x000715B3,0,15,TRUE,5),
    
    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0}}
  }       
};

HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE6 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE6) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE
    //      Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue...
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101915b3,0xFFFFFFFF,16,31,TRUE,4),  
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x0370cabc,0xFFFFFFFF,16,31,TRUE,4),
    PCIE_PHY_PARA_OVERRIDE(FLAG_FORCE,pcie_phy_para_override,CPU_HYGON_CX,6,0x0370cabc,0xFFFFFFFF,8,15,0xac, 0x18, 0xb6, 0x7d, 0x07, 0x48, 0x55, 0x01, 0x01),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x102115B3,0x002215B3,16,31,TRUE,7), //Add PCIe device preset from 2023-Q1
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15b3,0x003915B3,16,31,TRUE,7),//MLX CX6 1 +
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15b3,0x001615B3,16,31,TRUE,7),//MLX CX6 2 +
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15b3,0x003915B3,16,31,TRUE,5),//MLX CX6 1 +
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15b3,0x001615B3,16,31,TRUE,6),//MLX CX6 2 +

    //SGPZT#7125-Improting Hygon preset data -S
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x005d1000,0x93611000,16,31,TRUE,5),
    
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x30901000,16,31,TRUE,4),
    
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00971000,16,31,TRUE,4),
    
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101515B3,0x000415B3,16,31,TRUE,4),
    
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101515B3,0x000315B3,16,31,TRUE,4),
    
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00CE1000,0x93711000,16,31,TRUE,5),
    
    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101715B3,0x002015B3,16,31,TRUE,4),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00981000,16,31,TRUE,4),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101f15b3,0x000115b3,16,31,TRUE,1), //Riser B 24002228 Slot3 P1

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x10E21000,0x40101000,16,31,TRUE,0),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x10E21000,0x40001000,16,31,TRUE,7),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00E61000,0x40501000,16,31,TRUE,7),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00E61000,0x40601000,16,31,TRUE,7),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x005d1000,0x93611000,16,31,TRUE,5),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00971000,16,31,TRUE,5),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00981000,16,31,TRUE,4),



    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101715B3,0x000715B3,16,31,TRUE,4),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101715B3,0x000615B3,16,31,TRUE,3),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101B15B3,0x000615B3,16,31,TRUE,2),//24001866 P7->P2

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15B3,0x001115B3,16,31,TRUE,7),//24002780 P7->P2

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101B15B3,0x000815B3,16,31,TRUE,7),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15B3,0x001815B3,16,31,TRUE,5),

    PCIE_EQ(0x37,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101B15B3,0x000715B3,16,31,TRUE,7),
    //SGPZT#7125-Improting Hygon preset data -E
    //SGPZT#7125-Improting Hygon preset data 2 -S
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x005D1000,0x93611000,16,31,TRUE,5),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x30901000,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00971000,16,31,TRUE,5),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00CE1000,0x93711000,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00981000,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x10E21000,0x40101000,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x10E21000,0x40001000,16,31,TRUE,7),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00E61000,0x40501000,16,31,TRUE,7),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00E61000,0x40601000,16,31,TRUE,7),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x005D1000,0x93611000,16,31,TRUE,5),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00971000,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x00971000,0x00981000,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101515B3,0x000415B3,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101515B3,0x000315B3,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101715B3,0x002015B3,16,31,TRUE,4),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101F15B3,0x000115B3,16,31,TRUE,6), //Riser B 24002816 Slot2 P6
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15B3,0x001115B3,16,31,TRUE,7),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101B15B3,0x000815B3,16,31,TRUE,7),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101D15B3,0x101815B3,16,31,TRUE,7),
    PCIE_EQ(0x38,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,6,0x101B15B3,0x000715B3,16,31,TRUE,7),
    //SGPZT#7125-Improting Hygon preset data 2 -E


    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0}}
  }       
};

HYGON_CPM_EQ_CONFIG_TABLE   gPresetSetting_DIE7 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof ( gPresetSetting_DIE7) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    //PCIE
    //      Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue...
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x23b71d94,0xFFFFFFFF,16,31,TRUE,8),
    PCIE_PHY_PARA_OVERRIDE(FLAG_FORCE,pcie_phy_para_override,CPU_HYGON_CX,7,0x23b71d94,0xFFFFFFFF,16,31,0xac, 0x18, 0xb6, 0x7d, 0x07, 0x48, 0x55, 0x01, 0x01),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x102115B3,0x002215B3,16,31,TRUE,4),//Add PCIe device preset from 2023-Q1
    
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101D15b3, 0x003915B3,16,31,TRUE,7),//MLX CX6 1 +
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101D15b3, 0x001615B3,16,31,TRUE,7),//MLX CX6 2 +

    //SGPZT#7125-Improting Hygon preset data -S
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005d1000,0x93611000,24,31,TRUE,6),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005d1000,0x93611000,16,23,TRUE,5),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x30901000,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x30901000,16,23,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00971000,24,31,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00971000,16,23,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101515B3,0x000415B3,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101515B3,0x000415B3,16,23,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101515B3,0x000315B3,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101515B3,0x000315B3,16,23,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00CE1000,0x93711000,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00CE1000,0x93711000,16,23,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101715B3,0x002015B3,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101715B3,0x002015B3,16,23,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00981000,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00981000,16,23,TRUE,3),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101f15b3,0x000115b3,24,31,TRUE,3), //Riser C 24002209 Slot1 P3
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101f15b3,0x000115b3,16,23,TRUE,2), //Riser C 24002209 Slot2 P2

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x10E21000,0x40101000,24,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x10E21000,0x40101000,16,23,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x10E21000,0x40001000,24,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x10E21000,0x40001000,16,23,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00E61000,0x40501000,24,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00E61000,0x40501000,16,23,TRUE,7),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00E61000,0x40601000,24,31,TRUE,7),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00E61000,0x40601000,16,23,TRUE,5),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005d1000,0x93611000,24,31,TRUE,6),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005d1000,0x93611000,16,23,TRUE,5),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005d1000,0x93611000,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005d1000,0x93611000,16,23,TRUE,4),

    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00981000,24,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00981000,16,23,TRUE,4),
    //SGPZT#7125-Improting Hygon preset data -E

    //SGPZT#7125-Improting Hygon preset data 2 -S
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005D1000,0x93611000,16,31,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x30901000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00971000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00CE1000,0x93711000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00981000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x10E21000,0x40101000,16,31,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x10E21000,0x40001000,16,31,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00E61000,0x40501000,16,31,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00E61000,0x40601000,16,31,TRUE,6),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x005D1000,0x93611000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00971000,16,31,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x00971000,0x00981000,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101515B3,0x000415B3,16,31,TRUE,3),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101515B3,0x000315B3,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen3_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101715B3,0x002015B3,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101F15B3,0x000115B3,16,31,TRUE,5), //Riser C 24002807 Slot1 P5
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101D15B3,0x001115B3,16,31,TRUE,5),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101B15B3,0x000815B3,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101D15B3,0x101815B3,16,31,TRUE,4),
    PCIE_EQ(0xFF,FLAG_SPECIFIED,pcie_gen4_ttx_force_otherside_preset,CPU_HYGON_CX,7,0x101B15B3,0x000715B3,16,31,TRUE,4),
    //SGPZT#7125-Improting Hygon preset data 2 -E
    {0xFF,EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0}}
  }       
};

VOID *HygonPresetTableList[] = {
        &gPresetSetting_DIE0,
        &gPresetSetting_DIE1,
        &gPresetSetting_DIE2,
        &gPresetSetting_DIE3,
        &gPresetSetting_DIE4,
        &gPresetSetting_DIE5,
        &gPresetSetting_DIE6,
        &gPresetSetting_DIE7,
        NULL
};
