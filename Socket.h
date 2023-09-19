#ifndef SOCKET_H
#define SOCKET_H


//the socket's address saved in the Inetaddress class.
class Inetaddress; 

class Socket {
private:
    /* data */
    int socket_fd;
public:
    // 默认创建一个TCP socket.
    Socket();
    Socket(int fd);
    Socket(int domain, int type, int protocol);
    ~Socket();

    /* server */

    void s_bind(Inetaddress *s_addr);
    void s_listen();
    //   不直接返回Socket，不破坏accept接口
    int  s_accept(Inetaddress *s_addr); 
    

    /* client */

    void s_connect(int fd, Inetaddress *addr);


    /* utility */
    int getfd();
    int s_setnonblocking();


    bool isSocket(int fd);

};


#endif