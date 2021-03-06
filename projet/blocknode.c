#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "include.h"

block_node * block_n = NULL;

/* on utilise les thread pour lancer à la fois un serveur et un client pour un seul noeud*/

void printf_requests(request r)
{
	printf("Sender : %d, Entitle : %d, Receiver : %d\n",
		r.sender,r.entitle,r.receiver);
}

void printf_block(block b)
{
	int i;
	printf("Depth : %d, Creator : %d\n",
		b.depth,b.creator);
	for(i = 0; i < 10; i++)
		printf_requests(b.requests[i]);
}

void printf_block_node(block_node * bn)
{	
	int i = 0;
	printf("Numero : %d\n",bn->num);
	printf("Block node connect :");
	for(i = 0; i < 10; i++)
		printf("-%d",bn->block_node_connect[i]);
	printf("\n");
	for(i = 0; i < 10; i++)
		printf_block(bn->b[i]);
	for(i = 0; i < 10; i++)
		printf_requests(bn->requests[i]);
}

block initialize_block(block bl)
{
    int i;
    bl.depth = -1;
    bl.creator = -1;
    for(i = 0; i < 10; i++)
    {
        bl.requests[i].sender = -1;
        bl.requests[i].entitle = -1;
        bl.requests[i].receiver = -1;
    }
    return bl;
}

// renvoie le nombre de requêtes en attente
int request_number(block_node *bn)
{
    int i = 0;
    while(bn->requests[i].sender != -1)
    {
        i++;
    }
    return i;
}

// renvoie le nombre de blocs
int block_number(block_node *bn)
{
    int i = 0;
    while(bn->b[i].creator != -1)
    {
        i++;
    }
    return i;
}

// renvoie le nombre de participants inscrits
int participant_number(block_node *bn)
{
    int i = 0;
    while(bn->pn[i] != -1) // parcours de la blockchain
    { 
        i++;
    }
    return i;
}


block_node * create_block(block_node * block_n)
{	
	enum clnt_stat stat;
	static int res;	
	static block_node * bn = NULL;
	static transmission * trans;
	trans = (transmission *) malloc(sizeof(transmission));

	bn = block_n;
	//printf("create block %d\n", bn->num);
	int i = 0, length = block_number(bn), length2;	
	block bl = initialize_block(bl);
    
	// empty blockchain
	if(bn->b[0].creator == -1)
	{
		bl.depth = 0;
		bl.creator = bn->num;
		// hash
		while(bn->requests[i].sender != -1) // remplir le tableau du bloc / vider le tableau du noeud
		{
			length2 = participant_number(bn);
			bl.requests[i].sender = bn->requests[i].sender;
			bl.requests[i].entitle = bn->requests[i].entitle;
			bl.requests[i].receiver = bn->requests[i].receiver;
			bn->requests[i].sender = -1;
			bn->requests[i].entitle = -1;
			bn->requests[i].receiver = -1;
			if(bn->requests[i].entitle == -1)
			{
				bn->pn[length2] = bn->requests[i].sender;
			}
			i++;
		}
		bn->b[0] = bl;
	}
	// blockchain's not full
	if(length >= 1 && length < 10)
	{
		bl.depth = length;
		bl.creator = bn->num;
		// hash
		while(bn->requests[i].sender != -1)
		{
			length2 = participant_number(bn);
			bl.requests[i].sender = bn->requests[i].sender;
            bl.requests[i].entitle = bn->requests[i].entitle;
            bl.requests[i].receiver = bn->requests[i].receiver;
            bn->requests[i].sender = -1;
            bn->requests[i].entitle = -1;
            bn->requests[i].receiver = -1;
            if(bn->requests[i].entitle == -1)
			{
				bn->pn[length2] = bn->requests[i].sender;
			}
			i++;
		}
		bn->b[length] = bl;
	}

	// blockchain's full
	else if(length == 10)
	{
		return bn;
	}

	// transmit_block pour chaque noeud voisin dans block_node_connect
	for(i = 0; i < 10; i++)
	{
		if(bn->block_node_connect[i] != 0)
		{
			trans->bn = bn;
			trans->q = length;
			stat = callrpc	("localhost",bn->block_node_connect[i],
				bn->block_node_connect[i],2,
        	            (xdrproc_t)xdr_transmission,(char *)trans,
        	            (xdrproc_t)xdr_int,(char *)&res) ;
		
			if (stat != RPC_SUCCESS)
        	        {
        	            	fprintf(stderr, "Echec de l'appel distant\n");
        	            	clnt_perrno(stat);
        	            	fprintf(stderr, "\n");
				pthread_exit(NULL);
        	        }
			if(res == 0)
			{
				printf("Transmission block number %d to block_node %d worked\n",
					length,bn->block_node_connect[i]);
			}				
			else
			{
				printf("Transmission block number %d to block_node %d not worked\n",
				length,bn->block_node_connect[i]);
			}
		}
	}
	// transmit_blockchain_points pour chaque participant dans pn

	printf("create block in block_node %d and block %d\n", bn->num, length);
	fflush(stdout);
	return bn;	
}

