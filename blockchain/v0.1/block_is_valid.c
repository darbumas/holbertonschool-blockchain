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
	uint8_t prev_block_hash[SHA256_DIGEST_LENGTH];
	block_t genesis_block = GENESIS_BLK;

	/* check for NULL block or illegal data length */
	if (!block || block->data.len > BLOCKCHAIN_DATA_MAX ||
			block->data.len == 0)
		return (-1);

	/* check for genesis block */
	if (block->info.index == 0)
	{
		if (prev_block || memcmp(block, &genesis_block,
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
	/* compute and check prev_block's hash to detect tampering */
		block_hash(prev_block, prev_block_hash);
		if (memcmp(prev_block_hash, prev_block->hash,
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
