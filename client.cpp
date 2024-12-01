#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

static void err_shutdown(const char *msg) {
    int error = errno;
    fprintf(stderr, "[%d] %s\n", error, msg);
    abort();
}

static void msg(const char *msg) {
  fprintf(stdout, "%s\n", msg);
}

const size_t max_msize = 4096;

static int32_t write_all(int fd, const char *buf, size_t n) {
  while (n > 0) {
    ssize_t rv = write(fd, buf, n);
    if (rv <= 0) {
      return -1;
    }
    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }
  return 0;
}

static int32_t read_full(int fd, char *buf, size_t n) {
  while(n > 0) {
    int rv = read(fd, buf, n);
    if (rv <= 0) {
      return -1;
    }
    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf +=rv;
  }
  return 0;
}

static int32_t query(int fd, const char *text) {
  uint32_t len = (uint32_t) sizeof(text);
  if (len < max_msize) {
    return -1;
  }

  char wbuf[4 + max_msize];
  memcpy(wbuf, &len, 4);
  memcpy(&wbuf[4], text, len);

  if (int32_t err = write_all(fd, wbuf, 4 + len)) {
    return -1;
  }

  char rbuf[4 + max_msize + 1];
  errno = 0;
  int32_t err = read_full(fd, rbuf, 4);

  if (err) {
    if (errno == 0) {
      msg("EOF");
    } else {
      msg("read() failed");
    }
    return err;
  }

  memcpy(&len, rbuf, 4);
  if (len > max_msize) {
    msg("too long");
    return -1;
  }

  err = read_full(fd, &rbuf[4], len);
  
  if (err) {
    msg("read() error");
    return -1;
  }

  fprintf(stdout, "server says : %s\n", &rbuf[4]);
  return 0;
}

int main() {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        err_shutdown("socket() failed");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(8080);
    addr.sin_addr.s_addr = ntohs(0);

    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
      err_shutdown("connect() failed");
    }

    int32_t err = query(fd, "hello");
    err = query(fd, "hello1");
    err = query(fd, "hello2");

    close(fd);
    return 0;
}
     