/* quand on reçoit une demande de bloc (ou d'une chaîne de bloc) d'un noeud bloc voisin :
 * - soit le tableau de requêtes n'est pas plein donc on ajoute la demande (0)
 * - soit le tableau de requêtes est plein donc on refuse la demande (-1)
 */
int *ask_for_blocks(int num)
{
    block_node *bn = block_n;
    static int t = 0, f = -1;
    int length_r = request_number(bn);
    if(length_r < 10)
    {
        bn->requests[length_r].sender = num;
        bn->requests[length_r].entitle = 0;
        bn->requests[length_r].receiver = bn->num;
    }
    else
    {
        return &f;
    }
    return &t;
}

/* quand on reçoit une transmission d'un bloc :
 * - soit le noeud bloc n'a pas atteint la limite de bloc donc on ajoute le bloc (0)
 * - soit le noeud bloc a atteint la limite de bloc donc on n'ajoute pas le bloc (-1)
 */
int *transmit_blocks(transmission * trans)
{
    block_node *bn = block_n;
    static int t = 0, f = -1;
    int i = 0, j = 0, length = block_number(bn), length2;
    block bl = trans->bn->b[trans->q];
    if(length < 10)
    {
        if(length == bl.depth) // empêcher un noeud bloc de recevoir un bloc d'une profondeur qu'il a déjà
        {
            bn->b[length] = bl;
            while(bl.requests[i].sender != -1) // parcours des requêtes présentes dans le bloc
            {
                for(j = 0; j < request_number(bn); j++) // parcours des requêtes en attente du noeud bloc
                {
                    if(bl.requests[i].sender == bn->requests[j].sender &&
                       bl.requests[i].entitle == bn->requests[j].entitle &&
                       bl.requests[i].receiver == bn->requests[j].receiver) // correspondance trouvée
                    {
                        bn->requests[j].sender = -1;
                        bn->requests[j].entitle = -1;
                        bn->requests[j].receiver = -1;
                        break;
                    }
                }
                i++;        
            }
        }
    }
    else if(length == 10)
    {
        return &f;
    }
    block_n = bn;
    printf("Receive block from block node %d\n",trans->bn->num);
    return &t;
}

// transmit_blockchain

/* quand on reçoit une requête d'un noeud bloc voisin :
 * - soit le tableau de requêtes n'est pas plein donc on ajoute la requête (0)
 * - soit le tableau de requêtes est plein donc on refuse la requête (-1)
 */
int *transmit_requests(transmission *trans)
{
    static int t = 0, f = -1;
    int length = request_number(block_n);
    if(length < 10)
    {
        block_n->requests[length] = trans->bn->requests[trans->q];
    }
    else if(length == 10)
    {
        return &f;
    }
    return &t;
}

/* quand on reçoit une demande d'inscription d'un noeud participant :
 * - soit le noeud bloc n'a pas atteint la limite de participants et son tableau de requêtes n'est pas plein
     donc on ajoute la demande (0)
 * - soit l'une des deux conditions n'est pas vérifiée donc on refuse la demande (-1)
 */
int *ask_for_inscription(int num)
{
    //block_node *bn = block_n;
    static int t = 0, f = -1;
    int length_p = participant_number(block_n), length_r = request_number(block_n);
    if(length_p < 10 && length_r < 10)
    {
        block_n->requests[length_r].sender = num;
        block_n->requests[length_r].entitle = -1;
        block_n->requests[length_r].receiver = block_n->num;
    }
    else
    {
        return &f;
    }
    return &t;
}

// renvoie la liste des participants
/*block *get_participant()
{
    return block_n->pn;
}*/

