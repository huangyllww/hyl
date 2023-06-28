//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file TseFunctionsInit.c
    Contains implementation for creating index json file with all the mapping strings and it's current values

**/

//---------------------------------------------------------------------------

#include "SetupDataCreationLib.h"

//---------------------------------------------------------------------------

extern EFI_BOOT_SERVICES    *pBS;


//---------------------------------------------------------------------------

#define SETUP_VARIABLE_GUID \
    { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } }


static SETUP_PKG _SetupPackageTemplate = {
        {'$', 'S', 'P', 'F'}, //PackageSignature[4];
        0x0102,             //StructureRevision;
        0x100,              //PackageVersion;
        {0,0,0,0,0,0},      //Reserved[6];
        SETUP_VARIABLE_GUID, //PackageGuid;
        0,                  //PackageVariable;
        0,                  //PackageScreen;
        0,                  //PackagePageList;
        0,                  //PackageControlOffset;
        0,                  //PackageVariableList;
        0,                  //PackageHpkData;
        0,                  //PackageGuidList;
        0,                  //PackagePageIdList;
        0                   //PackageIfrData;
};

static SCREEN_INFO _SetupPackageScreen = {
    {0},                    //MainHandle;
    0,     //MainTitle;
    0, //MainCopyright;
    0,       //MainHelpTitle;
    0,                  //MainMenuCount;
    0                   //MainNavCount;
};


static PAGE_INFO _SetupPageInfo =
{
        {0},            //      PageHandle;
        0,          //      PageIdIndex;
        0,          //      PageFormID;
        0,          //      PageTitle;
        0,          //      PageSubTitle;
        0,          //      PageID;
        0,          //      PageParentID;
        {0},            //      PAGE_FLAGS      PageFlags;
        0,          //      PageVariable;
        {0},        //      PageControls;
};

static CONTROL_INFO _SetupControlInfo_N_N = {
        {0},            //  EFI_HII_HANDLE  ControlHandle;
        0,          //  UINT32          ControlVariable;
        {0},            //  UINT32          ControlConditionalVariable[2];
        0,          //  UINT16          ControlType;
        0,          //  UINT16          ControlPageID;
        0xFFFF,         //  UINT16          ControlDestPageID;
        {0},        //  CONTROL_FLAGS   ControlFlags;
        0,          //  UINT16          ControlHelp;
        0,          //  UINT16          ControlLabel;
        0,          //  UINT16          ControlIndex;
        0,          //  UINT16          ControlLabelCount;
        {0},            //  union {
                    //  UINTN           ControlPtr;
                    //  UINT64  res1;
                    //  };
        {0},            //  union {
                    //  INTN            ControlConditionalPtr;
                    //  UINT64  res2;
                    //  };A
        0,          //  UINT16          ControlDataLength;
        0,          //  UINT16          ControlDataWidth;
        0           //  UINT16          QuestionId;
};

#define _VARIABLE_COUNT     31
#define _GUID_COUNT         20

