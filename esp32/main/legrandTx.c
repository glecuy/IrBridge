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


#define mk_pulse( t0, t1)  pulse( item++, t0 / 10 * RMT_TICK_10_US, t1 / 10 * RMT_TICK_10_US)

static void pulse( rmt_item32_t* pItem, short t0, short t1){
	pItem->level0 = 1;
    pItem->duration0 = t0;
    pItem->level1 = 0;
    pItem->duration1 = t1;
} 

#define mk_zeros( t0, t1)  zeros( item++, t0 / 10 * RMT_TICK_10_US, t1 / 10 * RMT_TICK_10_US)

static void zeros( rmt_item32_t* pItem, short t0, short t1){
	pItem->level0 = 0;
    pItem->duration0 = t0;
    pItem->level1 = 0;
    pItem->duration1 = t1;
} 


int legrand_fill_header(rmt_item32_t* pItem, short cursor, short bits )
{
    rmt_item32_t* item = pItem;
    
    mk_pulse(860, 418);
    mk_pulse(418, 418);
    mk_pulse(418, 418);
    mk_pulse(418, 1280);

    switch ( bits ){  // Bit B1
		case 0:
			mk_pulse(860, 860);
			mk_pulse(1660, 1280);
			break;
		case 1:
			mk_pulse(418, 418);
			mk_pulse(418, 418);
			mk_pulse(1660, 1280);
			break;
		case 2:
			mk_pulse(1280, 418);
			mk_pulse(1660, 1280);
			break;	
		case 3:
			mk_pulse(418, 1280);
			mk_pulse(1660, 1280);
			break;			
		default:
			mk_pulse(418, 850);
			break;
    }

    mk_pulse(418, 850);
    mk_pulse(418, 1660);
    mk_pulse(418, 850);
    mk_pulse(418, 4200);

    // Key Code is there !
    switch ( bits ){ // Bit B2
        case 0:
            mk_pulse(418, 418);
            mk_pulse(2100, 850);
            break;
        case 1:
            mk_pulse(418, 850);
            mk_pulse(1660, 850);
            break;
        case 2:
            mk_pulse(418, 418);
            mk_pulse(418, 418);
            mk_pulse(1280, 850);
            break;
        case 3:
            mk_pulse(418, 1280);
            mk_pulse(1280, 850);
            break;
        default:
            mk_pulse(418, 850);
            break;
    }
    if ( cursor == 0 ){
		mk_zeros(418, 418);
	}
	else {
		mk_pulse(418, 418);
	}
    mk_pulse(418, 1260);
    mk_pulse(2100, 850);
    switch ( bits ){  // Bit B3
        case 0:
            mk_pulse(850, 418);
            mk_pulse(418, 418);
            mk_pulse(850, 418);
            mk_pulse(418, 850);
            break;
        case 1:
            mk_pulse(418, 418);
            mk_pulse(418, 418);
            mk_pulse(418, 418);
            mk_pulse(850, 850);
            break;
        case 2:
            mk_pulse(850, 418);
            mk_pulse(850, 850);
            break;
        case 3:
            mk_pulse(418, 418);
            mk_pulse(418, 850);
            mk_pulse(418, 418);
            mk_pulse(850, 850);
            break;
        default:;
            mk_pulse(418, 418);
            break;
    }

    return (item - pItem);
}

/*
 * @brief Build LEGRAND  waveform from raw data.
 */
int legrandBuildItems(rmt_item32_t* item, int item_num, short bits )
{
    int nbItem;
    if(item_num < LEGRAND_DATA_ITEM_NUM) {
        return -1;
    }

	printf( "bits :%d\n", bits );
	
    nbItem = legrand_fill_header(item, bits, 0 );
    item += nbItem;

    printf( "nbItem = %d\n", nbItem);


    return LEGRAND_DATA_ITEM_NUM;
}



void InfraredLegrandTxExecute( short cursor, short bits ){

    size_t size = (sizeof(rmt_item32_t) * LEGRAND_DATA_ITEM_NUM);
    //each item represent a cycle of waveform.
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    memset((void*) item, 0, size);

    size = (size_t)legrandBuildItems( item, LEGRAND_DATA_ITEM_NUM, bits);

	if ( size > LEGRAND_DATA_ITEM_NUM ){
		printf("Sequence too long ! (%d)", size);
	}

    //To send data according to the waveform items.
    rmt_write_items(RMT_TX_CHANNEL, item, size, true);
    //Wait until sending is done.
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
    //before we free the data, make sure sending is already done.
    free(item);
}

