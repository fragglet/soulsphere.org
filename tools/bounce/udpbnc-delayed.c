//
// Copyright(C) 2007 Simon Howard
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
// IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//
// UDP bouncer, with a delay deliberately inserted on all packets sent.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// read buffer size
#define BUFSIZE 128

// lag time in ms
#define LAG 300

typedef struct packet_s packet_t;

typedef enum
{
        PACKET_SOURCE_SERVER,
        PACKET_SOURCE_CLIENT,
} packet_source_t;

struct packet_s
{
        void *data;
        size_t data_length;
        packet_source_t source;
        uint64_t send_time;
        packet_t *next;
};

struct sockaddr_in client, server;
packet_t *client_queue = NULL;
packet_t *server_queue = NULL;
int sock;

uint64_t get_time(void)
{
        struct timeval tv;

        gettimeofday(&tv, NULL);

        return ((uint64_t) tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

void init_destination(char *dest_addr, int dest_port)
{
        struct hostent *hent;

        // resolve the destination address now before
        // we start waiting for connections

        if(!(hent = gethostbyname(dest_addr)))
        {
                perror("cant resolve destination address");
                exit(-1);
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = ((struct in_addr *) hent->h_addr)->s_addr;
        server.sin_port = htons(dest_port);
}

void init_socket(int port)
{
        struct sockaddr_in in;

        // set up listening socket and wait for a connection

        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        memset(&in, 0, sizeof(struct sockaddr_in));

        in.sin_family = AF_INET;
        in.sin_addr.s_addr = INADDR_ANY;
        in.sin_port = htons(port);

        if (bind(sock, (struct sockaddr *) &in,
                 sizeof (struct sockaddr_in)) < 0)
        {
                perror("cant bind socket");
                exit(-1);
        }
}

packet_t *new_packet(void *data, size_t data_length)
{
        packet_t *packet;

        packet = malloc(sizeof(packet_t));

        packet->data = malloc(data_length);
        packet->data_length = data_length;
        memcpy(packet->data, data, data_length);

        return packet;
}

void free_packet(packet_t *packet)
{
        free(packet->data);
        free(packet);
}

packet_t *read_packet(void)
{
        struct sockaddr_in src;
        socklen_t addrlen;
        char buf[3000];
        packet_t *packet;
	int bytes;

        addrlen = sizeof(struct sockaddr_in);

        bytes = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *) &src, &addrlen);

        if (bytes <= 0)
        {
                return NULL;
        }

        packet = new_packet(buf, bytes);

        // Has this come from the server?

        if (src.sin_addr.s_addr == server.sin_addr.s_addr
         && src.sin_port == server.sin_port)
        {
                packet->source = PACKET_SOURCE_SERVER;
        }
        else
        {
                // this came from another machine
                // - assume this other machine is the client. save the
                // client address

                memcpy(&client, &src, sizeof(struct sockaddr_in));

                packet->source = PACKET_SOURCE_CLIENT;
        }

        return packet;
}

void queue_packet(packet_t **queue, packet_t *packet)
{
        // Hook into the end of the list

        while (*queue != NULL)
        {
                queue = &(*queue)->next;
        }

        packet->next = NULL;
        *queue = packet;

        // Set a transmission time with a lag from the current time

        packet->send_time = get_time() + LAG;
}

void get_packets(void)
{
        packet_t *packet;

        packet = read_packet();

        if (packet != NULL)
        {
                // Queue the packet for transmission to the client or
                // server, depending on its source.

                if (packet->source == PACKET_SOURCE_CLIENT)
                {
                        queue_packet(&server_queue, packet);
                }
                else
                {
                        queue_packet(&client_queue, packet);
                }
        }
}

uint64_t queue_timeout(packet_t *queue, uint64_t now)
{
        if (queue == NULL)
        {
                return UINT64_MAX;
        }

        if (now < queue->send_time)
        {
                return queue->send_time - now;
        }
        else
        {
                return 0;
        }
}

uint64_t calculate_timeout(void)
{
        uint64_t client_timeout, server_timeout;
        uint64_t now;

        now = get_time();

        client_timeout = queue_timeout(client_queue, now);
        server_timeout = queue_timeout(client_queue, now);

        if (client_timeout < server_timeout)
        {
                return client_timeout;
        }
        else
        {
                return server_timeout;
        }
}

void sleep_for_packets(void)
{
        struct timeval tv;
        struct timeval *tv_param;
        uint64_t timeout;
        fd_set fd_list;
        int result;

        // Calculate timeout

        timeout = calculate_timeout();

        if (timeout == UINT64_MAX)
        {
                tv_param = NULL;
        }
        else
        {
                tv_param = &tv;
                tv.tv_sec = timeout / 1000;
                tv.tv_usec = (timeout % 1000) * 1000;
        }

        // Call select() to sleep until a packet is received or the timeout
        // is reached.

        FD_ZERO(&fd_list);
        FD_SET(sock, &fd_list);

        result = select(sock + 1, &fd_list, NULL, NULL, tv_param);

        if (result < 0)
        {
                perror("select");
                exit(-1);
        }

        if (FD_ISSET(sock, &fd_list))
        {
                get_packets();
        }
}

void send_packet(packet_t *packet, struct sockaddr_in *dest)
{
        socklen_t addr_length;

        addr_length = sizeof(struct sockaddr_in);

        sendto(sock, packet->data, packet->data_length,
               0, (struct sockaddr *) dest, addr_length);
}

void run_queue(packet_t **queue, struct sockaddr_in *dest)
{
        uint64_t now;
        int result;
        packet_t *packet;

        now = get_time();

        // Process all packets that are ready to send

        while (*queue != NULL && now > (*queue)->send_time)
        {
                // Pop from the queue

                packet = *queue;
                *queue = packet->next;

                // Send packet

                send_packet(packet, dest);
                free_packet(packet);
        }
}

void send_packets(void)
{
        run_queue(&client_queue, &client);
        run_queue(&server_queue, &server);
}

int main(int argc, char *argv[])
{
        if (argc < 4)
        {
                printf("usage: %s <listening port> <host> <portnum>\n", argv[0]);
                exit(-1);
        }

        init_destination(argv[2], atoi(argv[3]));
        init_socket(atoi(argv[1]));

        for (;;)
        {
                sleep_for_packets();
                send_packets();
        }
}

