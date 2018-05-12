#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


typedef struct block {
	
};

typedef struct block_node {
	int num;
	int block_node_connect[10];
	struct block b[10];
	char * requests[10];
};


/* on utilise les thread pour lancer à la fois un server et un client pour un seul noeud*/

int * hello(void)
{
    static int r = 0;
    printf("Hello world\n");
    fflush(stdout);
    return &r;
}

void *node(void *arg)
{
    printf("launching server\n");

    int stop = 0;
    int ask;
    enum clnt_stat stat ;
    static int res;

    while(stop != 1)
    {
        printf(">");
        scanf("%d",&ask);
        switch(ask)
        {
            case 0:
				scanf("%d",&ask);
                stat = callrpc("localhost",
                   	/*PROGNUM, VERSNUM, PROCNUM,*/ask,ask,ask,
                    (xdrproc_t)xdr_void, (void *)0,
                    (xdrproc_t)xdr_int, (char *)&res) ;

                if (stat != RPC_SUCCESS)
                {
                    fprintf(stderr, "Echec de l'appel distant\n") ;
                    clnt_perrno(stat) ;
                    fprintf(stderr, "\n") ;
                    //return 1 ;
                }
                break;
            case 1:
                stop = 1;
                break;
            default:
                break;
        }
        printf("\n");
    }
    
    pthread_exit(NULL);
}

int main(int argc, char ** argv)
{
    pthread_t thread_client;
	int i = 0;
	int PROGNUM;
	int VERSNUM;
	int PROCNUM;

	if(argc < 3 || argc >= 12)
	{
		printf("To few argument\n");
		return 0;
	}
	
	struct block_node bn;
	bn.num = atoi(argv[1]);

	PROGNUM = bn.num;
	VERSNUM = bn.num;
	PROCNUM = bn.num;

	for(i = 2; i < argc; i++)
	{
		bn.block_node_connect[i-2] = atoi(argv[i]);
	}
	printf("%d, %d, %d\n",PROGNUM,VERSNUM,PROCNUM);
    
    if(pthread_create(&thread_client, NULL, node, NULL) == -1){
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(registerrpc(PROGNUM, VERSNUM, PROCNUM, hello, (xdrproc_t)xdr_void, 
		(xdrproc_t)xdr_int) == -1){
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
