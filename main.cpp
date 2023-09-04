#include "src/common/log.h"
#include <pthread.h>

void* fun(void*) 
{
    rpc::DEBUG_BLOG("多线程");
    return NULL;
}
int main()
{
    pthread_t thread;
    pthread_create(&thread, NULL, &fun, NULL); 
    rpc::DEBUG_BLOG("Args args...");
}