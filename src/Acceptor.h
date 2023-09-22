#pragma once

#include<functional>

class Eventloop;
class Socket;
class Inetaddress;

class Acceptor {
private:
    /* data */
    Eventloop   *server_loop;
    Socket      *server_socket;
    Inetaddress *server_addr;
    std::function<void()> accept_connection_cb;

public:
    Acceptor(Eventloop *_loop);
    ~Acceptor();

    Socket*      get_sersocket();
    Inetaddress* get_inetaddr();

    void set_acptconn_cb(std::function<void()> cb);
    void accept_connection();
};
