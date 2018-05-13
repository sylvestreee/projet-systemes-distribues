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
	//request requests[NB];
};

typedef struct block block;

struct participant_node {
	int value;
	int block_node_connect[NB]; 
};

typedef struct participant_node participant_node;

struct block_node {
	int num;
	int block_node_connect[NB];
	block b[NB]; // blockchain
	//participant_node p[NB]; // participant_node which are connected to the blocknode
	request requests[NB];
};

typedef struct block_node block_node;

bool_t xdr_request(XDR * xdrs, char * request);

bool_t xdr_block(XDR *xdrs, block * block);

bool_t xdr_block_node(XDR * xdrs, block_node * bn);

#endif /* __INCLUDE_H__ */
