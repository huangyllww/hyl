#ifndef _AMD_CPM_OEM_H_
#define _AMD_CPM_OEM_H_

//
// The following items need to be defined according to the code base.
//
// AMD_CPM_SETUP_VARIABLE_NAME
// AMD_CPM_SETUP_GUID
// AMD_CPM_SETUP_OPTION
//
// AMD_CPM_SETUP_OPTION_SPECIAL_VGA_FEATURE
// AMD_CPM_SETUP_OPTION_POWER_XPRESS_DYNAMIC_MODE
// AMD_CPM_SETUP_OPTION_PRIMARY_VIDEO_ADAPTOR
// AMD_CPM_SETUP_OPTION_LOOPBACK_ADAPTOR
// AMD_CPM_SETUP_OPTION_DISPLAY_OUTPUT
// AMD_CPM_SETUP_OPTION_BRIGHTNESS_CONTROL_METHOD
// AMD_CPM_SETUP_OPTION_BLUE_TOOTH_EN
// AMD_CPM_SETUP_OPTION_ZERO_POWER_ODD_EN
// AMD_CPM_SETUP_OPTION_UNUSED_GPP_CLOCK_OFF
// AMD_CPM_SETUP_OPTION_SYSTEM_BOOT_WITH_PS0
// AMD_CPM_SETUP_OPTION_ACPI_THERMAL_FAN_EN
//

/// Redriver type
typedef enum {
  BR401A = 0x00,           ///< Add-in Card ID for Field
  BR800                    ///< Add-in Card ID for Drift
} REDRIVER_TYPE;

/// Redriver Profile
typedef struct {
  REDRIVER_TYPE   Type;                    ///< Re-Driver model
  UINT8           Channel;                 ///< I2CMUX channel
  UINT8           SlaveAddress;            ///< Slave Address
  UINT8           EQ;                      ///< value
  UINT8           VOD;                     ///< value
  UINT8           DEM;                     ///< value
} REDRIVER_PROFILE;
#endif
