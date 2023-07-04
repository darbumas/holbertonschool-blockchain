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
 * block_hash - Computes the hash of a Block
 * @block: points to the Block to be hashed
 * @hash_buf: byte array to store the resulting hash
 *
 * Return: pointer to the hush buffer it is stored in.
 */
uint8_t *block_hash(block_t const *block,
		uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t len;
	uint8_t *buf;

	if (!block || !hash_buf)
		return (NULL);

	len = sizeof(block->info) + block->data.len;
	buf = calloc(1, len);
	if (!buf)
		return (NULL);

	memcpy(buf, &(block->info), sizeof(block->info));
	memcpy(buf + sizeof(block->info), block->data.buffer, block->data.len);

	compute_SHA256(buf, len, hash_buf);
	free(buf);

	return (hash_buf);
}
