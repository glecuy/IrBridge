/* Philips RC6 remote infrared RMT encoding


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
#include "rc6Tx.h"



#define RC6_HEADER_HIGH_US    2685           /*!< RC6 protocol header: positive 2.685ms */
#define RC6_HEADER_LOW_US      895           /*!< RC6 protocol header: negative 0.895ms*/
#define RC6_BIT_HIGH_US        444           /*!< RC6 protocol data bit 1: positive 0.444ms */
#define RC6_BIT_LOW_US         444           /*!< RC6 protocol data bit 1: negative 0.444ms */
#define RC6_BIT_TOGGLE_HIGH_US 889           /*!< RC6 protocol data bit 0: positive 0.889ms */
#define RC6_BIT_TOGGLE_LOW_US  889           /*!< RC6 protocol data bit 0: negative 0.889ms */

unsigned rc6_toggle;

/*
 * @brief Generate RC6 header value
 */
void rc6_fill_start_bit(rmt_item32_t* item)
{
    item->level0 = 1;
    item->duration0 = RC6_HEADER_HIGH_US / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = RC6_HEADER_LOW_US / 10 * RMT_TICK_10_US;
}

/*
 * @brief Generate RC6 data bit 1
 */
void rc6_fill_item_bit_one(rmt_item32_t* item)
{
    item->level0 = 1;
    item->duration0 = RC6_BIT_HIGH_US / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = RC6_BIT_LOW_US / 10 * RMT_TICK_10_US;
}

/*
 * @brief Generate RC6 data bit 0
 */
void rc6_fill_item_bit_zero(rmt_item32_t* item)
{
    item->level0 = 0;
    item->duration0 = RC6_BIT_HIGH_US / 10 * RMT_TICK_10_US;
    item->level1 = 1;
    item->duration1 = RC6_BIT_LOW_US / 10 * RMT_TICK_10_US;
}

void rc6_fill_item_toggle_bit(rmt_item32_t* item)
{
    printf( "rc6_toggle = %d\n", rc6_toggle );
    if ( rc6_toggle != 0 ){
        item->level0 = 1;
        item->level1 = 0;
        rc6_toggle = 0;
    }
    else {
        item->level0 = 0;
        item->level1 = 1;
        rc6_toggle = 1;
    }
    item->duration0 = RC6_BIT_TOGGLE_HIGH_US / 10 * RMT_TICK_10_US;
    item->duration1 = RC6_BIT_TOGGLE_LOW_US / 10 * RMT_TICK_10_US;
}


/*
 * @brief Build RC6 waveform.
 * http://www.snrelectronicsblog.com/8051/rc-6-protocol-and-interfacing-with-microcontroller/
 */
int rc6BuildItems(rmt_item32_t* item, int item_num, uint16_t addr, uint16_t cmd_data)
{
    int i;
    if(item_num < RC6_DATA_ITEM_NUM) {
        return -1;
    }
    rc6_fill_start_bit(item++);

    // Mode bits
    rc6_fill_item_bit_one(item++);
    rc6_fill_item_bit_zero(item++);
    rc6_fill_item_bit_zero(item++);
    rc6_fill_item_bit_zero(item++);

    // Togle bit
    rc6_fill_item_toggle_bit(item++);

    // Addr
    for(i = 0; i < 8; i++) {
        if(addr & 0x80) {
            rc6_fill_item_bit_one(item);
        } else {
            rc6_fill_item_bit_zero(item);
        }
        item++;
        addr <<= 1;
    }
    // Cmd
    for(i = 0; i < 8; i++) {
        if(cmd_data & 0x80) {
            rc6_fill_item_bit_one(item);
        } else {
            rc6_fill_item_bit_zero(item);
        }
        item++;
        cmd_data <<= 1;
    }
    return RC6_DATA_ITEM_NUM;
}




/*

http://irdb.tk/codes/
****************************************************/
void InfraredRc6TxExecute( uint16_t add, uint16_t cmd ){
    printf( "RMT TX DATA");

    size_t size = (sizeof(rmt_item32_t) * RC6_DATA_ITEM_NUM);
    //each item represent a cycle of waveform.
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    memset((void*) item, 0, size);

    rc6BuildItems( item, RC6_DATA_ITEM_NUM, add, cmd);

    //To send data according to the waveform items.
    rmt_write_items(RMT_TX_CHANNEL, item, RC6_DATA_ITEM_NUM, true);
    //Wait until sending is done.
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
    //before we free the data, make sure sending is already done.
    free(item);
}