static VARIABLE_INFO _SetupVariableInfo[_VARIABLE_COUNT] = {
    {SETUP_VARIABLE_GUID ,                      L"Setup",                       0x07,0},            //0
    {EFI_GLOBAL_VARIABLE_GUID ,                     L"PlatformLang",                0x17,0},            //1
    {EFI_GLOBAL_VARIABLE_GUID ,                     L"Timeout",                     0x07,0},            //2
    {USER_DEFAULTS_GUID ,                       L"UserDefaults",                0x07,0},            //3
    {ERROR_MANAGER_GUID ,                       L"ErrorManager",                0x07,0},            //4
    {AMITSESETUP_GUID ,                             L"AMITSESetup",             0x07,0},            //5
    {IDE_SECURITY_CONFIG_GUID ,                     L"IDESecDev",                   0x07,0},            //6
    {EFI_GLOBAL_VARIABLE_GUID ,                     L"BootOrder",                   0x17,0},            //7
    {LEGACY_DEV_ORDER_GUID ,                    L"LegacyDevOrder",          0x17,0},            //8
    {DEL_BOOT_OPTION_GUID ,                         L"DelBootOption",               0x17,0},            //9
    {ADD_BOOT_OPTION_GUID ,                         L"AddBootOption",               0x17,0},            //10
    {BOOT_MANAGER_GUID ,                            L"BootManager",             0x07,0},            //11
    {BOOT_NOW_COUNT_GUID ,                      L"BootNowCount",                0x07,0},            //12
    {LEGACY_DEV_ORDER_GUID ,                    L"LegacyDev",                   0x01,0},            //13
    {AMI_CALLBACK_GUID ,                            L"AMICallback",             0x01,0},            //14
    {LEGACY_DEV_ORDER_GUID ,                    L"LegacyGroup",             0x01,0},            //15
    {BOOT_NOW_COUNT_GUID ,                      L"SetupSaveState",              0x01,0},            //16
    {OEM_TSE_VAR_GUID ,                             L"OemTSEVar",               0x17,0},            //17
    {DYNAMIC_PAGE_COUNT_GUID ,                  L"DynamicPageCount",            0x17,0},            //18
    {AMITSE_DRIVER_HEALTH_ENB_GUID ,            L"DriverHlthEnable",            0x17,0},            //19
    {AMITSE_DRIVER_HEALTH_GUID ,                L"DriverHealthCount",           0x17,0},            //20
    {AMITSE_DRIVER_HEALTH_CTRL_GUID ,       L"DrvHealthCtrlCnt",                0x17,0},            //21
    {DRIVER_MANAGER_GUID ,                      L"DriverManager",               0x17,0},            //22
    {EFI_GLOBAL_VARIABLE_GUID ,                     L"DriverOrder",                 0x17,0},            //23
    {ADD_DRIVER_OPTION_GUID ,                   L"AddDriverOption",         0x17,0},        //24
    {DEL_DRIVER_OPTION_GUID ,                   L"DelDriverOption",             0x17,0},        //25
    {OEM_PORT_VARIABEL1 ,                       L"OemPortVar1",             0x17,0},            //26
    {OEM_PORT_VARIABEL2 ,                       L"OemPortVar2",             0x17,0},            //27
    {OEM_PORT_VARIABEL3 ,                       L"OemPortVar3",             0x02,0},            //28
    {OEM_PORT_VARIABEL4 ,                       L"OemPortVar4",             0x02,0},            //29
    {OEM_PORT_VARIABEL5 ,                       L"OemPortVar5",             0x02,0}         //30

};

#define AMIDIAG_GUID    \
    { 0x8BDDF24B, 0x2C8A, 0x4360, 0xA3, 0xBB, 0xAE, 0xF0, 0x94, 0x98, 0x3B, 0xEA }

static GUID_INFO _SetupGUIDInfo[_GUID_COUNT] = {
    // Needs to be first on List for patching up later.
    {EFI_GLOBAL_VARIABLE_GUID ,         L"Dummy",               {1},5}, // For lang codes/platformlangcode  0
    {BOOT_MANAGER_GUID ,                    L"BootManager",     {1},1}, //1
    {BOOT_NOW_COUNT_GUID ,              L"BootNowCount",        {1},1}, //2
    {LEGACY_DEV_ORDER_GUID ,            L"LegacyDevOrder",  {1},7}, //3
    {AMI_CALLBACK_GUID ,                    L"AMICallback",     {1},2}, //4
    {ERROR_MANAGER_GUID ,               L"ErrorManager",        {9},3}, //5
    {EFI_GLOBAL_VARIABLE ,                  L"PlatformLangCodes",   {7},5}, //6
    {EFI_SHELL_FILE_GUID ,                  L"shellx64.efi",            {6},0}, //7
    {EFI_SHELL_FILE_GUID ,                  L"shellpbm.efi",            {6},0}, //8
    {IDE_SECURITY_CONFIG_GUID ,         L"IDESecDev",           {1},8}, //9
    {ADD_BOOT_OPTION_GUID ,             L"AddBootOption",       {0},9}, //10
    {OEM_TSE_VAR_GUID ,                 L"OemTSEVar",       {1},10},    //11
    {DYNAMIC_PAGE_COUNT_GUID ,          L"DynamicPageCount",    {0},14},    //12
    {AMITSE_DRIVER_HEALTH_ENB_GUID ,    L"DriverHlthEnable",    {1},15},    //13
    {AMITSE_DRIVER_HEALTH_GUID ,        L"DriverHealthCount",   {1},16},    //14
    {AMITSE_DRIVER_HEALTH_CTRL_GUID ,   L"DrvHealthCtrlCnt",        {1},17},    //15
    {DRIVER_MANAGER_GUID ,              L"DriverManager",       {0},18},    //16
    {OEM_PORT_GUID1 ,                       L"OemPortGuid1",        {0},19},    //17
    {OEM_PORT_GUID2 ,                       L"OemPortGuid2",        {0},20},    //18
    {AMIDIAG_GUID ,                         L"AMIDiag",         {2},21} //19
};

