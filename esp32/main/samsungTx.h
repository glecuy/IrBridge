
#ifndef _SAMSUNG_TX_H_
#define _SAMSUNG_TX_H_

#define SAMSUNG_DATA_ITEM_NUM   34  /*!< SAMSUNG code item number */


/*
 * @brief Build NEC 32bit waveform.
 */
int samsungBuildItems(rmt_item32_t* item, int item_num, uint32_t data);


void InfraredSamsungTxExecute( uint32_t code );


#endif // _SAMSUNG_TX_H_
