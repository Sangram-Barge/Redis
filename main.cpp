#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

static void err_shutdown(const char *msg) {
  int err = errno;
  fprintf(stdout, "%d %s\n", err, msg);
  abort();  
}

int main()  {
  // open socket connection
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd < 0) {
    err_shutdown("socket() failed");
  }

  // set socket options
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  // bind 
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(8080);
  addr.sin_addr.s_addr = ntohl(0);
  int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));

  if (rv) {
    err_shutdown("bind() failed");
  }

  rv = listen(fd, SOMAXCONN);

  if (rv) {
    err_shutdown("listen() failed");
  }


  // close the file descriptor
  close(fd);
  return 0;
}

