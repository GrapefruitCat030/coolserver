#ifndef TEST_UTIL_H
#define TEST_UTIL_H


// 定义一个宏，用于将行号转换为字符串
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
// 在报错errmsg后面加入此宏，即可实现行数报错
#define LINEABORT ", line " TOSTRING(__LINE__)


/*  The input should be the equation that represents the error,
    like "bind(arg..) == -1", if the function's return-code is -1,
    error report and abort.
*/
void errif(bool condition, const char *errmsg);

/*
    Add the O_NONBLOCK flag to the fd.
    the return-value is the old option value of fd, offering recovery.
*/
int setnonblocking(int fd); 




#endif  //TEST_UTIL_H