
#ifndef _NEC_TX_H_
#define _NEC_TX_H_

#define NEC_DATA_ITEM_NUM   34  /*!< NEC code item number: header + 32bit data + end */



/*
 * @brief Build NEC 32bit waveform.
 */
int necBuildItems(rmt_item32_t* item, int item_num, uint16_t addr, uint16_t cmd_data);


void InfraredNecTxExecute( uint16_t add, uint16_t cmd );


#endif // _NEC_TX_H_
