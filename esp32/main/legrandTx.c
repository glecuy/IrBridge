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


#define MAX_TX_ITEM 20

typedef struct rawTxItem_s{
	uint8_t h;
	uint8_t l;
} rawTxItem_t;


typedef struct rawTxItemSeq_s{
	rawTxItem_t rawTxItems[MAX_TX_ITEM];
	uint16_t length;
	uint16_t delay;
}rawTxItemSeq_t;

/*
 * 
 *
 * The table structure represents the RMT item structure:
 * {duration, level, duration, level}
 *
 */
rmt_item32_t txIitems01[] = {
        {{{   710, 1,   320, 0 }}}, // 0
        {{{   355, 1,   313, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   358, 1,   988, 0 }}}, // 6
        {{{   694, 1,   649, 0 }}}, // 8
        {{{  1369, 1,   985, 0 }}}, // 10
        {{{   355, 1,   652, 0 }}}, // 12
        {{{   358, 1,  1308, 0 }}}, // 14
        {{{   371, 1,   652, 0 }}}, // 16
        {{{   358, 1,  3321, 0 }}}, // 18
        {{{   374, 1,   316, 0 }}}, // 20
        {{{  1702, 1,  1305, 0 }}}, // 22
        {{{   377, 1,   985, 0 }}}, // 24
        {{{  1702, 1,   649, 0 }}}, // 26
        {{{   697, 1,   313, 0 }}}, // 28
        {{{   358, 1,   313, 0 }}}, // 30
        {{{   694, 1,   316, 0 }}}, // 32
        {{{   358, 1, 32767, 0 }}}, // 34
        {{{ 23339, 0, 23339, 0 }}},
        {{{ 23339, 0, 23339, 0 }}},
        {{{   716, 1,   313, 0 }}}, // 36
        {{{   361, 1,   313, 0 }}}, // 38
        {{{   358, 1,   313, 0 }}}, // 40
        {{{   355, 1,   985, 0 }}}, // 42
        {{{   361, 1,   985, 0 }}}, // 44
        {{{  1369, 1,   985, 0 }}}, // 46
        {{{   358, 1,   649, 0 }}}, // 48
        {{{   361, 1,  1302, 0 }}}, // 50
        {{{   374, 1,   649, 0 }}}, // 52
        {{{   358, 1,  3324, 0 }}}, // 54
        {{{   377, 1,   313, 0 }}}, // 56
        {{{  1702, 1,  1321, 0 }}}, // 58
        {{{   358, 1,   316, 0 }}}, // 60
        {{{   694, 1,   313, 0 }}}, // 62
        {{{  1369, 1,   649, 0 }}}, // 64
        {{{  1363, 1,   652, 0 }}}, // 66
        {{{   694, 1,  1000, 0 }}}, // 68
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};

rmt_item32_t txIitems02[] = {
        {{{   716, 1,   316, 0 }}}, // 0
        {{{   355, 1,   316, 0 }}}, // 2
        {{{   355, 1,   316, 0 }}}, // 4
        {{{   355, 1,   988, 0 }}}, // 6
        {{{   358, 1,   985, 0 }}}, // 8
        {{{  1369, 1,   985, 0 }}}, // 10
        {{{   355, 1,   656, 0 }}}, // 12
        {{{   355, 1,  1305, 0 }}}, // 14
        {{{   374, 1,   652, 0 }}}, // 16
        {{{   358, 1,  3324, 0 }}}, // 18
        {{{   371, 1,   656, 0 }}}, // 20
        {{{  1363, 1,  1305, 0 }}}, // 22
        {{{   377, 1,   985, 0 }}}, // 24
        {{{  1702, 1,   652, 0 }}}, // 26
        {{{   691, 1,   652, 0 }}}, // 28
        {{{   358, 1,   313, 0 }}}, // 30
        {{{  1030, 1, 32767, 0 }}}, // 32
        {{{ 23341, 0, 23341, 0 }}},
        {{{ 23341, 0, 23341, 0 }}},
        {{{   713, 1,   313, 0 }}}, // 34
        {{{   358, 1,   316, 0 }}}, // 36
        {{{   358, 1,   313, 0 }}}, // 38
        {{{   358, 1,   985, 0 }}}, // 40
        {{{  1033, 1,   313, 0 }}}, // 42
        {{{  1366, 1,   988, 0 }}}, // 44
        {{{   355, 1,   652, 0 }}}, // 46
        {{{   355, 1,  1324, 0 }}}, // 48
        {{{   358, 1,   649, 0 }}}, // 50
        {{{   358, 1,  3324, 0 }}}, // 52
        {{{   377, 1,   649, 0 }}}, // 54
        {{{  1363, 1,  1324, 0 }}}, // 56
        {{{   358, 1,   313, 0 }}}, // 58
        {{{   694, 1,   316, 0 }}}, // 60
        {{{  1366, 1,   649, 0 }}}, // 62
        {{{   358, 1,   988, 0 }}}, // 64
        {{{  1027, 1,   316, 0 }}}, // 66
        {{{   355, 1,  1000, 0 }}}, // 68
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};

