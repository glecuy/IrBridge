

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


#endif // _IR_BRIDGE_H_
