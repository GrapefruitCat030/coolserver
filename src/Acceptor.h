#pragma once

#include<functional>

class Socket;
class Inetaddress;
class Eventloop;
class Channel;

class Acceptor {
private:
    /* data */
    Socket      *server_socket;
    Inetaddress *server_addr;
    
    Eventloop   *server_loop;
    Channel     *acpt_chan;

    std::function<void(Socket*, Inetaddress*)> add_connection_cb;

public:
    Acceptor(Eventloop *_loop);
    ~Acceptor();

    Socket*      get_sersocket();
    Inetaddress* get_inetaddr();

    /* 
        function handling the events.
        Offer binding for the acpt Channel.
    */

    void accept_connection();
    
    /*
        setting the valiable.
        Called in Server after the Acceptor instance created.
    */
    
    void set_addconn_cb(std::function<void(Socket*, Inetaddress*)> cb);
};