rmt_item32_t txIitems03[] = {
        {{{   713, 1,   313, 0 }}}, // 0
        {{{   358, 1,   316, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   358, 1,   985, 0 }}}, // 6
        {{{  1030, 1,   316, 0 }}}, // 8
        {{{  1363, 1,   988, 0 }}}, // 10
        {{{   358, 1,   649, 0 }}}, // 12
        {{{   358, 1,  1324, 0 }}}, // 14
        {{{   358, 1,   649, 0 }}}, // 16
        {{{   355, 1,  3344, 0 }}}, // 18
        {{{   358, 1,   313, 0 }}}, // 20
        {{{   358, 1,   313, 0 }}}, // 22
        {{{  1033, 1,  1305, 0 }}}, // 24
        {{{   371, 1,   992, 0 }}}, // 26
        {{{  1699, 1,   652, 0 }}}, // 28
        {{{   694, 1,   313, 0 }}}, // 30
        {{{   694, 1, 32767, 0 }}}, // 32
        {{{ 23679, 0, 23679, 0 }}},
        {{{ 23679, 0, 23679, 0 }}},
        {{{   707, 1,   320, 0 }}}, // 34
        {{{   355, 1,   316, 0 }}}, // 36
        {{{   355, 1,   313, 0 }}}, // 38
        {{{   358, 1,   988, 0 }}}, // 40
        {{{   336, 1,   336, 0 }}}, // 42
        {{{   358, 1,   313, 0 }}}, // 44
        {{{  1369, 1,   985, 0 }}}, // 46
        {{{   358, 1,   652, 0 }}}, // 48
        {{{   355, 1,  1321, 0 }}}, // 50
        {{{   358, 1,   652, 0 }}}, // 52
        {{{   358, 1,  3340, 0 }}}, // 54
        {{{   358, 1,   313, 0 }}}, // 56
        {{{   336, 1,   339, 0 }}}, // 58
        {{{  1027, 1,  1321, 0 }}}, // 60
        {{{   342, 1,   332, 0 }}}, // 62
        {{{   694, 1,   313, 0 }}}, // 64
        {{{  1366, 1,   652, 0 }}}, // 66
        {{{  2041, 1,   313, 0 }}}, // 68
        {{{   691, 1,  1000, 0 }}}, // 70
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};





rmt_item32_t txIitems04[] = {
        {{{   713, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   361, 1,   313, 0 }}}, // 4
        {{{   358, 1,   985, 0 }}}, // 6
        {{{  1030, 1,   313, 0 }}}, // 8
        {{{  1366, 1,   988, 0 }}}, // 10
        {{{   358, 1,   649, 0 }}}, // 12
        {{{   358, 1,  1305, 0 }}}, // 14
        {{{   374, 1,   649, 0 }}}, // 16
        {{{   358, 1,  3324, 0 }}}, // 18
        {{{   377, 1,   985, 0 }}}, // 20
        {{{  1030, 1,  1308, 0 }}}, // 22
        {{{   371, 1,   988, 0 }}}, // 24
        {{{  1705, 1,   649, 0 }}}, // 26
        {{{   355, 1,   652, 0 }}}, // 28
        {{{  1030, 1,   649, 0 }}}, // 30
        {{{   358, 1, 32767, 0 }}}, // 32
        {{{ 23345, 0, 23345, 0 }}},
        {{{ 23345, 0, 23345, 0 }}},
        {{{   713, 1,   313, 0 }}}, // 34
        {{{   358, 1,   313, 0 }}}, // 36
        {{{   358, 1,   313, 0 }}}, // 38
        {{{   358, 1,   988, 0 }}}, // 40
        {{{   358, 1,   313, 0 }}}, // 42
        {{{   355, 1,   320, 0 }}}, // 44
        {{{  1363, 1,   985, 0 }}}, // 46
        {{{   358, 1,   652, 0 }}}, // 48
        {{{   358, 1,  1305, 0 }}}, // 50
        {{{   374, 1,   652, 0 }}}, // 52
        {{{   358, 1,  3337, 0 }}}, // 54
        {{{   358, 1,   988, 0 }}}, // 56
        {{{  1030, 1,  1305, 0 }}}, // 58
        {{{   377, 1,   313, 0 }}}, // 60
        {{{   694, 1,   313, 0 }}}, // 62
        {{{  1366, 1,   649, 0 }}}, // 64
        {{{   358, 1,   313, 0 }}}, // 66
        {{{  1033, 1,   649, 0 }}}, // 68
        {{{   358, 1,  1000, 0 }}}, // 70
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};

rmt_item32_t txIitems05[] = {
        {{{   694, 1,   336, 0 }}}, // 0
        {{{   336, 1,   339, 0 }}}, // 2
        {{{   336, 1,   336, 0 }}}, // 4
        {{{   336, 1,  1004, 0 }}}, // 6
        {{{  1014, 1,   332, 0 }}}, // 8
        {{{  1347, 1,  1008, 0 }}}, // 10
        {{{   336, 1,   675, 0 }}}, // 12
        {{{   336, 1,  1324, 0 }}}, // 14
        {{{   358, 1,   668, 0 }}}, // 16
        {{{   336, 1,  3344, 0 }}}, // 18
        {{{   358, 1,   332, 0 }}}, // 20
        {{{   675, 1,   332, 0 }}}, // 22
        {{{   675, 1,  1328, 0 }}}, // 24
        {{{   355, 1,  1004, 0 }}}, // 26
        {{{  1686, 1,   668, 0 }}}, // 28
        {{{   336, 1,  2003, 0 }}}, // 30
        {{{   355, 1, 32767, 0 }}}, // 32
        {{{ 23433, 0, 23433, 0 }}},
        {{{ 23433, 0, 23433, 0 }}},
        {{{   694, 1,   336, 0 }}}, // 34
        {{{   336, 1,   336, 0 }}}, // 36
        {{{   339, 1,   336, 0 }}}, // 38
        {{{   336, 1,  1008, 0 }}}, // 40
        {{{   339, 1,   332, 0 }}}, // 42
        {{{   339, 1,   332, 0 }}}, // 44
        {{{  1350, 1,  1004, 0 }}}, // 46
        {{{   336, 1,   672, 0 }}}, // 48
        {{{   339, 1,  1324, 0 }}}, // 50
        {{{   355, 1,   672, 0 }}}, // 52
        {{{   339, 1,  3344, 0 }}}, // 54
        {{{   355, 1,   336, 0 }}}, // 56
        {{{   672, 1,   332, 0 }}}, // 58
        {{{   675, 1,  1324, 0 }}}, // 60
        {{{   355, 1,   336, 0 }}}, // 62
        {{{   675, 1,   336, 0 }}}, // 64
        {{{  1344, 1,   672, 0 }}}, // 66
        {{{   339, 1,   332, 0 }}}, // 68
        {{{   339, 1,   668, 0 }}}, // 70
        {{{   342, 1,   668, 0 }}}, // 72
        {{{   336, 1,  1000, 0 }}}, // 74
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};
	
