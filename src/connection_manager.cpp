 /**
 * @connection_manager
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
 * Connection Manager listens for incoming connections/messages from the
 * controller and other routers and calls the desginated handlers.
 */

using namespace std;

#include <sys/select.h>
#include <sys/time.h>

#include "../include/connection_manager.h"
#include "../include/global.h"
#include "../include/control_handler.h"
#include "../include/timer_manager.h"
#include "../include/update_manager.h"
#include "../include/router_manager.h"
#include "../include/recvfile_manager.h"


fd_set master_list, watch_list;
int head_fd;

void add_socket(int sock_index) {
    FD_SET(sock_index, &master_list);
    if (sock_index > head_fd) head_fd = sock_index;
}

void timeout_event() {

    printf("isTimeOut!!!\n");

    uint16_t stop_id = get_stop_id_and_pop();

    if (is_self(stop_id)) {

        send_update(router_socket);

    } else {

        remove_from_net(stop_id);

    }
}

void main_loop()
{
    int selret, sock_index, fdaccept;

    while(1){

        watch_list = master_list;

        while (has_timer()) {

            int stop_time = get_next_stop_time();

            if (stop_time < 0) {

                timeout_event();

            } else break;
            
        }


        if (!has_timer()) {

            // printf("%s\n", "Select Start No Timer");
            selret = select(head_fd+1, &watch_list, NULL, NULL, NULL);

        } else {

            int stop_time = get_next_stop_time();

            struct timeval tv;
            tv.tv_sec = stop_time;

            // printf("Select Start %d Sec After\n", stop_time);
            selret = select(head_fd+1, &watch_list, NULL, NULL, &tv);

        }

        if(selret < 0)
            ERROR("select failed.");

        /* Loop through file descriptors to check which ones are ready */

        // printf("select return\n");

        for(sock_index=0; sock_index<=head_fd; sock_index+=1){

            if(FD_ISSET(sock_index, &watch_list)){

                /* control_socket */
                if(sock_index == control_socket){
                    fdaccept = new_control_conn(sock_index);

                    /* Add to watched socket list */
                    FD_SET(fdaccept, &master_list);
                    if(fdaccept > head_fd) head_fd = fdaccept;
                }

                /* router_socket */
                else if(sock_index == router_socket){
                    receive_update(router_socket);
                }

                /* data_socket */
                else if(sock_index == data_socket){
                    fdaccept = create_data_recv_socket(data_socket);

                    FD_SET(fdaccept, &master_list);
                    if (fdaccept > head_fd) head_fd = fdaccept;
                }

                /* Existing connection */
                else{
                    if(isControl(sock_index)){
                        if(!control_recv_hook(sock_index)) FD_CLR(sock_index, &master_list);
                    }
                    //else if isData(sock_index);

                    else if (isData(sock_index)) {
                        if (!recvfile(sock_index)) FD_CLR(sock_index, &master_list);
                    }
                    else ERROR("Unknown socket index");
                }
            } 
        } 

        if (selret == 0) {

            timeout_event();

        }
    }
}

void init()
{
    control_socket = create_control_sock();

    //router_socket and data_socket will be initialized after INIT from controller

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the control socket */
    FD_SET(control_socket, &master_list);
    head_fd = control_socket;
    
    main_loop();
}