

#ifndef _IR_BRIDGE_H_
#define _IR_BRIDGE_H_

void InfraredTxInit( void );

void InfraredNecTxExecute( uint16_t add, uint16_t cmd );
void InfraredRc5TxExecute( uint16_t add, uint16_t cmd );

void InfraredNecCodeSend( uint32_t code );
void InfraredRc5CodeSend( uint32_t code );
void InfraredRc6CodeSend( uint32_t code );
void InfraredSamsungCodeSend( uint32_t code );
void InfraredLegrandCodeSend( uint32_t code );


// Samsung key codes
#define SMG_MESG_KEY_0    0x900
#define SMG_MESG_KEY_1    0x901
#define SMG_MESG_KEY_2    0x902
#define SMG_MESG_KEY_3    0x903
#define SMG_MESG_KEY_4    0x904
#define SMG_MESG_KEY_5    0x905
#define SMG_MESG_KEY_6    0x906
#define SMG_MESG_KEY_7    0x907
#define SMG_MESG_KEY_8    0x908
#define SMG_MESG_KEY_9    0x909

#define SMG_MESG_KEY_CHP  0x911
#define SMG_MESG_KEY_CHM  0x913

#define SMG_MESG_KEY_VP   0x910
#define SMG_MESG_KEY_VM   0x912

#define SMG_MESG_KEY_SOURCE 0x916
#define SMG_MESG_KEY_UP     0x917
#define SMG_MESG_KEY_DOWN   0x918
#define SMG_MESG_KEY_ENTER  0x919


#define SMG_MESG_KEY_MUTE  0x914
#define SMG_MESG_KEY_POWER 0x915


//Legrand Key codes
// Cursor High
#define LGD_MESG_KEY_1P_1  0x000
#define LGD_MESG_KEY_1P_2  0x002
#define LGD_MESG_KEY_1P_3  0x004
#define LGD_MESG_KEY_1P_4  0x006
#define LGD_MESG_KEY_1P_5  0x008
#define LGD_MESG_KEY_1P_6  0x010

#define LGD_MESG_KEY_1M_1  0x001
#define LGD_MESG_KEY_1M_2  0x003
#define LGD_MESG_KEY_1M_3  0x005
#define LGD_MESG_KEY_1M_4  0x007
#define LGD_MESG_KEY_1M_5  0x009
#define LGD_MESG_KEY_1M_6  0x011

// Cursor Low
#define LGD_MESG_KEY_2P_1  0x012
#define LGD_MESG_KEY_2P_2  0x014
#define LGD_MESG_KEY_2P_3  0x016
#define LGD_MESG_KEY_2P_4  0x018
#define LGD_MESG_KEY_2P_5  0x020
#define LGD_MESG_KEY_2P_6  0x022

#define LGD_MESG_KEY_2M_1  0x013
#define LGD_MESG_KEY_2M_2  0x015
#define LGD_MESG_KEY_2M_3  0x017
#define LGD_MESG_KEY_2M_4  0x019
#define LGD_MESG_KEY_2M_5  0x021
#define LGD_MESG_KEY_2M_6  0x023


#endif // _IR_BRIDGE_H_