rmt_item32_t txIitems06[] = {	
        {{{   694, 1,   336, 0 }}}, // 0
        {{{   339, 1,   332, 0 }}}, // 2
        {{{   339, 1,   332, 0 }}}, // 4
        {{{   339, 1,  1008, 0 }}}, // 6
        {{{   336, 1,  1008, 0 }}}, // 8
        {{{  1347, 1,  1004, 0 }}}, // 10
        {{{   342, 1,   668, 0 }}}, // 12
        {{{   339, 1,  1324, 0 }}}, // 14
        {{{   355, 1,   672, 0 }}}, // 16
        {{{   336, 1,  3344, 0 }}}, // 18
        {{{   355, 1,   672, 0 }}}, // 20
        {{{   339, 1,   336, 0 }}}, // 22
        {{{   672, 1,  1324, 0 }}}, // 24
        {{{   358, 1,  1004, 0 }}}, // 26
        {{{  1683, 1,   668, 0 }}}, // 28
        {{{  2019, 1,   675, 0 }}}, // 30
        {{{   336, 1, 32767, 0 }}}, // 32
        {{{ 23349, 0, 23349, 0 }}},
        {{{ 23349, 0, 23349, 0 }}},
        {{{   697, 1,   336, 0 }}}, // 34
        {{{   336, 1,   336, 0 }}}, // 36
        {{{   336, 1,   332, 0 }}}, // 38
        {{{   342, 1,  1004, 0 }}}, // 40
        {{{  1011, 1,   336, 0 }}}, // 42
        {{{  1344, 1,  1011, 0 }}}, // 44
        {{{   336, 1,   668, 0 }}}, // 46
        {{{   339, 1,  1344, 0 }}}, // 48
        {{{   336, 1,   672, 0 }}}, // 50
        {{{   339, 1,  3340, 0 }}}, // 52
        {{{   358, 1,   672, 0 }}}, // 54
        {{{   339, 1,   332, 0 }}}, // 56
        {{{   672, 1,  1328, 0 }}}, // 58
        {{{   355, 1,   336, 0 }}}, // 60
        {{{   672, 1,   332, 0 }}}, // 62
        {{{  1350, 1,   668, 0 }}}, // 64
        {{{   339, 1,   336, 0 }}}, // 66
        {{{  1008, 1,   675, 0 }}}, // 68
        {{{   672, 1,  1000, 0 }}}, // 70
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};


rmt_item32_t txIitems11[] = {
        {{{   716, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   355, 1,   320, 0 }}}, // 4
        {{{   355, 1,   985, 0 }}}, // 6
        {{{   697, 1,   649, 0 }}}, // 8
        {{{  1369, 1,   985, 0 }}}, // 10
        {{{   355, 1,   652, 0 }}}, // 12
        {{{   358, 1,  1305, 0 }}}, // 14
        {{{   377, 1,   649, 0 }}}, // 16
        {{{   355, 1,  3324, 0 }}}, // 18
        {{{   713, 1,   316, 0 }}}, // 20
        {{{  1344, 1,  1344, 0 }}}, // 22
        {{{   358, 1,   985, 0 }}}, // 24
        {{{  1683, 1,   672, 0 }}}, // 26
        {{{   358, 1,  1641, 0 }}}, // 28
        {{{  1027, 1, 32767, 0 }}}, // 30
        {{{ 23349, 0, 23349, 0 }}},
        {{{ 23349, 0, 23349, 0 }}},
        {{{   716, 1,   316, 0 }}}, // 32
        {{{   358, 1,   313, 0 }}}, // 34
        {{{   355, 1,   316, 0 }}}, // 36
        {{{   355, 1,   988, 0 }}}, // 38
        {{{   336, 1,  1011, 0 }}}, // 40
        {{{  1366, 1,   988, 0 }}}, // 42
        {{{   355, 1,   649, 0 }}}, // 44
        {{{   358, 1,  1324, 0 }}}, // 46
        {{{   336, 1,   672, 0 }}}, // 48
        {{{   361, 1,  3318, 0 }}}, // 50
        {{{   713, 1,   316, 0 }}}, // 52
        {{{  1366, 1,  1305, 0 }}}, // 54
        {{{   374, 1,   313, 0 }}}, // 56
        {{{   694, 1,   316, 0 }}}, // 58
        {{{  1366, 1,   652, 0 }}}, // 60
        {{{   355, 1,   316, 0 }}}, // 62
        {{{   355, 1,   649, 0 }}}, // 64
        {{{   697, 1,  1000, 0 }}}, // 66
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};


rmt_item32_t txIitems12[] = {
        {{{   710, 1,   316, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   339, 1,  1008, 0 }}}, // 6
        {{{   691, 1,   652, 0 }}}, // 8
        {{{  1366, 1,   988, 0 }}}, // 10
        {{{   358, 1,   649, 0 }}}, // 12
        {{{   336, 1,  1324, 0 }}}, // 14
        {{{   377, 1,   652, 0 }}}, // 16
        {{{   336, 1,  3344, 0 }}}, // 18
        {{{  1027, 1,   336, 0 }}}, // 20
        {{{  1030, 1,  1324, 0 }}}, // 22
        {{{   358, 1,   985, 0 }}}, // 24
        {{{  1702, 1,   652, 0 }}}, // 26
        {{{   358, 1,   313, 0 }}}, // 28
        {{{   336, 1,   339, 0 }}}, // 30
        {{{   336, 1,   668, 0 }}}, // 32
        {{{   358, 1, 32767, 0 }}}, // 34
        {{{ 23428, 0, 23428, 0 }}},
        {{{ 23428, 0, 23428, 0 }}},
        {{{   713, 1,   313, 0 }}}, // 36
        {{{   358, 1,   316, 0 }}}, // 38
        {{{   358, 1,   313, 0 }}}, // 40
        {{{   358, 1,   988, 0 }}}, // 42
        {{{   358, 1,   988, 0 }}}, // 44
        {{{  1363, 1,   988, 0 }}}, // 46
        {{{   358, 1,   649, 0 }}}, // 48
        {{{   358, 1,  1305, 0 }}}, // 50
        {{{   374, 1,   649, 0 }}}, // 52
        {{{   358, 1,  3328, 0 }}}, // 54
        {{{  1046, 1,   313, 0 }}}, // 56
        {{{  1030, 1,  1305, 0 }}}, // 58
        {{{   377, 1,   313, 0 }}}, // 60
        {{{   694, 1,   313, 0 }}}, // 62
        {{{  1366, 1,   652, 0 }}}, // 64
        {{{   358, 1,   988, 0 }}}, // 66
        {{{   694, 1,   649, 0 }}}, // 68
        {{{   355, 1,  1000, 0 }}}, // 70
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};


