#include "../blockchain.h"
#include <stdbool.h>

/**
 * create_hash_buffer - Helper function creates a buffer to store the computed
 * hash
 *
 * @transaction: pointer to the transaction to be hashed
 * @input_hash_len: pointer to store the length of the inputs hash
 * @output_hash_len: pointer to store the length of the outputs hash
 * @total_size: pointer to store the total size of the buffer
 * Return: pointer to the created buffer
 */
uint8_t *create_hash_buffer(transaction_t const *transaction,
	size_t *input_hash_len, size_t *output_hash_len, size_t *total_size)
{
	uint8_t *buffer;

	*input_hash_len = llist_size(transaction->inputs) *
		(SHA256_DIGEST_LENGTH * 3); /* 3 = 32 + 1 + 32 */
	*output_hash_len = llist_size(transaction->outputs) *
		SHA256_DIGEST_LENGTH;
	*total_size = *input_hash_len + *output_hash_len;

	buffer = calloc(1, *total_size);

	return (buffer);
}

/**
 * copy_inputs_hashes - Helper function copies the hash of each input into the
 * buffer
 *
 * @transaction: pointer to the transaction to be hashed
 * @buffer_ptr: pointer to the buffer to store the hash
 */
void copy_inputs_hashes(transaction_t const *transaction, uint8_t *buffer_ptr)
{
	ssize_t i;
	tx_in_t *tx_in;

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
}

/**
 * copy_outputs_hashes - Helper function copies the hash of each output into
 * the buffer
 *
 * @transaction: pointer to the transaction to be hashed
 * @buffer_ptr: pointer to the buffer to store the hash
 * @input_hash_len: length of the inputs hash
 */
void copy_outputs_hashes(transaction_t const *transaction, uint8_t *buffer_ptr,
	size_t input_hash_len)
{
	ssize_t i;
	tx_out_t *tx_out;

	for (i = 0; i < llist_size(transaction->outputs); i++)
	{
		tx_out = llist_get_node_at(transaction->outputs, i);
		memcpy(buffer_ptr + input_hash_len, tx_out->hash,
			SHA256_DIGEST_LENGTH);
		input_hash_len += SHA256_DIGEST_LENGTH;
	}
}

/**
 * compute_tx_hash - Helper function computes the hash of a transaction
 *
 * @hash_buffer: pointer to the buffer to store the hash
 * @total_size: total size of the buffer
 * @hash_buf: p
 * Return: true if the hash was computed, false otherwise
 */
bool compute_tx_hash(const uint8_t *hash_buffer, size_t total_size,
	uint8_t *hash_buf)
{
	if (!sha256((const int8_t *)hash_buffer, total_size, hash_buf))
		return (false);
	return (true);
}

/**
 * transaction_hash - Computes the ID (hash) of a transaction
 *
 * @transaction: pointer to the transaction to be hashed
 * @hash_buf: pointer to store the computed hash
 * Return: pointer to @hash_buf, or NULL if failed
 */
uint8_t *transaction_hash(transaction_t const *transaction,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	uint8_t *hash_buffer = NULL;
	size_t input_hash_len, output_hash_len, total_size;

	if (!transaction || !hash_buf)
	{
		fprintf(stderr, "transaction_hash: NULL parameter(s)\n");
		return (NULL);
	}

	hash_buffer = create_hash_buffer(transaction, &input_hash_len,
		&output_hash_len, &total_size);
	if (!hash_buffer)
	{
		fprintf(stderr, "transaction_hash: calloc failed\n");
		return (NULL);
	}

	copy_inputs_hashes(transaction, hash_buffer);
	copy_outputs_hashes(transaction, hash_buffer, input_hash_len);

	if (!compute_tx_hash(hash_buffer, total_size, hash_buf))
	{
		fprintf(stderr, "transaction_hash: sha256 failed\n");
		free(hash_buffer);
		return (NULL);
	}

	free(hash_buffer);
	return (hash_buf);
}
