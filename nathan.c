#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "types.h"

#define PROGNUM 0x20000135
#define VERSNUM 1
#define PROCNUM 1

/* on utilise les thread pour lancer à la fois un server et un client pour un seul noeud*/

void hello()
{
    printf("Hello world");
}

void quit()
{

}

void *thread_client(void *arg)
{
    printf("launching server\n");

    //..

    printf("node terminated\n");

    pthread_exit(NULL);
}

int main(void)
{
    pthread_t thread_client;

    if(pthread_create(&thread_client, NULL, thread_client, NULL) == -1)
    {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(registerrpc(PROGNUM, VERSNUM, PNUM_HELLO, hello, (xdrproc_t)xdr_void, (xdrproc_t)xdr_void) == -1)
    {
        fprintf(stderr, "unable to register 'hello' !\n");
        return EXIT_FAILURE;
    }

    printf("thread launched\n");

    svc_run();

    if(pthread_join(thread_client,NULL)==-1){
        perror("pthread_join");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}