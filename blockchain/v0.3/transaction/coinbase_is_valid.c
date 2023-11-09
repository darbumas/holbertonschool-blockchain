#include "../blockchain.h"

/**
 * coinbase_is_valid - Verifies if a coinbase transaction is valid
 *
 * @coinbase: points to the coinbase transaction to verify
 * @block_index: the index of the Block the coinbase transaction belongs to
 * Return: 1 if valid coinbase transaction, 0 otherwise
 */
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
	size_t i = 0;
	tx_in_t *txi;
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];

	if (!coinbase)
		return (0);

	/* check integrity */
	if (!transaction_hash(coinbase, hash_buf) ||
		memcmp(hash_buf, coinbase->id, SHA256_DIGEST_LENGTH))
		return (0);
	/* Transaction must only contain 1 input */
	if (llist_size(coinbase->inputs) != 1)
		return (0);
	/* First (and only) input must have its coinbase field set to 1 */
	txi = llist_get_node_at(coinbase->inputs, 0);
	if (!txi || !txi->tx_out_hash ||
		memcmp(txi->tx_out_hash, &block_index, 4))
		return (0);
	/* Transaction must only contain 1 output */
	if (llist_size(coinbase->outputs) != 1)
		return (0);
	/* First (and only) output must have a valid amount */
	if (!coinbase->outputs)
		return (0);

	/* All other fields can be set to anything */
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		if (txi->block_hash[i] || txi->tx_id[i])
			return (0);
	}
	for (i = 0; i < SIG_MAX_LEN; i++)
	{
		if (txi->sig.sig[i])
			return (0);
	}
	return (1);
}