#pragma pack(8)
typedef struct _TSE_SETUP_PACKAGE
{
    SETUP_PKG       SetupPkg;
    SCREEN_INFO     ScreenInfo;
    PAGE_LIST       PageList;
    UINT32          PageOffset[1]; // Increase if you have more pages - Dummy
    PAGE_INFO       PageInfo[1];   // Increase if you have more pages - Dummy

    CONTROL_INFO    ControlInfo[1];// Increase if controls  - Dummy

    VARIABLE_LIST   VariableList;
    UINT32          VariableOffset[_VARIABLE_COUNT]; // Increase if you have more Variable
    VARIABLE_INFO   VariableInfo[_VARIABLE_COUNT];
    HPK_INFO        HpkInfo[1]; //- Dummy
    GUID_LIST       GuidList;
    UINT32          GuidOffset[_GUID_COUNT]; // Increase if you have more GUID
    GUID_INFO       GuidInfo[_GUID_COUNT];
    PAGE_ID_LIST    PageIdList;
    UINT32          PageIdOffset[1]; // Increase if you have more Page - Dummy
    PAGE_ID_INFO    PageIdInfo[1];  // Increase if you have more Page - Dummy
}TSE_SETUP_PACKAGE;

UINT8           *gApplicationData;
SETUP_PKG       *gSetupPkg;
SCREEN_INFO     *gScreen;
PAGE_LIST       *gToolPages;
PAGE_LIST       *gPages;
CONTROL_INFO    *gToolControlInfo;
CONTROL_INFO    *gControlInfo;
VARIABLE_LIST   *gToolVariables;
VARIABLE_LIST   *gVariables;
HPK_INFO        *gHpkInfo;
GUID_LIST       *gGuidList;
PAGE_ID_LIST    *gPageIdList;
PAGE_ID_LIST    *gToolPageIdList;
PAGE_ID_INFO    *gPageIdInfo;
PAGE_ID_INFO    *gToolPageIdInfo;
UINTN           gHpkInfoLength;

BOOLEAN         gBrowserCallbackEnabled = FALSE; 
BOOLEAN         gEnableDrvNotification = FALSE; //TRUE if allow notification function to process action, FALSE to ignore the notification

UINTN           gLangCount;
CHAR16          gPlatformLang [20];         
CHAR16          gCachePlatformLang[20];//To Save the Current PlatformLang for Cache purpose.
LANGUAGE_DATA   *gLanguages;
VOID            *gHiiHandle;
EFI_HII_DATABASE_PROTOCOL *gHiiDatabase;
#pragma pack()

/**
    function to Initialize all the global pointer declared.

    @param void

    @retval void

**/
VOID InitGlobalPointers( VOID )
{
    UINT8   *data;

    gSetupPkg = (SETUP_PKG *)gApplicationData;
    data = (UINT8 *)gSetupPkg;

    gScreen = (SCREEN_INFO *)(data + gSetupPkg->PackageScreen);
    gToolPages = gPages = (PAGE_LIST *)(data + gSetupPkg->PackagePageList);
    gToolControlInfo = (CONTROL_INFO *) (data + gSetupPkg->PackageControlOffset);
    gToolVariables = gVariables = (VARIABLE_LIST *)(data + gSetupPkg->PackageVariableList);
    gHpkInfo = (HPK_INFO *)(data + gSetupPkg->PackageHpkData);
    gHpkInfoLength = gSetupPkg->PackageGuidList - gSetupPkg->PackageHpkData;
    gGuidList = (GUID_LIST *)(data + gSetupPkg->PackageGuidList);
    gToolPageIdList = gPageIdList = (PAGE_ID_LIST *) (data + gSetupPkg->PackagePageIdList);
    gToolPageIdInfo = gPageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gToolPageIdList) + gToolPageIdList->PageIdList[0]);
}

