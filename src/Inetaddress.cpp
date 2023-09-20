#include "Inetaddress.h"
#include<string.h>
#include<assert.h>


Inetaddress::Inetaddress():addr_len(sizeof(sock_addr)) {
    memset(&sock_addr, 0, sizeof(sock_addr));
}

Inetaddress::Inetaddress(const char * ipaddr, uint16_t port) {
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family      = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(ipaddr); assert(sock_addr.sin_addr.s_addr != INADDR_NONE);
    sock_addr.sin_port        = htons(port);
    addr_len = sizeof(sock_addr); 
}

Inetaddress::~Inetaddress() {}

