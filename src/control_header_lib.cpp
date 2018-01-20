/**
 * @control_header_lib
 * @author  Swetank Kumar Saha <swetankk@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Routines to parse/generated control/control-response headers.
 */

using namespace std;

#include <string.h>
#include <netinet/in.h>

#include "../include/global.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"
#include "../include/router_manager.h"
#include "../include/update_manager.h"
#include "../include/recvfile_manager.h"

#ifndef PACKET_USING_STRUCT
    #define CNTRL_RESP_CONTROL_CODE_OFFSET 0x04
    #define CNTRL_RESP_RESPONSE_CODE_OFFSET 0x05
    #define CNTRL_RESP_PAYLOAD_LEN_OFFSET 0x06
#endif

void init_response(int sock_index, char* cntrl_payload) {
    uint16_t routers_num;
    uint16_t update_interval;
    ssize_t send_len;

    struct INIT_HEADER* init_header = (struct INIT_HEADER*)malloc(sizeof(char)*INIT_HEADER_SIZE);
    bzero(init_header, INIT_HEADER_SIZE);
    memcpy(init_header, cntrl_payload, INIT_HEADER_SIZE);

    routers_num = ntohs(init_header->routers_num);
    update_interval = ntohs(init_header->update_interval);
    set_init(routers_num, update_interval);
    free(init_header);

    set_init(routers_num, update_interval);

    for (int i = 0; i < routers_num; i++) {

        struct INIT_ROUTER_INFO* init_router_info = (struct INIT_ROUTER_INFO*)malloc(INIT_ROUTER_INFO_SIZE);
        bzero(init_router_info, INIT_ROUTER_INFO_SIZE);
        memcpy(init_router_info, cntrl_payload+INIT_HEADER_SIZE+i*INIT_ROUTER_INFO_SIZE, INIT_ROUTER_INFO_SIZE);

        init_router(ntohs(init_router_info->id), ntohs(init_router_info->router_port), ntohs(init_router_info->data_port), ntohs(init_router_info->cost), ntohl(init_router_info->router_ip_addr));

        free(init_router_info);
    }

    init_router_socket();

    create_data_socket(DATA_PORT);

    init_table();

    uint8_t control_code = 0x01;
    uint8_t response_code = 0x00;
    uint16_t payload_len = 0;
    char* response_header = create_response_header(sock_index, control_code, response_code, payload_len);
    send_len = CNTRL_RESP_HEADER_SIZE;
    sendALL(sock_index, response_header, send_len);
    
}

char* create_response_header(int sock_index, uint8_t control_code, uint8_t response_code, uint16_t payload_len)
{
    char *buffer;
    #ifdef PACKET_USING_STRUCT
        /** ASSERT(sizeof(struct CONTROL_RESPONSE_HEADER) == 8) 
          * This is not really necessary with the __packed__ directive supplied during declaration (see control_header_lib.h).
          * If this fails, comment #define PACKET_USING_STRUCT in control_header_lib.h
          */
        BUILD_BUG_ON(sizeof(struct CONTROL_RESPONSE_HEADER) != CNTRL_RESP_HEADER_SIZE); // This will FAIL during compilation itself; See comment above.

        struct CONTROL_RESPONSE_HEADER *cntrl_resp_header;
    #endif
    #ifndef PACKET_USING_STRUCT
        char *cntrl_resp_header;
    #endif

    struct sockaddr_in addr;
    socklen_t addr_size;

    buffer = (char *) malloc(sizeof(char)*CNTRL_RESP_HEADER_SIZE);
    #ifdef PACKET_USING_STRUCT
        cntrl_resp_header = (struct CONTROL_RESPONSE_HEADER *) buffer;
    #endif
    #ifndef PACKET_USING_STRUCT
        cntrl_resp_header = buffer;
    #endif

    addr_size = sizeof(struct sockaddr_in);
    getpeername(sock_index, (struct sockaddr *)&addr, &addr_size);

    #ifdef PACKET_USING_STRUCT
        /* Controller IP Address */
        memcpy(&(cntrl_resp_header->controller_ip_addr), &(addr.sin_addr), sizeof(struct in_addr));
        /* Control Code */
        cntrl_resp_header->control_code = control_code;
        /* Response Code */
        cntrl_resp_header->response_code = response_code;
        /* Payload Length */
        cntrl_resp_header->payload_len = htons(payload_len);
    #endif

    #ifndef PACKET_USING_STRUCT
        /* Controller IP Address */
        memcpy(cntrl_resp_header, &(addr.sin_addr), sizeof(struct in_addr));
        /* Control Code */
        memcpy(cntrl_resp_header+CNTRL_RESP_CONTROL_CODE_OFFSET, &control_code, sizeof(control_code));
        /* Response Code */
        memcpy(cntrl_resp_header+CNTRL_RESP_RESPONSE_CODE_OFFSET, &response_code, sizeof(response_code));
        /* Payload Length */
        payload_len = htons(payload_len);
        memcpy(cntrl_resp_header+CNTRL_RESP_PAYLOAD_LEN_OFFSET, &payload_len, sizeof(payload_len));
    #endif

    return buffer;
}