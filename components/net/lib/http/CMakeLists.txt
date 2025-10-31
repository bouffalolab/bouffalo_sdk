# SPDX-License-Identifier: Apache-2.0

sdk_generate_library()

if(CONFIG_HTTP_PARSER_STRICT)
sdk_add_compile_definitions(HTTP_PARSER_STRICT)
endif()

sdk_add_include_directories(inc .)

sdk_library_add_sources(src/http_parser.c src/http_parser_url.c)

sdk_library_add_sources_ifdef(CONFIG_HTTP_CLIENT https_wrapper.c https_client.c
                            src/http_client.c)
sdk_library_add_sources_ifdef(CONFIG_HTTP_SERVER src/http_server_core.c
                        src/http_server_http1.c
                        src/http_server_http2.c
                        src/http_hpack.c
                        src/http_huffman.c)
if(CONFIG_HTTP_SERVER AND CONFIG_WEBSOCKET)
  sdk_library_add_sources(src/http_server_ws.c)
endif()


