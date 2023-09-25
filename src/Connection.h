#pragma once

#include<functional>

class Socket;
class Inetaddress;
class Eventloop;
class Channel;
class Buffer;

class Connection {
private:
    /* data */
    Socket      *conn_socket;
    Inetaddress *conn_addr;
    
    Eventloop *server_loop;
    Channel   *conn_chan;

    // disconnect. Callback the Server function and kill itself.
    std::function<void()> del_connection_cb;
    
    // The working buffer.
    Buffer *work_recvbuffer;
    Buffer *work_sendbuffer;
public:
    // _loop + addr --> a connection!!
    Connection(Eventloop *_loop, Socket *socket, Inetaddress *addr);
    ~Connection();


    Socket*      get_connsocket();
    Inetaddress* get_inetaddr(); 


    /* 
        function handling the events.
        Offer binding for the acpt Channel.
    */
    void reading_echo();

    /*
        setting the valiable.
        Called in Server after the Acceptor instance created.
    */
   void set_delconn_cb(std::function<void()> cb);
};