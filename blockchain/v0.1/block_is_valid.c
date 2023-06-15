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

	/* check for NULL block or illegal data length */
	if (!block || block->data.len > BLOCKCHAIN_DATA_MAX ||
			block->data.len == 0)
		return (-1);

	/* check for genesis block */
	if (block->info.index == 0)
	{
		if (prev_block || memcmp(block, &GENESIS_BLK,
					sizeof(block_t)) != 0)
			return (-1);
	}
	else
	{ /* check for NULL prev_block or incorrect index */
		if (!prev_block || block->info.index !=
				prev_block->info.index + 1)
			return (-1);
	/* check if the prev_block's hash matches the reference in block */
		if (memcmp(prev_block->hash, block->info.prev_hash,
					SHA256_DIGEST_LENGTH) != 0)
			return (-1);
	}

	/* check if block's hash matches the expected one */
	block_hash(block, expected_hash);
	if (memcmp(expected_hash, block->hash, SHA256_DIGEST_LENGTH) != 0)
		return (-1);

	/* TODO: Add verification of block difficulty and timestamp if needed */

	return (0);
}
