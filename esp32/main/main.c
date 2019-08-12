/* irBridge main (entry point)

   Based on simple http server example provided in ESP32 SDK
   (IDF_PATH)



*/

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include <esp_http_server.h>

#include "IrBridge.h"
#include "WebUi.h"

#include "Wifi_SSIDs.h"

QueueHandle_t  q=NULL;


/*
 *
 * WiFi configurations are defined in Wifi_SSIDs.h
 *
*/
wifi_config_t wifiConfigs[] = {
    {
        .sta = {
            .ssid = IR_WIFI_SSID_0,
            .password = IR_WIFI_PASS_0,
        },
    },
    {
        .sta = {
            .ssid = IR_WIFI_SSID_1,
            .password = IR_WIFI_PASS_1,
        },
    },
};
unsigned short iWifiConfig = 0;


#define GPIO_RED_LED 5




static const char *TAG="APP";


void sendCommand( unsigned value ){
    if ( q == NULL )
        return;
    ESP_LOGI(TAG, "sendCommand (HEX): %X", value);
    xQueueSend(q,(void *)&value,(TickType_t )0);
}


/* An HTTP GET handler */
esp_err_t ir_bridge_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    ESP_LOGI(TAG, "ir_bridge_handler: uri= %s", req->uri);

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    if( strstr( req->uri, "Bridge" ) != NULL ){
        httpd_resp_send(req, (const char *)samsung_html, samsung_html_size());
    }
    else if( strstr( req->uri, "samsung.css" ) != NULL ){
        ESP_LOGI(TAG, "Sending samsung.css");
        httpd_resp_set_type     (req, "text/css");
        httpd_resp_send(req, (const char *)samsung_css, samsung_css_size());
    }
    else if( strstr( req->uri, "legrand.css" ) != NULL ){
        ESP_LOGI(TAG, "Sending legrand.css");
        httpd_resp_set_type     (req, "text/css");
        httpd_resp_send(req, (const char *)legrand_css, legrand_css_size());
    }
    else if( strstr( req->uri, "legrand.png" ) != NULL ){
        ESP_LOGI(TAG, "Sending legrand.png");
        httpd_resp_set_type     (req, "image/png");
        httpd_resp_send(req, (const char *)legrand_png, legrand_png_size());
    }
    else if( strstr( req->uri, "keyClick.js" ) != NULL ){
        ESP_LOGI(TAG, "Sending legrand.png");
        httpd_resp_set_type     (req, "text/javascript");
        httpd_resp_send(req, (const char *)keyClick_js, keyClick_js_size());
    }
    else if( strstr( req->uri, "legrand.html" ) != NULL ){
        ESP_LOGI(TAG, "legrand.html");
        httpd_resp_send(req, (const char *)legrand_html, legrand_html_size());
    }
    else if( strstr( req->uri, "legrand.png" ) != NULL ){
        ESP_LOGI(TAG, "Sending legrand.png");
        httpd_resp_set_type     (req, "image/png");
        httpd_resp_send(req, (const char *)legrand_png, legrand_png_size());
    }
    else if( strstr( req->uri, "power.png" ) != NULL ){
        ESP_LOGI(TAG, "Sending power.png");
        httpd_resp_set_type     (req, "image/png");
        httpd_resp_send(req, (const char *)power_png, power_png_size());
    }
    else if( strstr( req->uri, "samsung.png" ) != NULL ){
        ESP_LOGI(TAG, "Sending samsung.png");
        httpd_resp_set_type     (req, "image/png");
        httpd_resp_send(req, (const char *)samsung_png, samsung_png_size());
    }
    else if( strstr( req->uri, "KeyName" ) != NULL ){
        ESP_LOGI(TAG, "Receiving %s", req->uri );
        buf_len = httpd_req_get_hdr_value_len(req, "Value") + 1;
        if (buf_len > 1) {
            buf = malloc(buf_len);
            if (httpd_req_get_hdr_value_str(req, "Value", buf, buf_len) == ESP_OK) {
                ESP_LOGI(TAG, "Found Value : %s", buf);
                sendCommand( (buf[0]-'0')<<8 | (buf[1]-'0')<<4 | (buf[2]-'0') );  // BCD like format
            }
            free(buf);
        }
        httpd_resp_set_type     (req, "text/plain");
        httpd_resp_send(req, (const char *)"OK", 2);
    }
    else {
        ESP_LOGI(TAG, "Sending default");
        httpd_resp_set_type     (req, "text/plain");
        httpd_resp_send(req, (const char *)"Not found      ", 10);

    }
    /*
        "image/jpeg"    // images JPEG
        "image/png" // images PNG
    */

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}


