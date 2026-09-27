idf_component_register(SRCS "dht11.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_gpio esp-idf-lib__dht
                    REQUIRES common)
