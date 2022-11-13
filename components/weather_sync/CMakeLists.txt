idf_component_register(SRCS "weather.c"
                        INCLUDE_DIRS "include"
                        REQUIRES "http" "jsmn")