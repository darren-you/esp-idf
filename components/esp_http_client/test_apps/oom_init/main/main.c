// SPDX-License-Identifier: Apache-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_heap_caps.h"
#include "esp_http_client.h"
#include "esp_transport.h"

static const char *fail_transport_scheme;

esp_err_t __real_esp_transport_list_add(esp_transport_list_handle_t list,
                                        esp_transport_handle_t transport,
                                        const char *scheme);

esp_err_t __wrap_esp_transport_list_add(esp_transport_list_handle_t list,
                                        esp_transport_handle_t transport,
                                        const char *scheme)
{
    if (fail_transport_scheme != NULL && strcmp(scheme, fail_transport_scheme) == 0) {
        return ESP_ERR_NO_MEM;
    }
    return __real_esp_transport_list_add(list, transport, scheme);
}

static void rejected_init(const char *scheme)
{
    const esp_http_client_config_t config = {.url = "https://example.com/"};
    fail_transport_scheme = scheme;
    esp_http_client_handle_t client = esp_http_client_init(&config);
    fail_transport_scheme = NULL;
    if (client != NULL) abort();
}

static void check_failed_add(const char *scheme)
{
    rejected_init(scheme); /* Warm the error logging path before measuring. */
    const size_t before = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    for (unsigned int attempt = 0; attempt < 8; ++attempt) rejected_init(scheme);
    const size_t after = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("esp_http_client_init %s OOM: before=%zu after=%zu delta=%zu\n",
           scheme, before, after, before - after);
    if (after != before) abort();
}

void app_main(void)
{
    const esp_http_client_config_t config = {.url = "https://example.com/"};
    esp_http_client_handle_t warmup = esp_http_client_init(&config);
    if (warmup == NULL) abort();
    if (esp_http_client_cleanup(warmup) != ESP_OK) abort();

    check_failed_add("http");
    check_failed_add("https");
    puts("esp_http_client_init OOM: PASS");
}
