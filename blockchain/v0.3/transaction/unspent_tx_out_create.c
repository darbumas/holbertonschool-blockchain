#include "../blockchain.h"

/**
 * unspent_tx_out_create - Creates an unspent transaction output structure
 *
 * @block_hash: Hash of the Block containing the transaction
 * @tx_id: Hash of a transaction where the referenced output is located
 * @out: Address of the referenced output in the transaction tx_id
 * Return: Pointer to the created unspent transaction output upon success, or
 * NULL upon failure
 */
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH], tx_out_t const *out)
{
	unspent_tx_out_t *utx = calloc(1, sizeof(*utx));

	if (!utx)
	{
		fprintf(stderr, "unspent_tx_out_create: calloc failed\n");
		return (NULL);
	}

	memcpy(utx->block_hash, block_hash, SHA256_DIGEST_LENGTH);
	memcpy(utx->tx_id, tx_id, SHA256_DIGEST_LENGTH);
	memcpy(&utx->out, out, sizeof(utx->out));

	return (utx);
}
