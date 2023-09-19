#ifndef INETADDRESS_H
#define INETADDRESS_H

#include<arpa/inet.h>

class Inetaddress {
public:
    /* data */
    struct sockaddr_in sock_addr;
    socklen_t addr_len; 
    
    Inetaddress();
    Inetaddress(const char * addr, uint16_t port);
    ~Inetaddress();
};


#endif