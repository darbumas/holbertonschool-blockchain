#include "blockchain.h"

/**
 * blockchain_destroy - Deletes an existing blockchain, along with all the
 * Blocks it contains
 * @blockchain: the blockchain structure to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);
	free(blockchain);
}
