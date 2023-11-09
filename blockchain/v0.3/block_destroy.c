#include "blockchain.h"

/**
 * block_destroy - Deletes an existing Block
 * @block: Block to delete
 */
void block_destroy(block_t *block)
{
	/* destroy all the transaction in the block */
	llist_destroy(block->transactions, 1, (node_dtor_t)transaction_destroy);
	free(block);
}
