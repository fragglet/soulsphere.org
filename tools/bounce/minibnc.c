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
//
// Simple connection bouncer
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// read buffer size
#define BUFSIZE 128

void sigchld_handler()
{
  wait(NULL);
}

int main(int argc, char *argv[])
{
  struct hostent *hent;
  struct sockaddr_in in, target;
  int listen_sock;
  int input_sock, output_sock;

  signal(SIGCHLD, sigchld_handler);

  if(argc < 4)
    {
      printf("usage: %s <listening port> <host> <portnum>\n", argv[0]);
      exit(-1);
    }

  // resolve the destination address now before
  // we start waiting for connections

  if(!(hent = gethostbyname(argv[2])))
    {
      perror("cant resolve destination address");
      exit(-1);
    }

  // set up listening socket and wait for a connection

  listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  memset ( &in, 0, sizeof ( struct sockaddr_in ) );

  in.sin_family = AF_INET;
  in.sin_addr.s_addr = INADDR_ANY;
  in.sin_port = htons(atoi(argv[1]));

  if(bind(listen_sock,
	  (struct sockaddr *) &in, sizeof (struct sockaddr_in)) < 0)
    {
      perror("cant bind socket");
      exit(-1);
    }

  if(listen(listen_sock, 10) < 0)
    {
      perror("cant listen");
      exit(-1);
    }

  while(1)
    {
      // wait for a connection

      if((input_sock = accept(listen_sock, NULL, NULL)) < 0)
	{
	  perror("error accepting connection");
	  exit(-1);
	}

      // fork a new process for the new connection

      if (fork())
	continue;

      // now we have a connection from the input port, establish a connection
      // to the destination host

      output_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      target.sin_family = AF_INET;
      target.sin_addr.s_addr = ((struct in_addr *) hent->h_addr)->s_addr;
      target.sin_port = htons(atoi(argv[3]));

      if(connect(output_sock, (struct sockaddr *)&target, sizeof(target)) < 0)
	{
	  perror("connect");
	  exit(-1);
	}

      // now we just set up a loop passing data between the 2 ports

      while(1)
	{
	  fd_set fd_list;

	  FD_ZERO(&fd_list);
	  FD_SET(input_sock, &fd_list);
	  FD_SET(output_sock, &fd_list);

	  select((input_sock > output_sock ? input_sock : output_sock) + 1,
		 &fd_list,
		 NULL, NULL, NULL);

	  if(FD_ISSET(input_sock, &fd_list))
	    {
	      char readbuf[BUFSIZE];
	      int read;

	      // check for closed socket or error
	      if((read = recv(input_sock, readbuf, BUFSIZE, 0)) <= 0)
		break;

	      send(output_sock, readbuf, read, 0);
	    }

	  if(FD_ISSET(output_sock, &fd_list))
	    {
	      char readbuf[BUFSIZE];
	      int read;

	      // check for closed socket or error
	      if((read = recv(output_sock, readbuf, BUFSIZE, 0)) <= 0)
		break;

	      send(input_sock, readbuf, read, 0);
	    }
	}

      close(input_sock);
      close(output_sock);
    }

}