httpd_uri_t irBridge = {
    .uri       = "/Bridge",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};

httpd_uri_t samsungCss = {
    .uri       = "/samsung.css",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};

httpd_uri_t legrandCss = {
    .uri       = "/legrand.css",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};


httpd_uri_t keyClickJs = {
    .uri       = "/keyClick.js",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};

httpd_uri_t samsungPng = {
    .uri       = "/images/samsung.png",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};

httpd_uri_t legrandPng = {
    .uri       = "/images/legrand.png",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};

httpd_uri_t legrandHtml = {
    .uri       = "/legrand.html",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};

httpd_uri_t powerPng = {
    .uri       = "/images/power.png",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};

httpd_uri_t keyName = {
    .uri       = "/KeyName",
    .method    = HTTP_GET,
    .handler   = ir_bridge_handler,
    .user_ctx  = NULL
};


httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Overwrite default to 10
    config.max_uri_handlers = 10;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &irBridge);
        httpd_register_uri_handler(server, &samsungCss);
        httpd_register_uri_handler(server, &legrandCss);
        httpd_register_uri_handler(server, &keyClickJs);
        httpd_register_uri_handler(server, &samsungPng);
        httpd_register_uri_handler(server, &legrandPng);
        httpd_register_uri_handler(server, &legrandHtml);
        httpd_register_uri_handler(server, &powerPng);
        httpd_register_uri_handler(server, &keyName);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}


static void retry_wifi(void *arg)
{
    iWifiConfig++;

    if ( iWifiConfig >= sizeof(wifiConfigs)/sizeof(wifi_config_t) ){
        iWifiConfig = 0;
    }

    wifi_config_t * pWifiConfig = &wifiConfigs[iWifiConfig];

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", pWifiConfig->sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, pWifiConfig ));
    ESP_ERROR_CHECK(esp_wifi_start());
}


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    httpd_handle_t *server = (httpd_handle_t *) ctx;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "Got IP: '%s'",
                ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        /* Start the web server */
        if (*server == NULL) {
            *server = start_webserver();
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        gpio_set_level(GPIO_RED_LED, 1);

        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());

        retry_wifi(NULL);

        /* Stop the web server */
        if (*server) {
            stop_webserver(*server);
            *server = NULL;
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void *arg)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_config_t * pWifiConfig = &wifiConfigs[0];
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", pWifiConfig->sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, pWifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());
}



void infrared_task(void *pvParameter)
{
    // Init Infrared module
    InfraredTxInit();

    // Message loop
    // Receive from Network, send to IR
    unsigned int MesgValue;
    if(q == NULL){
        printf("Queue is not ready");
        return;
    }
    while(1){
        // xQueueReceive(xQueue, pvBuffer, xTicksToWait)
        if( xQueueReceive(q, &MesgValue,(TickType_t )(1000/portTICK_PERIOD_MS)) ){
            printf("value received on queue: %04x \n", MesgValue);
            gpio_set_level(GPIO_RED_LED, 1);

            if ( MesgValue >= 0x900 )
                InfraredSamsungCodeSend( MesgValue );
            else
                InfraredLegrandCodeSend( MesgValue );
        }
        else {
            gpio_set_level(GPIO_RED_LED, 0);
            printf("xQueueReceive time out \n");
        }
    }
}

/***********************************************************************
 *
 * Program entry point
 *
 * It creates two tasks
 *    http server
 *    infrared sender
 * A queue is used for tasks communication
 *
 **********************************************************************/

void app_main()
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());

    // Create a message queue to communicate
    // From http handler to IR task
    q=xQueueCreate(20, sizeof(unsigned int));
    if ( q == NULL ){
        ESP_LOGE(TAG, "Queue creation error!");
        return;
    }

    /* Configure the IOMUX register for pad BLINK_GPIO
     * to drive one led (simple indicator).
    */
    gpio_pad_select_gpio(GPIO_RED_LED);

    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_RED_LED, GPIO_MODE_OUTPUT);

    // TCP/IP + Wifi
    initialise_wifi(&server);

    // Infrared
    xTaskCreate(&infrared_task, "infrared_task", 2048, NULL, 5, NULL);

}