rmt_item32_t txIitems13[] = {
        {{{   720, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   355, 1,   320, 0 }}}, // 4
        {{{   355, 1,   985, 0 }}}, // 6
        {{{  1033, 1,   313, 0 }}}, // 8
        {{{  1363, 1,   988, 0 }}}, // 10
        {{{   358, 1,   652, 0 }}}, // 12
        {{{   358, 1,  1305, 0 }}}, // 14
        {{{   371, 1,   656, 0 }}}, // 16
        {{{   355, 1,  3324, 0 }}}, // 18
        {{{   710, 1,   649, 0 }}}, // 20
        {{{  1033, 1,  1305, 0 }}}, // 22
        {{{   377, 1,   985, 0 }}}, // 24
        {{{  1702, 1,   649, 0 }}}, // 26
        {{{   361, 1,   985, 0 }}}, // 28
        {{{   691, 1,   320, 0 }}}, // 30
        {{{   355, 1, 32767, 0 }}}, // 32
        {{{ 23430, 0, 23430, 0 }}},
        {{{ 23430, 0, 23430, 0 }}},
        {{{   713, 1,   313, 0 }}}, // 34
        {{{   358, 1,   313, 0 }}}, // 36
        {{{   361, 1,   313, 0 }}}, // 38
        {{{   336, 1,  1008, 0 }}}, // 40
        {{{   355, 1,   316, 0 }}}, // 42
        {{{   358, 1,   313, 0 }}}, // 44
        {{{  1366, 1,   988, 0 }}}, // 46
        {{{   358, 1,   652, 0 }}}, // 48
        {{{   358, 1,  1305, 0 }}}, // 50
        {{{   371, 1,   652, 0 }}}, // 52
        {{{   358, 1,  3324, 0 }}}, // 54
        {{{   710, 1,   649, 0 }}}, // 56
        {{{  1033, 1,  1305, 0 }}}, // 58
        {{{   374, 1,   316, 0 }}}, // 60
        {{{   691, 1,   313, 0 }}}, // 62
        {{{  1369, 1,   649, 0 }}}, // 64
        {{{   358, 1,   313, 0 }}}, // 66
        {{{   358, 1,   316, 0 }}}, // 68
        {{{   358, 1,   985, 0 }}}, // 70
        {{{   355, 1,  1000, 0 }}}, // 72
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};


rmt_item32_t txIitems14[] = {
        {{{   713, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   358, 1,   316, 0 }}}, // 4
        {{{   355, 1,   985, 0 }}}, // 6
        {{{   694, 1,   652, 0 }}}, // 8
        {{{  1369, 1,   985, 0 }}}, // 10
        {{{   355, 1,   652, 0 }}}, // 12
        {{{   358, 1,  1321, 0 }}}, // 14
        {{{   361, 1,   649, 0 }}}, // 16
        {{{   358, 1,  3321, 0 }}}, // 18
        {{{  1382, 1,   320, 0 }}}, // 20
        {{{   691, 1,  1308, 0 }}}, // 22
        {{{   352, 1,  1008, 0 }}}, // 24
        {{{  1702, 1,   652, 0 }}}, // 26
        {{{  1366, 1, 32767, 0 }}}, // 28
        {{{ 23764, 0, 23764, 0 }}},
        {{{ 23764, 0, 23764, 0 }}},
        {{{   713, 1,   316, 0 }}}, // 30
        {{{   355, 1,   316, 0 }}}, // 32
        {{{   358, 1,   313, 0 }}}, // 34
        {{{   358, 1,   988, 0 }}}, // 36
        {{{   336, 1,  1008, 0 }}}, // 38
        {{{  1366, 1,   988, 0 }}}, // 40
        {{{   336, 1,   672, 0 }}}, // 42
        {{{   358, 1,  1324, 0 }}}, // 44
        {{{   355, 1,   649, 0 }}}, // 46
        {{{   358, 1,  3324, 0 }}}, // 48
        {{{  1366, 1,   332, 0 }}}, // 50
        {{{   694, 1,  1324, 0 }}}, // 52
        {{{   336, 1,   336, 0 }}}, // 54
        {{{   697, 1,   313, 0 }}}, // 56
        {{{  1363, 1,   652, 0 }}}, // 58
        {{{   694, 1,   313, 0 }}}, // 60
        {{{   336, 1,   339, 0 }}}, // 62
        {{{   358, 1,   313, 0 }}}, // 64
        {{{   672, 1,  1000, 0 }}}, // 66
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};


rmt_item32_t txIitems15[] = {
        {{{   713, 1,   316, 0 }}}, // 0
        {{{   339, 1,   332, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   336, 1,  1011, 0 }}}, // 6
        {{{   694, 1,   652, 0 }}}, // 8
        {{{  1363, 1,   988, 0 }}}, // 10
        {{{   358, 1,   649, 0 }}}, // 12
        {{{   358, 1,  1324, 0 }}}, // 14
        {{{   355, 1,   652, 0 }}}, // 16
        {{{   358, 1,  3324, 0 }}}, // 18
        {{{   691, 1,   332, 0 }}}, // 20
        {{{   339, 1,   332, 0 }}}, // 22
        {{{   694, 1,  1305, 0 }}}, // 24
        {{{   355, 1,  1011, 0 }}}, // 26
        {{{  1683, 1,   668, 0 }}}, // 28
        {{{   336, 1,   336, 0 }}}, // 30
        {{{   697, 1,   313, 0 }}}, // 32
        {{{   358, 1,   649, 0 }}}, // 34
        {{{   336, 1, 32767, 0 }}}, // 36
        {{{ 23349, 0, 23349, 0 }}},
        {{{ 23349, 0, 23349, 0 }}},
        {{{   713, 1,   313, 0 }}}, // 38
        {{{   358, 1,   316, 0 }}}, // 40
        {{{   358, 1,   313, 0 }}}, // 42
        {{{   358, 1,   988, 0 }}}, // 44
        {{{   355, 1,   985, 0 }}}, // 46
        {{{  1369, 1,   985, 0 }}}, // 48
        {{{   358, 1,   652, 0 }}}, // 50
        {{{   355, 1,  1308, 0 }}}, // 52
        {{{   374, 1,   649, 0 }}}, // 54
        {{{   358, 1,  3324, 0 }}}, // 56
        {{{   710, 1,   313, 0 }}}, // 58
        {{{   358, 1,   313, 0 }}}, // 60
        {{{   697, 1,  1305, 0 }}}, // 62
        {{{   355, 1,   336, 0 }}}, // 64
        {{{   672, 1,   336, 0 }}}, // 66
        {{{  1366, 1,   649, 0 }}}, // 68
        {{{   358, 1,   652, 0 }}}, // 70
        {{{   358, 1,   985, 0 }}}, // 72
        {{{   336, 1,  1000, 0 }}}, // 74
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};


