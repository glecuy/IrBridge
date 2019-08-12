
#ifndef _LEGRAND_TX_H_
#define _LEGRAND_TX_H_

#define LEGRAND_DATA_ITEM_NUM   20  /* */


/*
 * @brief Build LEGRAND  waveform from raw data.
 */
int legrandBuildItems(rmt_item32_t* item, int item_num, short bits );


void InfraredLegrandTxExecute( short cursor, short bits);


#endif // _LEGRAND_TX_H_
