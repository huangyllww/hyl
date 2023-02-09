/* Definition for Compiler Option */
//#define DDR_MODE
#define CRTC_AUTODETECT

/* Definition for DP501 */
#define DP501_I2C_Channel		1
//#define DP501_I2C_Address		0x98	/* ??? */

#define DP501_DPCD_WRITE_DELAY	400
#define DP501_DPCD_READ_DELAY	1500

#define SINK_CONNECTED			0
#define SINK_NOT_CONNECTED		1

#define	DP						0
#define DVI						1

/* Training Status */
#define TRAINING_SUCCESS		0x00
#define CR_FAIL 				0x01
#define EQ_FAIL					0x02

/* Definition for DP501 Reg */
/* IRQ Status */
#define IRQS_Plug_inEvent		0x40
#define IRQS_Plug_outEvent		0x10
#define IRQS_Plug_toggleEvent	0x04
#define IRQS_LinkTrainingFinish	0x02
#define IRQS_DPCDReadDone		0x01
#define IRQS_AUXCH_Error		0x08
#define IRQS_DirectAccAuxDone	0x20

/* DPCD Reg */
#define DCPD103							0x103
  #define DPCD103_VOLSWINGSET			0x03
  #define DPCD103_MAX_SWINGREACH 		0x04
  #define DPCD103_PREEMPHASIS			0x11
  #define DPCD103_MAX_PREEMPHASIS		0x20
  #define DPCD103_VOLSWINGSET_SHIFT 	0
  #define DPCD103_MAX_SWINGREACH_SHIFT 	2
  #define DPCD103_PREEMPHASIS_SHIFT		3
  #define DPCD103_MAX_PREEMPHASIS_SHIFT 5

#define DPCD202     					0x202
  #define DPCD202_LANE0_CR_DONE			0x01
  #define DPCD202_LANE0_CHANNEL_EQ_DONE	0x02
  #define DPCD202_LANE0_SYMBOL_LOCKED	0x04
  #define DPCD202_LANE1_CR_DONE			0x10
  #define DPCD202_LANE1_CHANNEL_EQ_DONE	0x20
  #define DPCD202_LANE1_SYMBOL_LOCKED	0x40
 
#define DPCD203     					0x203
  #define DPCD203_LANE2_CR_DONE			0x01
  #define DPCD203_LANE2_CHANNEL_EQ_DONE	0x02
  #define DPCD203_LANE2_SYMBOL_LOCKED	0x04  
  #define DPCD203_LANE3_CR_DONE			0x10
  #define DPCD203_LANE3_CHANNEL_EQ_DONE	0x20
  #define DPCD203_LANE3_SYMBOL_LOCKED	0x40
   
#define	DPCD204							0x204
  #define DPCD204_INTERLANE_ALIGN_DONE	0x01
   
/* Export */
void Enable_Video_Output(_DEVInfo *DevInfo);
void Disable_Video_Output(_DEVInfo *DevInfo);
BOOL DP501_Find(_DEVInfo *DevInfo);

