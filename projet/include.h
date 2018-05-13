#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#define NB 10
#define LGTH 250

typedef char request[LGTH];

struct block 
{
	int depth;
	int creator;
	// hash
	request requests[NB];
};

struct participant_node {
	int value;
	int block_node_connect[NB]; 
};

typedef struct participant_node participant_node;

typedef struct block block;

struct block_node {
	int num;
	int block_node_connect[NB];
	block b[NB]; // blockchain
	//participant_node p[NB]; // participant_node which are connected to the blocknode
	request requests[NB];
};

typedef struct block_node block_node;

block * malloc_block(void);

block_node * malloc_block_node(void);

bool_t xdr_request(XDR * xdrs, char * request);

bool_t xdr_block(XDR *xdrs, block * block);

bool_t xdr_block_node(XDR * xdrs, block_node * bn);

#endif /* __INCLUDE_H__ */
