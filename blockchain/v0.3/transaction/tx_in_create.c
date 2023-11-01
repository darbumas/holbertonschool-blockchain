#include "../blockchain.h"

/**
 * tx_in_create - Allocates and initializes a transaction input structure
 *
 * @unspent: Pointer to the unspent transaction output to be converted to a
 * transaction input
 * Return: Pointer to the created transaction input upon success, or NULL
 */
tx_in_t *tx_in_create(unspent_tx_out_t const *unspent)
{
	tx_in_t *tx_in = calloc(1, sizeof(*tx_in));

	if (!tx_in)
	{
		fprintf(stderr, "tx_in_create: calloc failed\n");
		return (NULL);
	}

	memcpy(tx_in->block_hash, unspent->block_hash,
		sizeof(tx_in->block_hash));
	memcpy(tx_in->tx_id, unspent->tx_id, sizeof(tx_in->tx_id));
	memcpy(tx_in->tx_out_hash, unspent->out.hash,
		sizeof(tx_in->tx_out_hash));

	return (tx_in);
}
