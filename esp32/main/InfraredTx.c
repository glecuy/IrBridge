/* NEC remote infrared RMT example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
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
#include "necTx.h"
#include "legrandTx.h"
#include "samsungTx.h"
#include "IrBridge.h"

#define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */



/*
 * @brief RMT transmitter initialization
 */
static void rmt_tx_init(unsigned freq)
{
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_duty_percent = 40;
    rmt_tx.tx_config.carrier_freq_hz = freq;
    rmt_tx.tx_config.carrier_level = 1;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = 0;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = 0;
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);
}


void InfraredTxInit( void ){
    printf( "Configuring transmitter");
    //rmt_tx_init(36000); // RC5 Legrand
    rmt_tx_init(38000);   // Samsung

#if 0
    int cmd=0;
    while( 1 ){

        InfraredLegrandTxExecute( 0 );
        cmd++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#endif

}

void InfraredNecCodeSend( uint32_t code ){
    InfraredNecTxExecute( 0, (uint16_t)code );
}


void InfraredLegrandCodeSend( uint32_t code ){
    short index;

     switch( code ){
        case LGD_MESG_KEY_1P_1: index=0; break;
        case LGD_MESG_KEY_1P_2: index=1; break;
        case LGD_MESG_KEY_1P_3: index=2; break;
        case LGD_MESG_KEY_1P_4: index=3; break;


        default: index=0; break;
     }

     InfraredLegrandTxExecute( index );
}

/*
  http://irdb.tk/codes/

*****/
void InfraredSamsungCodeSend( uint32_t code ){
    // Convert code to Samsung
     uint16_t samsungcode;
     switch( code ){
        case SMG_MESG_KEY_0: samsungcode=17; break;
        case SMG_MESG_KEY_1: samsungcode=4; break;
        case SMG_MESG_KEY_2: samsungcode=5; break;
        case SMG_MESG_KEY_3: samsungcode=6; break;
        case SMG_MESG_KEY_4: samsungcode=8; break;
        case SMG_MESG_KEY_5: samsungcode=9; break;
        case SMG_MESG_KEY_6: samsungcode=10; break;
        case SMG_MESG_KEY_7: samsungcode=12; break;
        case SMG_MESG_KEY_8: samsungcode=13; break;
        case SMG_MESG_KEY_9: samsungcode=14; break;

        case SMG_MESG_KEY_CHP: samsungcode=18; break; // Channel +
        case SMG_MESG_KEY_CHM: samsungcode=16; break; // Channel -

        case SMG_MESG_KEY_VP: samsungcode=7; break;   // Volume +
        case SMG_MESG_KEY_VM: samsungcode=11; break;  // Volume -

        case SMG_MESG_KEY_MUTE:  samsungcode=15; break;
        case SMG_MESG_KEY_POWER: samsungcode=2; break;

        case SMG_MESG_KEY_SOURCE: samsungcode=1; break;
        case SMG_MESG_KEY_UP:     samsungcode=96; break;
        case SMG_MESG_KEY_DOWN:   samsungcode=97; break;
        case SMG_MESG_KEY_ENTER:  samsungcode=104; break;


        default:
            samsungcode=0;
            break;

     }
    InfraredSamsungTxExecute( samsungcode );
}

