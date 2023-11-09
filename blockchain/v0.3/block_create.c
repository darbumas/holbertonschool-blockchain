#include "blockchain.h"
#include <time.h>

/**
 * block_create - Creates a block structure and initializes it.
 * @prev: pointer to the previous Block in the Blockchain.
 * @data: points to a memory area to duplicate in the Block's data.
 * @data_len: stores the number of bytes to duplicate in data
 *
 * Note: if data_len is bigger than BLOCKCHAIN_DATA_MAX, then only
 * BLOCKCHAIN_DATA_MAX bytes must be duplicated.
 *
 * Return: pointer to the allocated Block
 */
block_t *block_create(block_t const *prev, int8_t const *data,
		uint32_t data_len)
{
	block_t *block = NULL;
	uint32_t data_len_cpy =
		data_len > BLOCKCHAIN_DATA_MAX ? BLOCKCHAIN_DATA_MAX : data_len;

	if (!prev || !data)
		return (NULL);

	block = malloc(sizeof(block_t));
	if (!block)
		return (NULL);

	block->transactions = llist_create(MT_SUPPORT_FALSE);
	if (!block->transactions)
		return (free(block), NULL);

	/* Init the Block */
	block->info.index = prev->info.index + 1;
	block->info.difficulty = 0;
	block->info.nonce = 0;
	block->info.timestamp = (uint64_t)time(NULL);

	/* Copy previous Block's hash in the current Block's prev_hash field */
	memcpy(block->info.prev_hash, prev->hash, SHA256_DIGEST_LENGTH);

	/* Copy data */
	memcpy(block->data.buffer, data, data_len_cpy);
	block->data.buffer[data_len_cpy] = '\0';
	block->data.len = data_len_cpy;

	/* Zero out the Block's hash */
	memset(block->hash, 0, SHA256_DIGEST_LENGTH);

	return (block);
}
