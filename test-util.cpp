#include "test-util.h"
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>



void errif(bool condition, const char *errmsg) {
    if(condition) { // success condition is 0.
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

int setnoblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}





