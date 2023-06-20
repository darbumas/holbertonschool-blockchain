#include "blockchain.h"

/**
 * block_is_valid - verifies that a Block is valid
 * @block: points to the Block to check
 * @prev_block: points to the previous Block in the Blockchain, or NULL if
 * `block` is the first Block of the chain.
 *
 * Return: 0 if Block is valid, or -1 if NOT
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t expected_hash[SHA256_DIGEST_LENGTH];
	size_t i;

	if (!block || !prev_block)
		return (-1);

	/* check if hash matches difficulty (leading 0s) */
	block_hash(block, hash);
	for (i = 0; i < block->info.difficulty; i++)
	{
		if (hash[i] != 0)
			return (-1);
	}

	/* check if prev_hash matches */
	if (memcmp(block->info.prev_hash, prev_block->hash, SHA256_DIGEST_LENGTH)
			!= 0)
		return (-1);

	/* check if index matches */
	if (block->info.index != prev_block->info.index + 1)
		return (-1);

	return (0);
}
