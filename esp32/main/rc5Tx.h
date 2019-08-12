
#ifndef _RC5_TX_H_
#define _RC5_TX_H_

#define RC5_DATA_ITEM_NUM   14  /*!< RC5 code item number: header + 20bit + start */


/*
 * @brief Build NEC 32bit waveform.
 */
int rc5BuildItems(rmt_item32_t* item, int item_num, uint16_t addr, uint16_t cmd_data);


void InfraredRc5TxExecute( uint16_t add, uint16_t cmd );


#endif // _RC5_TX_H_
