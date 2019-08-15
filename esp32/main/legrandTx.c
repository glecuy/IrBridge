/* LEGRAND Remote TX (raw data)
 *
 * "in One By Legrand"
 *
 * Telecommande 24 directions
 * 882 20


*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"

#include "InfraredTx.h"
#include "legrandTx.h"


#define MAX_TX_ITEM 30

typedef struct rawTxItem_s{
	uint8_t h;
	uint8_t l;
} rawTxItem_t;


typedef struct rawTxItemSeq_s{
	int length;
	rawTxItem_t rawTxItems[MAX_TX_ITEM];
}rawTxItemSeq_t;


/* Sample rate has been converted to 20 uS */
static const rawTxItemSeq_t TxItemSequences[] = {
// Bouton '000'
    {
        .length = 18,
        .rawTxItems = {
            { 44, 21}, { 21, 21}, { 22, 21}, { 21, 63}, { 43, 42}, { 84, 63}, { 22, 42}, { 21, 83}, 
            { 22, 42}, { 21,210}, { 22, 21}, {105, 82}, { 22, 64}, {105, 42}, { 43, 21}, { 21, 21}, 
            { 42, 22}, { 21,100}, 
        },
    },
    {
        .length = 17,
        .rawTxItems = {
            { 43, 21}, { 22, 21}, { 21, 21}, { 21, 64}, { 21, 63}, { 84, 64}, { 21, 42}, { 21, 83}, 
            { 22, 43}, { 21,209}, { 22, 21}, {105, 82}, { 23, 21}, { 42, 22}, { 83, 43}, { 83, 43}, 
            { 42,100}, 
        },
    },
// Bouton '001'
	{
        .length = 16,
        .rawTxItems = {
            { 44, 21}, { 21, 21}, { 21, 21}, { 22, 63}, { 43, 42}, { 84, 63}, { 21, 43}, { 21, 83}, 
            { 22, 42}, { 21,209}, { 43, 22}, { 83, 83}, { 22, 64}, {105, 42}, { 21,105}, { 64,100}, 
        },
    },
    {
        .length = 18,
        .rawTxItems = {
            { 44, 21}, { 22, 21}, { 21, 21}, { 21, 64}, { 21, 63}, { 84, 64}, { 21, 42}, { 21, 84}, 
            { 21, 43}, { 21,208}, { 44, 21}, { 84, 82}, { 23, 21}, { 42, 21}, { 84, 43}, { 21, 21}, 
            { 21, 43}, { 42,100}, 
        },
    },
};


#define mk_pulse( t0, t1)  pulse( item++, t0 / 10 * RMT_TICK_10_US, t1 / 10 * RMT_TICK_10_US)


/*
 * @brief Build LEGRAND  waveform from raw data.
 */
int legrandBuildItems(rmt_item32_t ** pItem, short set )
{
    int nbItem;

	nbItem = TxItemSequences[set].length;
	
	printf( "Allocating %d items\n", nbItem);
	
	size_t size = (sizeof(rmt_item32_t) * nbItem );
    //each item represent a cycle of waveform.
	rmt_item32_t * item = (rmt_item32_t*) malloc(size);
    memset((void*) item, 0, size);    
	printf( "item =  %p\n", item);
    *pItem = item;
	

	for( int i = 0; i < nbItem; i++) {
		item->level0 = 1;
		item->duration0 = (TxItemSequences[set].rawTxItems[i].h * 20 ) / 10 * RMT_TICK_10_US;
		item->level1 = 0;
		item->duration1 = (TxItemSequences[set].rawTxItems[i].l * 20 ) / 10 * RMT_TICK_10_US;
		item++;
	}
	
	//printf( "duration = %u\n", (unsigned)(10000 / 10 * RMT_TICK_10_US) );
		
    return nbItem;
}



void InfraredLegrandTxExecute( short index ){
	rmt_item32_t * item = NULL;
    int nbItem;

    nbItem = legrandBuildItems( &item, index*2 );        
    //To send data according to the waveform items.
    rmt_write_items(RMT_TX_CHANNEL, item, nbItem, true);
    //Wait until sending is done.
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
    //before we free the data, make sure sending is already done.
    free(item);
    
     vTaskDelay( 158 / portTICK_PERIOD_MS );
    

    nbItem = legrandBuildItems( &item, index*2+1 );        
    //To send data according to the waveform items.
    rmt_write_items(RMT_TX_CHANNEL, item, nbItem, true);
    //Wait until sending is done.
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
    //before we free the data, make sure sending is already done.
    free(item);
    
}

