#include "include.h"

bool_t
xdr_request (XDR *xdrs, request *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->sender))
		 return FALSE;
	  if (!xdr_pointer (xdrs, (char **)&objp->entitle, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->receiver))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_block (XDR *xdrs, block *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->depth))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->creator))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->requests, 10,
		sizeof (request), (xdrproc_t) xdr_request))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_participant_node (XDR *xdrs, participant_node *objp)
{
	register int32_t *buf;

	int i;

	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, (1 +  10 )* BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->value))
				 return FALSE;
			 if (!xdr_vector (xdrs, (char *)objp->block_node_connect, 10,
				sizeof (int), (xdrproc_t) xdr_int))
				 return FALSE;
		} else {
			IXDR_PUT_LONG(buf, objp->value);
			{
				register int *genp;

				for (i = 0, genp = objp->block_node_connect;
					i < 10; ++i) {
					IXDR_PUT_LONG(buf, *genp++);
				}
			}
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, (1 +  10 )* BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->value))
				 return FALSE;
			 if (!xdr_vector (xdrs, (char *)objp->block_node_connect, 10,
				sizeof (int), (xdrproc_t) xdr_int))
				 return FALSE;
		} else {
			objp->value = IXDR_GET_LONG(buf);
			{
				register int *genp;

				for (i = 0, genp = objp->block_node_connect;
					i < 10; ++i) {
					*genp++ = IXDR_GET_LONG(buf);
				}
			}
		}
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->value))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->block_node_connect, 10,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_block_node (XDR *xdrs, block_node *objp)
{
	register int32_t *buf;

	int i;

	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, (1 + ( 10 )) * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->num))
				 return FALSE;
			 if (!xdr_vector (xdrs, (char *)objp->block_node_connect, 10,
				sizeof (int), (xdrproc_t) xdr_int))
				 return FALSE;

		} else {
		IXDR_PUT_LONG(buf, objp->num);
		{
			register int *genp;

			for (i = 0, genp = objp->block_node_connect;
				i < 10; ++i) {
				IXDR_PUT_LONG(buf, *genp++);
			}
		}
		}
		 if (!xdr_vector (xdrs, (char *)objp->b, 10,
			sizeof (block), (xdrproc_t) xdr_block))
			 return FALSE;
		 if (!xdr_vector (xdrs, (char *)objp->p, 10,
			sizeof (participant_node), (xdrproc_t) xdr_participant_node))
			 return FALSE;
		 if (!xdr_vector (xdrs, (char *)objp->requests, 10,
			sizeof (request), (xdrproc_t) xdr_request))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, (1 + ( 10 )) * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->num))
				 return FALSE;
			 if (!xdr_vector (xdrs, (char *)objp->block_node_connect, 10,
				sizeof (int), (xdrproc_t) xdr_int))
				 return FALSE;

		} else {
		objp->num = IXDR_GET_LONG(buf);
		{
			register int *genp;

			for (i = 0, genp = objp->block_node_connect;
				i < 10; ++i) {
				*genp++ = IXDR_GET_LONG(buf);
			}
		}
		}
		 if (!xdr_vector (xdrs, (char *)objp->b, 10,
			sizeof (block), (xdrproc_t) xdr_block))
			 return FALSE;
		 if (!xdr_vector (xdrs, (char *)objp->p, 10,
			sizeof (participant_node), (xdrproc_t) xdr_participant_node))
			 return FALSE;
		 if (!xdr_vector (xdrs, (char *)objp->requests, 10,
			sizeof (request), (xdrproc_t) xdr_request))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->num))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->block_node_connect, 10,
		sizeof (int), (xdrproc_t) xdr_int))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->b, 10,
		sizeof (block), (xdrproc_t) xdr_block))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->p, 10,
		sizeof (participant_node), (xdrproc_t) xdr_participant_node))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->requests, 10,
		sizeof (request), (xdrproc_t) xdr_request))
		 return FALSE;
	return TRUE;
}
