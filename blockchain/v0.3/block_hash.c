#include "blockchain.h"

/**
 * compute_SHA256 - Compute the SHA256 hash
 * @data: input buffer
 * @len: length of the data
 * @hash: output buffer to store the resulting hash
 */
void compute_SHA256(const uint8_t *data, size_t len,
		uint8_t hash[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX sha_ctx;

	SHA256_Init(&sha_ctx);
	SHA256_Update(&sha_ctx, data, len);
	SHA256_Final(hash, &sha_ctx);
}

/**
 * copy_tx_output - Copies transaction ID to buffer
 * @tx: pointer to transaction to copy
 * @i: index of @tx in the list
 * @arg: points to buffer to copy @tx ID to
 * Return: 0 on success, -1 on failure
 */
int copy_tx_output(llist_node_t tx, unsigned int i, void *arg)
{
	memcpy((uint8_t *)arg + i * SHA256_DIGEST_LENGTH,
			tx, SHA256_DIGEST_LENGTH);

	return (0);
}

/**
 * block_hash - Computes the hash of a Block
 * @block: points to the Block to be hashed
 * @hash_buf: byte array to store the resulting hash
 *
 * Return: pointer to the hush buffer it is stored in.
 */
uint8_t *block_hash(block_t const *block,
		uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	ssize_t len, size;
	uint8_t *buf;

	if (!block || !hash_buf)
		return (NULL);

	len = sizeof(block->info) + block->data.len;

	/* set the block size to the number of transactions */
	size = llist_size(block->transactions);
	if (size == -1)
		size = 0;

	/* add the size of each transaction to the block size */
	len += size * SHA256_DIGEST_LENGTH;

	buf = calloc(1, len);
	if (!buf)
		return (NULL);

	memcpy(buf, block, sizeof(block->info) + block->data.len);

	/* copy each transaction into the buffer */
	llist_for_each(block->transactions, copy_tx_output, (buf + len - size *
			SHA256_DIGEST_LENGTH));

	/* compute the hash of the block */
	compute_SHA256(buf, len, hash_buf);
	free(buf);

	return (hash_buf);
}
