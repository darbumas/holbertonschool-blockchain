#include "blockchain.h"

/**
 * blockchain_destroy - Deletes an existing blockchain, along with all the
 * Blocks it contains.
 * @blockchain: the blockchain structure to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;
	llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);

	/* Free the list of unspent transaction outputs */
	if (blockchain->unspent)
		llist_destroy(blockchain->unspent, 1, NULL);

	free(blockchain);
}