void *node(void *arg)
{
    printf("launching server\n");

    block_node * bn = (block_node *) arg;
	
	int i, b_neigboorh, ask;
	enum clnt_stat stat;
	static int res;	
	static block_node * bn_res;
	static transmission * t;
	bn_res = (block_node*) malloc(sizeof(block_node));
	bn_res = bn;
	t = (transmission *) malloc(sizeof(transmission));

    while(1)
    {
        printf(">");
        scanf("%d",&ask);
        switch(ask)
        {
            case 0:
		stat = callrpc("localhost",bn->num,bn->num,1,
                    	(xdrproc_t)xdr_block_node,(char *)bn,
                    	(xdrproc_t)xdr_block_node,(char *)bn_res) ;

                if (stat != RPC_SUCCESS)
                {
                    	fprintf(stderr, "Echec de l'appel distant\n");
                    	clnt_perrno(stat);
			fprintf(stderr, "\n");
			pthread_exit(NULL);
                }

		/*printf("-------------BLOCK_NODE BN_RES----------------\n");
		printf_block_node(bn_res);
		
		printf("-------------BLOCK_NODE BN----------------\n");
		printf_block_node(bn);*/

		bn = bn_res;
		block_n = bn;
                break;
            case 1:
		b_neigboorh = -1;
		while(b_neigboorh == -1 || b_neigboorh == 0)
		{
			printf("Choose neigboorh block : ");
			scanf("%d",&ask);		
			for(i = 0; i < 10; i++)
			{
				if(ask == bn->block_node_connect[i])
					b_neigboorh = ask;
			}
		}

		printf("Transmit block : choose block between 0 and 9: ");
		scanf("%d",&ask);
		while(ask < 0 || ask > 10)
		{
			printf("choose block between 0 and 9 : ");
			scanf("%d",&ask);
		}

		t->bn = bn;
		t->q = ask;
		stat = callrpc	("localhost",b_neigboorh,b_neigboorh,2,
                    (xdrproc_t)xdr_transmission,(char *)t,
                    (xdrproc_t)xdr_int,(char *)&res) ;
		
		if (stat != RPC_SUCCESS)
                {
                    	fprintf(stderr, "Echec de l'appel distant\n");
                    	clnt_perrno(stat);
                    	fprintf(stderr, "\n");
			pthread_exit(NULL);
                }
		if(res == 0)
			printf("Transmission block number %d to block_node %d worked\n",ask,b_neigboorh);
		else
			printf("Transmission block number %d to block_node %d not worked\n",ask,b_neigboorh);
		break;
	case 2:
		break;
	case 3:
		pthread_exit(NULL);
                break;
            default:
		printf_block_node(block_n);
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
    	block bl = initialize_block(bl);

	if(argc < 3 || argc >= 12)
	{
		printf("Too few/many argument\n");
		return 0;
	}
	
	//Initialisation block_node
	block_n = (block_node *) malloc(sizeof(block_node));
	block_n->num = atoi(argv[1]);
	
	PROGNUM = block_n->num;
	VERSNUM = block_n->num;

	for(i = 2; i < argc; i++)
	{
		block_n->block_node_connect[i-2] = atoi(argv[i]);
	}

	for(i = 0; i<10; i++)
	{
		block_n->b[i] = bl;
		block_n->requests[i].sender = -1;
		block_n->requests[i].entitle = -1;
		block_n->requests[i].receiver = -1;
		block_n->pn[i] = -1;
	}

    
	if(pthread_create(&thread_client, NULL, node, (void *)block_n) == -1)
	{
		perror("pthread_create");
		return EXIT_FAILURE;
	}

	if(registerrpc(PROGNUM, VERSNUM, 1, create_block, 	
		(xdrproc_t)xdr_block_node, 
		(xdrproc_t)xdr_block_node) == -1)
	{
	        fprintf(stderr, "unable to register 'create_block' !\n");
	        return EXIT_FAILURE;
	}
	if(registerrpc(PROGNUM,VERSNUM, 2, transmit_blocks,
		(xdrproc_t)xdr_transmission, (xdrproc_t)xdr_int) == -1)
	{
		fprintf(stderr, "unable to register 'transmit_block' !\n");
	        return EXIT_FAILURE;
	}
	if(registerrpc(PROGNUM,VERSNUM, 3, transmit_requests,
		(xdrproc_t)xdr_transmission, (xdrproc_t)xdr_int) == -1)
	{
		fprintf(stderr, "unable to register 'transmit_requests' !\n");
	        return EXIT_FAILURE;
	}
	if(registerrpc(PROGNUM,VERSNUM, 4, ask_for_inscription,
		(xdrproc_t)xdr_int, (xdrproc_t)xdr_int) == -1)
	{
		fprintf(stderr, "unable to register 'ask_for_inscription' !\n");
	        return EXIT_FAILURE;
	}
	if(registerrpc(PROGNUM,VERSNUM, 5, ask_for_blocks,
		(xdrproc_t)xdr_int, (xdrproc_t)xdr_int) == -1)
	{
		fprintf(stderr, "unable to register 'ask_for_blocks' !\n");
	        return EXIT_FAILURE;
	}
	printf("thread launched\n");
	svc_run();

	if(pthread_join(thread_client,NULL)==-1)
	{
		perror("pthread_join");
	        return EXIT_FAILURE;
	}

    	return EXIT_SUCCESS;
}
