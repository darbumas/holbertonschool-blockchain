#include "../blockchain.h"

/**
 * transaction_destroy - Deallocates a transaction structure
 *
 * @transaction: Transaction to delete
 */
void transaction_destroy(transaction_t *transaction)
{
	if (!transaction)
		return;
	llist_destroy(transaction->inputs, 1, free);
	llist_destroy(transaction->outputs, 1, free);
	free(transaction);
}
