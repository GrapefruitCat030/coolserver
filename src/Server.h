#pragma once

/*
    Server类负责的事情：
    注册头领socket：listen socket 并把它注册到事件请求队列上
    包含各种业务处理函数，如listen socket的connect业务、其余socket的正常处理业务
*/

class Socket;
class Eventloop;
class Acceptor;

class Server {
private:
    /* data */

    //  服务器的事件请求队列
    Eventloop *server_loop;
    // Acceptor module
    Acceptor *server_accpetor;
public:
    // 负责创建listen socket并注册到所属的事件表中
    Server(Eventloop *);
    ~Server();

    /* handle function*/
    void accpet_connection(Socket *server_sock);
    void reading_handle(int conn_sockfd);


};
