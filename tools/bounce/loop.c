//
// Copyright(C) Simon Howard
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
//---------------------------------------------------------------------
//
// TCP 'loop'
// This is kind of like the 'loops' that used to exist in the phone
// system: these consisted of two numbers, if you dialed one and
// somebody else dialed the other then you could talk for free.
//
// Under this, two (or more) people connecting to the loop can
// 'talk': all data sent by one connection is forwarded to the
// others
//
//---------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_CONNECTIONS 16
#define BUFSIZE 128

static int port;
static int listen_sock;
static int connections[MAX_CONNECTIONS];
static int num_connections = 0;

void accept_connection()
{
        if (num_connections >= MAX_CONNECTIONS) {
                fprintf(stderr, "accept_connection: no more connections!\n");
                exit(-1);
        }

        connections[num_connections] = accept(listen_sock, NULL, NULL);

        if (connections[num_connections] < 0) {
                perror("error accepting connection\n");
                return;
        }

        ++num_connections;
}

void kill_connection(int i)
{
        close(connections[i]);

        for(; i<num_connections; ++i)
                connections[i] = connections[i+1];

        --num_connections;
}

int main(int argc, char *argv[])
{
        struct sockaddr_in in;

        if (argc < 2) {
                puts("Usage: loop <port>");
                exit(-1);
        }

        port = atoi(argv[1]);

        // set up listen_sock

        listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        memset(&in, 0, sizeof(in));

        in.sin_family = AF_INET;
        in.sin_addr.s_addr = INADDR_ANY;
        in.sin_port = htons(port);

        if (bind(listen_sock, (struct sockaddr *) &in, sizeof(in)) < 0) {
                perror("cant bind listen_sock");
                exit(-1);
        }

        if (listen(listen_sock, 1) < 0) {
                perror("cant listen");
                exit(-1);
        }

        // main loop

        for (;;) {
                fd_set fd_list;
                int max = listen_sock;
                int i;

                FD_ZERO(&fd_list);
                FD_SET(listen_sock, &fd_list);

                for (i=0; i<num_connections; ++i) {
                        FD_SET(connections[i], &fd_list);
                        if (connections[i] > max)
                                max = connections[i];
                }

                select(max+1, &fd_list, NULL, NULL, NULL);

                if (FD_ISSET(listen_sock, &fd_list)) {
                        accept_connection();
                        continue;
                }

                for (i=0; i<num_connections; ++i) {
                        if (FD_ISSET(connections[i], &fd_list)) {
                                char readbuf[BUFSIZE];
                                int read;
                                int n;

                                // check for closed socket or error
                                if ((read = recv(connections[i], readbuf,
                                                 BUFSIZE, 0)) <= 0) {
                                        kill_connection(i);
                                        break;
                                }

                                // forward to other connections

                                for (n=0; n<num_connections; ++n) {
                                        if (n == i)
                                                continue;

                                        send(connections[n], readbuf, read, 0);
                                }
                        }
                }
        }
}
