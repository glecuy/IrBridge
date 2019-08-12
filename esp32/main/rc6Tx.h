
#ifndef _RC6_TX_H_
#define _RC6_TX_H_

#define RC6_DATA_ITEM_NUM   22  /*!< RC6 code item number: header + 20bit + start */


/*
 * @brief Build NEC 32bit waveform.
 */
int rc6BuildItems(rmt_item32_t* item, int item_num, uint16_t addr, uint16_t cmd_data);


void InfraredRc6TxExecute( uint16_t add, uint16_t cmd );


#endif // _RC6_TX_H_
