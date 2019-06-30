#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <sys/param.h>

#include <esp_http_server.h>
#include "esp_log.h"

esp_err_t hello_get_handler(httpd_req_t *req);
esp_err_t echo_post_handler(httpd_req_t *req);
esp_err_t ctrl_put_handler(httpd_req_t *req);
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t server);
static esp_err_t event_handler(void *ctx, system_event_t *event);