rmt_item32_t txIitems16[] = {
        {{{   713, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   358, 1,   316, 0 }}}, // 4
        {{{   358, 1,   985, 0 }}}, // 6
        {{{  1030, 1,   316, 0 }}}, // 8
        {{{  1363, 1,   988, 0 }}}, // 10
        {{{   358, 1,   652, 0 }}}, // 12
        {{{   358, 1,  1305, 0 }}}, // 14
        {{{   371, 1,   652, 0 }}}, // 16
        {{{   358, 1,  3324, 0 }}}, // 18
        {{{  1046, 1,   649, 0 }}}, // 20
        {{{   697, 1,  1305, 0 }}}, // 22
        {{{   374, 1,   988, 0 }}}, // 24
        {{{  1702, 1,   649, 0 }}}, // 26
        {{{  2377, 1,   313, 0 }}}, // 28
        {{{   358, 1, 32767, 0 }}}, // 30
        {{{ 23346, 0, 23346, 0 }}},
        {{{ 23346, 0, 23346, 0 }}},
        {{{   713, 1,   313, 0 }}}, // 32
        {{{   358, 1,   316, 0 }}}, // 34
        {{{   358, 1,   313, 0 }}}, // 36
        {{{   358, 1,   988, 0 }}}, // 38
        {{{   358, 1,   313, 0 }}}, // 40
        {{{   355, 1,   316, 0 }}}, // 42
        {{{  1366, 1,   985, 0 }}}, // 44
        {{{   358, 1,   652, 0 }}}, // 46
        {{{   358, 1,  1305, 0 }}}, // 48
        {{{   374, 1,   649, 0 }}}, // 50
        {{{   358, 1,  3321, 0 }}}, // 52
        {{{  1049, 1,   652, 0 }}}, // 54
        {{{   694, 1,  1305, 0 }}}, // 56
        {{{   377, 1,   313, 0 }}}, // 58
        {{{   672, 1,   332, 0 }}}, // 60
        {{{  1369, 1,   652, 0 }}}, // 62
        {{{   694, 1,   313, 0 }}}, // 64
        {{{   691, 1,   320, 0 }}}, // 66
        {{{   672, 1,  1000, 0 }}}, // 68
		// RMT end marker
		{{{ 0, 1, 0, 0 }}}
	};
	
	

