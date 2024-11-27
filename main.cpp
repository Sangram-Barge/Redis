#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

static void err_shutdown(const char *msg) {
  int err = errno;
  fprintf(stdout, "[%d] %s\n", err, msg);
  abort();  
}

static void msg(const char *msg) {
  int err = errno;
  fprintf(stderr, "[%d] %s\n", err, msg);
}

static void do_something(int conn_fd) {
  char rbuf[64] = {};
  ssize_t n = read(conn_fd, rbuf, sizeof(rbuf) - 1);
  if (n < 0) {
    msg("read() error");
    return;
  }
  printf("client says: %s\n", rbuf);

  char wbuf[] = "world\n";
  write(conn_fd, wbuf, strlen(wbuf));
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

  while(true) {
    struct sockaddr_in client_addr = {};
    socklen_t addr_len = sizeof(client_addr);
    int conn_fd = accept(fd, (struct sockaddr *) &client_addr, &addr_len);
    if(conn_fd < 0) {
      continue;
    }
    do_something(conn_fd);
    close(conn_fd);
  }

  // close the file descriptor
  close(fd);
  return 0;
}