void InitHiiAndVariables (void)
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    InitGlobalPointers ();       // To initialize global buffers
    VarLoadVariables ((VOID **)&gVariableList, NULL);
    Status = HiiFixupData ();
    if (EFI_ERROR (Status))
        return;
    Status = VarBuildDefaults ();
}
        
/**
    function to initialize the application data

    @param ImageHandle 

    @retval EFI_STATUS

**/
EFI_STATUS  InitApplicationData ()
{
    EFI_STATUS  Status=EFI_SUCCESS;
    UINTN i;
    CHAR16 *name = NULL;

    // Init the Application data
    // Hardcoded Application data for the TSE Lite
    gApplicationData = EfiLibAllocateZeroPool( 0x2000 ); // Allocate more of you add more

    // SETUP_PKG
    MemCopy(gApplicationData,&_SetupPackageTemplate, sizeof(_SetupPackageTemplate) );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageVariable = 0;
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageScreen = STRUCT_OFFSET( TSE_SETUP_PACKAGE, ScreenInfo );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackagePageList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, PageList );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageControlOffset = STRUCT_OFFSET( TSE_SETUP_PACKAGE, ControlInfo );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageVariableList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, VariableList );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageHpkData = STRUCT_OFFSET( TSE_SETUP_PACKAGE, HpkInfo );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageGuidList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, GuidList );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackagePageIdList = STRUCT_OFFSET( TSE_SETUP_PACKAGE, PageIdList );
    ((TSE_SETUP_PACKAGE *)gApplicationData)->SetupPkg.PackageIfrData = sizeof(TSE_SETUP_PACKAGE);

    //SCREEN_INFO
    MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->ScreenInfo,&_SetupPackageScreen,sizeof(SCREEN_INFO));

    //PAGE_INFO
    MemSet(&_SetupPageInfo,sizeof(_SetupPageInfo),0);
    MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->PageInfo,&_SetupPageInfo,sizeof(_SetupPageInfo));

    //PAGE_LIST
    ((TSE_SETUP_PACKAGE *)gApplicationData)->PageList.PageCount = 1;
    ((TSE_SETUP_PACKAGE *)gApplicationData)->PageList.PageList[0] = (UINT32)((UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->PageInfo[0])-(UINTN)(gApplicationData));

    //CONTROL_INFO
    MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->ControlInfo,&_SetupControlInfo_N_N,sizeof(CONTROL_INFO));

    //VARIABLE_INFO
    name = GetVariableNameByID(VARIABLE_ID_LANGUAGE);
    EfiStrCpy(_SetupVariableInfo[VARIABLE_ID_LANGUAGE].VariableName, name);
    MemFreePointer(&name);
    MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->VariableInfo,&_SetupVariableInfo,sizeof(_SetupVariableInfo));

    //VARIABLE_LIST
    ((TSE_SETUP_PACKAGE *)gApplicationData)->VariableList.VariableCount = _VARIABLE_COUNT;
    for(i=0;i<_VARIABLE_COUNT;i++)
        ((TSE_SETUP_PACKAGE *)gApplicationData)->VariableList.VariableList[i] = (UINT32)((UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->VariableInfo[i])-(UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->VariableList));

    //GUID_INFO
    name = GetGUIDNameByID(VARIABLE_ID_LANGUAGE);
    EfiStrCpy(_SetupGUIDInfo[0].GuidName , name);
    MemFreePointer(&name);
    MemCopy(&((TSE_SETUP_PACKAGE *)gApplicationData)->GuidInfo,_SetupGUIDInfo,sizeof(_SetupGUIDInfo));

    //GUID_LIST
    ((TSE_SETUP_PACKAGE *)gApplicationData)->GuidList.GuidCount = _GUID_COUNT;
    for(i=0;i<_GUID_COUNT;i++)
        ((TSE_SETUP_PACKAGE *)gApplicationData)->GuidList.GuidList[i] = (UINT32)((UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->GuidInfo[i])-(UINTN)(&((TSE_SETUP_PACKAGE *)gApplicationData)->GuidList));

    InitHiiAndVariables ();
    return Status;
}

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
