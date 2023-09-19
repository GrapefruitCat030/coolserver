#include "Inetaddress.h"
#include "Socket.h"

#include "test-util.h"

#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>

Socket::Socket() : socket_fd(-1) {
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    errif(socket_fd == -1, "socket inital:something wrong");

}
Socket::Socket(int fd) : socket_fd(fd) {
    errif(isSocket(socket_fd) == false, "socket inital:not a socket");
}
Socket::Socket(int domain, int type, int protocol) {
    socket_fd = socket(domain, type, protocol);
    errif(socket_fd == -1, "socket inital:something wrong");
}

Socket::~Socket() {
    if(socket_fd != -1) {
        close(socket_fd);
        socket_fd = -1; 
    }
}


/* server */

void Socket::s_bind(Inetaddress *s_addr) {
    errif(bind(socket_fd, (struct sockaddr *)&(s_addr->sock_addr), s_addr->addr_len) == -1, \
            "s_bind error");
}

void Socket::s_listen() {
    errif(listen(socket_fd, SOMAXCONN) == -1, \
            "s_listen error");
}

int Socket::s_accept(Inetaddress *s_addr) {
    int conn_fd = accept(socket_fd, (struct sockaddr *)&(s_addr->sock_addr), &(s_addr->addr_len));
    errif(conn_fd == -1, \
            "s_accept error");
    return conn_fd;
}

/* client */

void Socket::s_connect(int fd, Inetaddress *addr) {
    errif(connect(fd, (struct sockaddr *)&(addr->sock_addr), addr->addr_len) == -1, \
            "s_connect error");
}


/* utility */

int Socket::getfd() { 
    return socket_fd;
}

int Socket::s_setnonblocking() {
    int old_option = setnonblocking(socket_fd);
    return old_option;
}

bool Socket::isSocket(int fd) {
    struct stat fileStat;
    if (fstat(fd, &fileStat) == -1) {
        // 处理错误情况
        return false;
    }
    return S_ISSOCK(fileStat.st_mode);
}
