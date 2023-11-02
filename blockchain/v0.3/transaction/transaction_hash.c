#include "../blockchain.h"

/**
 * transaction_hash - Computes the ID (hash) of a transaction
 *
 * @transaction: Pointer to the transaction to hash
 * @hash_buf: Buffer in which to store the computed hash
 * Return: Pointer to @hash_buf, or NULL if failed
 */
uint8_t *transaction_hash(transaction_t const *transaction,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	ssize_t i, input_hash_len = 0, output_hash_len = 0, total_len = 0;
	uint8_t *hash_buffer = NULL, *buffer_ptr = NULL;
	tx_in_t *tx_in = NULL;
	tx_out_t *tx_out = NULL;

	if (!transaction || !hash_buf)
	{
		fprintf(stderr, "transaction_hash: NULL parameter(s)\n");
		return (NULL);
	}

	input_hash_len = llist_size(transaction->inputs) *
		(SHA256_DIGEST_LENGTH * 3); /* 3 = 32 + 1 + 32 */
	output_hash_len = llist_size(transaction->outputs) *
		SHA256_DIGEST_LENGTH;
	total_len = input_hash_len + output_hash_len;

	hash_buffer = calloc(1, total_len);
	if (!hash_buffer)
	{
		fprintf(stderr, "transaction_hash: calloc failed\n");
		return (NULL);
	}
	buffer_ptr = hash_buffer;

	/* Concatenate input hashes */
	for (i = 0; i < llist_size(transaction->inputs); i++)
	{
		tx_in = llist_get_node_at(transaction->inputs, i);
		memcpy(buffer_ptr, tx_in->block_hash, SHA256_DIGEST_LENGTH);
		buffer_ptr += SHA256_DIGEST_LENGTH;
		memcpy(buffer_ptr, tx_in->tx_id, SHA256_DIGEST_LENGTH);
		buffer_ptr += SHA256_DIGEST_LENGTH;
		memcpy(buffer_ptr, tx_in->tx_out_hash, SHA256_DIGEST_LENGTH);
		buffer_ptr += SHA256_DIGEST_LENGTH;
	}

	/* Concatenate output hashes */
	for (i = 0; i < llist_size(transaction->outputs); i++)
	{
		tx_out = llist_get_node_at(transaction->outputs, i);
		memcpy(buffer_ptr, tx_out->hash, SHA256_DIGEST_LENGTH);
		buffer_ptr += SHA256_DIGEST_LENGTH;
	}

	/* Compute the hash of the buffer */
	if (!sha256((const int8_t *)hash_buffer, total_len, hash_buf))
	{
		fprintf(stderr, "transaction_hash: sha256 failed\n");
		free(hash_buffer);
		return (NULL);
	}
	free(hash_buffer);
	return (hash_buf);
}
