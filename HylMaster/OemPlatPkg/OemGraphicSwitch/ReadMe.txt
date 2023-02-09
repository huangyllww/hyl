--------------------------------------------------------------------------
@Description:
    OemGraphicSwitch Module
    板载和外接显卡切换功能
    Onboard or External, External first，优先外接显卡显示，有外接显卡，禁用板载显卡（OS 下不可见设备），若无外接显卡，使用板载显卡
    Onboard and External, Onboard first，优先板载显卡显示
    Onboard and External, External first，优先外接显卡显示，同时存在板载和外接显卡设备

@Parts:
    DisableOnboardVideo
    IpmiSetBMCGPIO
    OemPciDxeInitLib

@SDL Setting：
    DISPLAY_WITH_FIXED_SLOT
    PCI_SLOT_BRIDGE_LIST
    DEFAULT_EXTERNAL_VGA_SLOT

@Porting Part:
    ONBOARD_VIDEO_DISABLE_BY_GPIO  - 通过GPIO 禁用板载VGA 设备
    ONBOARD_VIDEO_DISABLE_BY_DXIO  - 通过DXIO 禁用板载VGA 设备