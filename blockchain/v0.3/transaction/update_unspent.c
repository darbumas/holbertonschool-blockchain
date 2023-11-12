#include "../blockchain.h"

/**
 * unspent_find - callback to traverse unspent list and find unspent tx_out
 *
 * @node: current node being traversed
 * @hash: hash of transaction output to find
 * Return: 0 if continue else 1
 */
int unspent_find(llist_node_t node, void *hash)
{
	unspent_tx_out_t *utx = node;

	return (!memcmp(utx->out.hash, hash, SHA256_DIGEST_LENGTH));
}

/**
 * update_unspent - updates list of UTXOs
 *
 * @transactions: list of all validated transactions
 * @block_hash: hash of block containing transaction
 * @all_unspent: list of all unspent transaction outputs
 * Return: all transaction outputs from each transaction
 */
llist_t *update_unspent(llist_t *transactions,
			uint8_t block_hash[SHA256_DIGEST_LENGTH],
			llist_t *all_unspent)
{
	transaction_t *tx;
	tx_out_t *txo;
	tx_in_t *txi;
	unspent_tx_out_t *utx;
	int i, j;

	if (!transactions || !block_hash || !all_unspent)
		return (NULL);

	/* iterate through all transactions */
	for (i = 0; i < llist_size(transactions); i++)
	{
		tx = llist_get_node_at(transactions, i);
		/* iterate through all transaction inputs */
		for (j = 0; j < llist_size(tx->inputs); j++)
		{
			txi = llist_get_node_at(tx->inputs, j);
			/* find matching unspent tx output */
			llist_remove_node(all_unspent, unspent_find,
				txi->tx_out_hash, 1, free);
		}
		/* iterate through all transaction outputs */
		for (j = 0; j < llist_size(tx->outputs); j++)
		{
			txo = llist_get_node_at(tx->outputs, j);
			/* create new unspent tx output */
			utx = unspent_tx_out_create(block_hash, tx->id, txo);
			/* add new unspent tx output to list */
			llist_add_node(all_unspent, utx, ADD_NODE_REAR);
		}
	}

	return (all_unspent);
}
