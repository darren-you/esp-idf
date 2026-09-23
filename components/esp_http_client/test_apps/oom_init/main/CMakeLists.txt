idf_component_register(SRCS "main.c"
                       REQUIRES esp_http_client tcp_transport heap)

target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=esp_transport_list_add")
