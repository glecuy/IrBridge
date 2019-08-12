/* SAMSUNG remote infrared RMT encoding


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
#include "samsungTx.h"

/* http://read.pudn.com/downloads157/sourcecode/embed/701593/docs/IR%20Formats%202.PDF
 *
The Samsung code consists of a start bit followed by a 12 Bit
manufacturers code and an 8 bit command code. The message
is always sent a minimum of twice.
A digital zero is represented by a ‘1‘ of 0.56 ms followed by
a ‘0‘ of 0.56 ms. A digital one is represented by a ‘1‘ of 0.56 ms
followed by a ‘0‘ of 1.69 ms. A continuous key press will cause
the message to be repeated every 60 mS. The carrier frequency used for this code is 38 kHz.
*
***********************************************************************************************/

#define SAMSUNG_HEADER_HIGH_US    4500
#define SAMSUNG_HEADER_LOW_US     4500
#define SAMSUNG_BIT_HIGH_US        560
#define SAMSUNG_BIT_ONE_LOW_US    1690
#define SAMSUNG_BIT_ZERO_LOW_US    560


// https://www.techdesign.be/projects/011/011_waves.htm

/*
 * @brief Generate SAMSUNG header value
 */
void samsung_fill_start_bit(rmt_item32_t* item)
{
    item->level0 = 1;
    item->duration0 = SAMSUNG_HEADER_HIGH_US / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = SAMSUNG_HEADER_LOW_US / 10 * RMT_TICK_10_US;
}

/*
 * @brief Generate SAMSUNG data bit 1
 */
void samsung_fill_item_bit_one(rmt_item32_t* item)
{
    item->level0 = 1;
    item->duration0 = SAMSUNG_BIT_HIGH_US / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = SAMSUNG_BIT_ONE_LOW_US / 10 * RMT_TICK_10_US;
}

/*
 * @brief Generate SAMSUNG data bit 0
 */
void samsung_fill_item_bit_zero(rmt_item32_t* item)
{
    item->level0 = 1;
    item->duration0 = SAMSUNG_BIT_HIGH_US / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = SAMSUNG_BIT_ZERO_LOW_US / 10 * RMT_TICK_10_US;
}

void samsung_fill_item_stop_bit(rmt_item32_t* item, int n)
{
    item->level0 = 1;
    item->duration0 = SAMSUNG_BIT_HIGH_US / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = 10000  / 10 * RMT_TICK_10_US;
}


/*
 * @brief Build SAMSUNG waveform.
 *
 */
int samsungBuildItems(rmt_item32_t* item, int item_num, uint32_t data)
{
    int i;
    int n=0;
    if(item_num < SAMSUNG_DATA_ITEM_NUM) {
        return -1;
    }
    samsung_fill_start_bit(item++);

    // data
    for(i = 0; i < 32; i++) {
        if(data & 0x1) {
            samsung_fill_item_bit_one(item);
        } else {
            samsung_fill_item_bit_zero(item);
            n++;
        }
        item++;
        data >>= 1;
    }

    samsung_fill_item_stop_bit(item, n);

    return SAMSUNG_DATA_ITEM_NUM;
}




/*
https://rusticengineering.files.wordpress.com/2011/02/sheet.png
http://www.remotecentral.com/cgi-bin/codes/samsung/tv_functions/
http://irdb.tk  <<<
****************************************************/
void InfraredSamsungTxExecute( uint32_t code ){
    printf( "SamsungTxExecute %u\n", code);

    size_t size = (sizeof(rmt_item32_t) * SAMSUNG_DATA_ITEM_NUM);
    //each item represent a cycle of waveform.
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    memset((void*) item, 0, size);

    uint32_t bits = 0x0707;   // Device + Subdevice
    uint32_t code16 = (~code & 0xFF)<<8 | (code & 0xFF);
    bits |= code16<<16;

    printf( "Samsung bits = %08X\n", bits);
    samsungBuildItems( item, SAMSUNG_DATA_ITEM_NUM, bits);

    //To send data according to the waveform items.
    rmt_write_items(RMT_TX_CHANNEL, item, SAMSUNG_DATA_ITEM_NUM, true);
    //Wait until sending is done.
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
    //before we free the data, make sure sending is already done.
    free(item);
}