/*
Curseur bas :
Type [C-a] [C-h] to see available commands
Terminal ready
HelloAVR

    {
        {{{   713, 1,   316, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   358, 1,   985, 0 }}}, // 6
        {{{   694, 1,   652, 0 }}}, // 8
        {{{  1366, 1,   988, 0 }}}, // 10
        {{{   355, 1,   652, 0 }}}, // 12
        {{{   358, 1,  1302, 0 }}}, // 14
        {{{   377, 1,   652, 0 }}}, // 16
        {{{   355, 1,  3344, 0 }}}, // 18
        {{{   358, 1,   313, 0 }}}, // 20
        {{{  1030, 1,   316, 0 }}}, // 22
        {{{   355, 1,  1324, 0 }}}, // 24
        {{{   358, 1,   985, 0 }}}, // 26
        {{{  1705, 1,   649, 0 }}}, // 28
        {{{   694, 1,   652, 0 }}}, // 30
        {{{   691, 1,   316, 0 }}}, // 32
        {{{   358, 1, 32767, 0 }}}, // 34
        {{{ 23429, 0, 23429, 0 }}},
        {{{ 23429, 0, 23429, 0 }}},
        {{{   710, 1,   320, 0 }}}, // 36
        {{{   358, 1,   313, 0 }}}, // 38
        {{{   358, 1,   313, 0 }}}, // 40
        {{{   358, 1,   988, 0 }}}, // 42
        {{{   355, 1,   988, 0 }}}, // 44
        {{{  1366, 1,   985, 0 }}}, // 46
        {{{   358, 1,   652, 0 }}}, // 48
        {{{   358, 1,  1302, 0 }}}, // 50
        {{{   377, 1,   649, 0 }}}, // 52
        {{{   358, 1,  3340, 0 }}}, // 54
        {{{   358, 1,   316, 0 }}}, // 56
        {{{  1030, 1,   313, 0 }}}, // 58
        {{{   358, 1,  1305, 0 }}}, // 60
        {{{   377, 1,   313, 0 }}}, // 62
        {{{   694, 1,   313, 0 }}}, // 64
        {{{  1366, 1,   652, 0 }}}, // 66
        {{{  1030, 1,   313, 0 }}}, // 68
        {{{   358, 1,   985, 0 }}}, // 70
        {{{   358, 1, 32767, 0 }}}, // 72
        {{{    66, 0,    66, 0 }}},
        {{{    66, 0,    66, 0 }}},
    {
Nb pulse = 37
Frame length = 176323
    {
        {{{   716, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   358, 1,   992, 0 }}}, // 6
        {{{   352, 1,   992, 0 }}}, // 8
        {{{  1363, 1,   988, 0 }}}, // 10
        {{{   358, 1,   649, 0 }}}, // 12
        {{{   358, 1,  1308, 0 }}}, // 14
        {{{   371, 1,   656, 0 }}}, // 16
        {{{   355, 1,  3324, 0 }}}, // 18
        {{{   374, 1,   652, 0 }}}, // 20
        {{{   694, 1,   313, 0 }}}, // 22
        {{{   358, 1,  1305, 0 }}}, // 24
        {{{   374, 1,   988, 0 }}}, // 26
        {{{  1702, 1,   649, 0 }}}, // 28
        {{{   694, 1,   316, 0 }}}, // 30
        {{{   358, 1, 32767, 0 }}}, // 32
        {{{ 23766, 0, 23766, 0 }}},
        {{{ 23766, 0, 23766, 0 }}},
        {{{   710, 1,   316, 0 }}}, // 34
        {{{   355, 1,   316, 0 }}}, // 36
        {{{   358, 1,   313, 0 }}}, // 38
        {{{   358, 1,   988, 0 }}}, // 40
        {{{  1027, 1,   316, 0 }}}, // 42
        {{{  1366, 1,   988, 0 }}}, // 44
        {{{   355, 1,   652, 0 }}}, // 46
        {{{   358, 1,  1308, 0 }}}, // 48
        {{{   371, 1,   656, 0 }}}, // 50
        {{{   352, 1,  3344, 0 }}}, // 52
        {{{   355, 1,   652, 0 }}}, // 54
        {{{   694, 1,   313, 0 }}}, // 56
        {{{   358, 1,  1308, 0 }}}, // 58
        {{{   374, 1,   313, 0 }}}, // 60
        {{{   694, 1,   313, 0 }}}, // 62
        {{{  1366, 1,   652, 0 }}}, // 64
        {{{   355, 1,   652, 0 }}}, // 66
        {{{   358, 1,   313, 0 }}}, // 68
        {{{   358, 1,   313, 0 }}}, // 70
        {{{   358, 1, 32767, 0 }}}, // 72
        {{{   150, 0,   150, 0 }}},
        {{{   150, 0,   150, 0 }}},
    {
Nb pulse = 37
Frame length = 175987
    {
        {{{   713, 1,   313, 0 }}}, // 0
        {{{   358, 1,   320, 0 }}}, // 2
        {{{   352, 1,   320, 0 }}}, // 4
        {{{   352, 1,   992, 0 }}}, // 6
        {{{   358, 1,   985, 0 }}}, // 8
        {{{  1366, 1,   988, 0 }}}, // 10
        {{{   358, 1,   649, 0 }}}, // 12
        {{{   358, 1,  1305, 0 }}}, // 14
        {{{   377, 1,   649, 0 }}}, // 16
        {{{   358, 1,  3340, 0 }}}, // 18
        {{{   358, 1,   313, 0 }}}, // 20
        {{{   358, 1,   313, 0 }}}, // 22
        {{{   358, 1,   316, 0 }}}, // 24
        {{{   355, 1,  1324, 0 }}}, // 26
        {{{   358, 1,   985, 0 }}}, // 28
        {{{  1702, 1,   652, 0 }}}, // 30
        {{{  1702, 1,   313, 0 }}}, // 32
        {{{   358, 1,   316, 0 }}}, // 34
        {{{   355, 1, 32767, 0 }}}, // 36
        {{{ 23344, 0, 23344, 0 }}},
        {{{ 23344, 0, 23344, 0 }}},
        {{{   713, 1,   316, 0 }}}, // 38
        {{{   358, 1,   313, 0 }}}, // 40
        {{{   358, 1,   313, 0 }}}, // 42
        {{{   358, 1,   988, 0 }}}, // 44
        {{{  1030, 1,   313, 0 }}}, // 46
        {{{  1366, 1,   988, 0 }}}, // 48
        {{{   358, 1,   649, 0 }}}, // 50
        {{{   358, 1,  1305, 0 }}}, // 52
        {{{   377, 1,   649, 0 }}}, // 54
        {{{   358, 1,  3340, 0 }}}, // 56
        {{{   358, 1,   313, 0 }}}, // 58
        {{{   358, 1,   313, 0 }}}, // 60
        {{{   358, 1,   316, 0 }}}, // 62
        {{{   355, 1,  1324, 0 }}}, // 64
        {{{   358, 1,   313, 0 }}}, // 66
        {{{   694, 1,   316, 0 }}}, // 68
        {{{  1366, 1,   649, 0 }}}, // 70
        {{{   358, 1,   316, 0 }}}, // 72
        {{{  2374, 1, 32767, 0 }}}, // 74
        {{{    68, 0,    68, 0 }}},
        {{{    68, 0,    68, 0 }}},
    {
Nb pulse = 38
Frame length = 174300
    {
        {{{   713, 1,   316, 0 }}}, // 0
        {{{   355, 1,   316, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   358, 1,   985, 0 }}}, // 6
        {{{  1030, 1,   316, 0 }}}, // 8
        {{{  1366, 1,   988, 0 }}}, // 10
        {{{   355, 1,   652, 0 }}}, // 12
        {{{   355, 1,  1308, 0 }}}, // 14
        {{{   377, 1,   649, 0 }}}, // 16
        {{{   355, 1,  3328, 0 }}}, // 18
        {{{   371, 1,   992, 0 }}}, // 20
        {{{   352, 1,   320, 0 }}}, // 22
        {{{   352, 1,  1308, 0 }}}, // 24
        {{{   377, 1,   988, 0 }}}, // 26
        {{{  1702, 1,   649, 0 }}}, // 28
        {{{   358, 1,  1321, 0 }}}, // 30
        {{{  1030, 1, 32767, 0 }}}, // 32
        {{{ 23429, 0, 23429, 0 }}},
        {{{ 23429, 0, 23429, 0 }}},
        {{{   716, 1,   313, 0 }}}, // 34
        {{{   358, 1,   313, 0 }}}, // 36
        {{{   358, 1,   316, 0 }}}, // 38
        {{{   355, 1,   988, 0 }}}, // 40
        {{{   358, 1,   313, 0 }}}, // 42
        {{{   358, 1,   313, 0 }}}, // 44
        {{{  1363, 1,   992, 0 }}}, // 46
        {{{   358, 1,   649, 0 }}}, // 48
        {{{   358, 1,  1305, 0 }}}, // 50
        {{{   377, 1,   649, 0 }}}, // 52
        {{{   358, 1,  3321, 0 }}}, // 54
        {{{   377, 1,   985, 0 }}}, // 56
        {{{   358, 1,   316, 0 }}}, // 58
        {{{   355, 1,  1308, 0 }}}, // 60
        {{{   374, 1,   313, 0 }}}, // 62
        {{{   694, 1,   316, 0 }}}, // 64
        {{{  1366, 1,   649, 0 }}}, // 66
        {{{   358, 1,   316, 0 }}}, // 68
        {{{   355, 1,   988, 0 }}}, // 70
        {{{   358, 1,   313, 0 }}}, // 72
        {{{   358, 1, 32767, 0 }}}, // 74
        {{{    66, 0,    66, 0 }}},
        {{{    66, 0,    66, 0 }}},
    {
Nb pulse = 38
Frame length = 176323
    {
        {{{   710, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   358, 1,   316, 0 }}}, // 4
        {{{   355, 1,   988, 0 }}}, // 6
        {{{  1030, 1,   316, 0 }}}, // 8
        {{{  1363, 1,   992, 0 }}}, // 10
        {{{   355, 1,   652, 0 }}}, // 12
        {{{   358, 1,  1302, 0 }}}, // 14
        {{{   377, 1,   652, 0 }}}, // 16
        {{{   355, 1,  3324, 0 }}}, // 18
        {{{   374, 1,   316, 0 }}}, // 20
        {{{   694, 1,   649, 0 }}}, // 22
        {{{   358, 1,  1305, 0 }}}, // 24
        {{{   374, 1,   988, 0 }}}, // 26
        {{{  1702, 1,   649, 0 }}}, // 28
        {{{   358, 1,   652, 0 }}}, // 30
        {{{   691, 1,   316, 0 }}}, // 32
        {{{   358, 1,   313, 0 }}}, // 34
        {{{   358, 1, 32767, 0 }}}, // 36
        {{{ 23347, 0, 23347, 0 }}},
        {{{ 23347, 0, 23347, 0 }}},
        {{{   713, 1,   313, 0 }}}, // 38
        {{{   358, 1,   316, 0 }}}, // 40
        {{{   355, 1,   316, 0 }}}, // 42
        {{{   358, 1,   985, 0 }}}, // 44
        {{{   358, 1,   316, 0 }}}, // 46
        {{{   355, 1,   316, 0 }}}, // 48
        {{{  1366, 1,   988, 0 }}}, // 50
        {{{   355, 1,   652, 0 }}}, // 52
        {{{   358, 1,  1305, 0 }}}, // 54
        {{{   374, 1,   652, 0 }}}, // 56
        {{{   355, 1,  3344, 0 }}}, // 58
        {{{   355, 1,   316, 0 }}}, // 60
        {{{   694, 1,   649, 0 }}}, // 62
        {{{   358, 1,  1305, 0 }}}, // 64
        {{{   374, 1,   316, 0 }}}, // 66
        {{{   694, 1,   313, 0 }}}, // 68
        {{{  1366, 1,   652, 0 }}}, // 70
        {{{   358, 1,   313, 0 }}}, // 72
        {{{  2038, 1, 32767, 0 }}}, // 74
        {{{   150, 0,   150, 0 }}},
        {{{   150, 0,   150, 0 }}},
    {
Nb pulse = 38
Frame length = 174307
    {
        {{{   713, 1,   316, 0 }}}, // 0
        {{{   355, 1,   316, 0 }}}, // 2
        {{{   358, 1,   313, 0 }}}, // 4
        {{{   358, 1,   985, 0 }}}, // 6
        {{{   694, 1,   656, 0 }}}, // 8
        {{{  1363, 1,   985, 0 }}}, // 10
        {{{   358, 1,   652, 0 }}}, // 12
        {{{   355, 1,  1308, 0 }}}, // 14
        {{{   374, 1,   649, 0 }}}, // 16
        {{{   358, 1,  3324, 0 }}}, // 18
        {{{   377, 1,   652, 0 }}}, // 20
        {{{   355, 1,   652, 0 }}}, // 22
        {{{   355, 1,  1324, 0 }}}, // 24
        {{{   358, 1,   985, 0 }}}, // 26
        {{{  1705, 1,   649, 0 }}}, // 28
        {{{   358, 1,   313, 0 }}}, // 30
        {{{  1702, 1,   316, 0 }}}, // 32
        {{{   358, 1, 32767, 0 }}}, // 34
        {{{ 23346, 0, 23346, 0 }}},
        {{{ 23346, 0, 23346, 0 }}},
        {{{   710, 1,   313, 0 }}}, // 36
        {{{   358, 1,   316, 0 }}}, // 38
        {{{   355, 1,   316, 0 }}}, // 40
        {{{   355, 1,   988, 0 }}}, // 42
        {{{   358, 1,   988, 0 }}}, // 44
        {{{  1366, 1,   985, 0 }}}, // 46
        {{{   358, 1,   649, 0 }}}, // 48
        {{{   358, 1,  1308, 0 }}}, // 50
        {{{   374, 1,   649, 0 }}}, // 52
        {{{   358, 1,  3344, 0 }}}, // 54
        {{{   355, 1,   656, 0 }}}, // 56
        {{{   352, 1,   656, 0 }}}, // 58
        {{{   355, 1,  1308, 0 }}}, // 60
        {{{   371, 1,   320, 0 }}}, // 62
        {{{   688, 1,   320, 0 }}}, // 64
        {{{  1363, 1,   649, 0 }}}, // 66
        {{{   358, 1,   656, 0 }}}, // 68
        {{{   688, 1,   320, 0 }}}, // 70
        {{{   691, 1, 32767, 0 }}}, // 72
        {{{ 43727, 0, 43727, 0 }}},
        {{{ 43727, 0, 43727, 0 }}},
    {
Nb pulse = 37
Frame length = 175654

Terminating...
Picocom was killed
[glecuyer@netbook2 esp32]$ picocom -b 19200 /dev/ttyACM0 
picocom v3.1

port is        : /dev/ttyACM0
flowcontrol    : none
baudrate is    : 19200
parity is      : none
databits are   : 8
stopbits are   : 1
escape is      : C-a
local echo is  : no
noinit is      : no
noreset is     : no
hangup is      : no
nolock is      : no
send_cmd is    : sz -vv
receive_cmd is : rz -vv -E
imap is        : 
omap is        : 
emap is        : crcrlf,delbs,
logfile is     : none
initstring     : none
exit_after is  : not set
exit is        : no

Type [C-a] [C-h] to see available commands
Terminal ready
HelloAVR

    {
        {{{   713, 1,   316, 0 }}}, // 0
        {{{   355, 1,   316, 0 }}}, // 2
        {{{   355, 1,   316, 0 }}}, // 4
        {{{   355, 1,   988, 0 }}}, // 6
        {{{   358, 1,   316, 0 }}}, // 8
        {{{   355, 1,   316, 0 }}}, // 10
        {{{  1366, 1,   988, 0 }}}, // 12
        {{{   355, 1,   652, 0 }}}, // 14
        {{{   358, 1,  1305, 0 }}}, // 16
        {{{   374, 1,   652, 0 }}}, // 18
        {{{   355, 1,  3324, 0 }}}, // 20
        {{{   710, 1,   320, 0 }}}, // 22
        {{{   688, 1,   320, 0 }}}, // 24
        {{{   352, 1,  1328, 0 }}}, // 26
        {{{   358, 1,   985, 0 }}}, // 28
        {{{  1702, 1,   652, 0 }}}, // 30
        {{{   355, 1,   316, 0 }}}, // 32
        {{{   358, 1,  1302, 0 }}}, // 34
        {{{   377, 1, 32767, 0 }}}, // 36
        {{{ 23429, 0, 23429, 0 }}},
        {{{ 23429, 0, 23429, 0 }}},
        {{{   713, 1,   316, 0 }}}, // 38
        {{{   355, 1,   316, 0 }}}, // 40
        {{{   355, 1,   316, 0 }}}, // 42
        {{{   358, 1,   985, 0 }}}, // 44
        {{{   694, 1,   656, 0 }}}, // 46
        {{{  1360, 1,   992, 0 }}}, // 48
        {{{   358, 1,   649, 0 }}}, // 50
        {{{   358, 1,  1305, 0 }}}, // 52
        {{{   374, 1,   652, 0 }}}, // 54
        {{{   358, 1,  3321, 0 }}}, // 56
        {{{   710, 1,   316, 0 }}}, // 58
        {{{   694, 1,   313, 0 }}}, // 60
        {{{   358, 1,  1308, 0 }}}, // 62
        {{{   371, 1,   316, 0 }}}, // 64
        {{{   691, 1,   316, 0 }}}, // 66
        {{{  1366, 1,   652, 0 }}}, // 68
        {{{  1030, 1,   652, 0 }}}, // 70
        {{{   355, 1, 32767, 0 }}}, // 72
        {{{   319, 0,   319, 0 }}},
        {{{   319, 0,   319, 0 }}},
    {
Nb pulse = 37
Frame length = 175315
    {
        {{{   713, 1,   313, 0 }}}, // 0
        {{{   358, 1,   313, 0 }}}, // 2
        {{{   358, 1,   316, 0 }}}, // 4
        {{{   355, 1,   988, 0 }}}, // 6
        {{{   691, 1,   652, 0 }}}, // 8
        {{{  1366, 1,   988, 0 }}}, // 10
        {{{   355, 1,   656, 0 }}}, // 12
        {{{   352, 1,  1308, 0 }}}, // 14
        {{{   377, 1,   649, 0 }}}, // 16
        {{{   358, 1,  3324, 0 }}}, // 18
        {{{  1046, 1,   313, 0 }}}, // 20
        {{{   358, 1,   313, 0 }}}, // 22
        {{{   358, 1,  1308, 0 }}}, // 24
        {{{   371, 1,   992, 0 }}}, // 26
        {{{  1702, 1,   649, 0 }}}, // 28
        {{{   358, 1,   316, 0 }}}, // 30
        {{{   691, 1,   652, 0 }}}, // 32
        {{{   355, 1,   316, 0 }}}, // 34
        {{{   355, 1, 32767, 0 }}}, // 36
        {{{ 23345, 0, 23345, 0 }}},
        {{{ 23345, 0, 23345, 0 }}},
        {{{   713, 1,   316, 0 }}}, // 38
        {{{   355, 1,   316, 0 }}}, // 40
        {{{   355, 1,   316, 0 }}}, // 42
        {{{   355, 1,   988, 0 }}}, // 44
        {{{   358, 1,   988, 0 }}}, // 46
        {{{  1363, 1,   992, 0 }}}, // 48
        {{{   352, 1,   656, 0 }}}, // 50
        {{{   358, 1,  1302, 0 }}}, // 52
        {{{   377, 1,   652, 0 }}}, // 54
        {{{   352, 1,  3328, 0 }}}, // 56
        {{{  1046, 1,   316, 0 }}}, // 58
        {{{   355, 1,   316, 0 }}}, // 60
        {{{   358, 1,  1305, 0 }}}, // 62
        {{{   374, 1,   313, 0 }}}, // 64
        {{{   694, 1,   320, 0 }}}, // 66
        {{{  1360, 1,   656, 0 }}}, // 68
        {{{   358, 1,   649, 0 }}}, // 70
        {{{   358, 1,   313, 0 }}}, // 72
        {{{  1030, 1, 32767, 0 }}}, // 74
        {{{   152, 0,   152, 0 }}},
        {{{   152, 0,   152, 0 }}},
    {
Nb pulse = 38
Frame length = 175308

*/

rmt_item32_t * txIitems[] = {
	txIitems01,
	txIitems02,
	txIitems03,
	txIitems04,
	txIitems05,
	txIitems06,
	txIitems11,
	txIitems12,
	txIitems13,
	txIitems14,
	txIitems15,
	txIitems16,

}; 


/*
 * @brief Build LEGRAND  waveform from raw data.
 */



void InfraredLegrandTxExecute( short index ){
    int nbItem;

	// Expecting "RMT end marker" shall stop TX
	nbItem = 40;
	
	if  ( index > 11 )
		index = 11;
	
    //To send data according to the waveform items.
    rmt_write_items(RMT_TX_CHANNEL, txIitems[index], nbItem, true);
    //Wait until sending is done.
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);   
    
}

