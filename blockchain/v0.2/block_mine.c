#include "blockchain.h"

/**
 * block_mine - mines a Block in order to insert it in the Blockchain
 * @block: points to the Block to be mined
 *
 * Note: must find a hash matching the difficulty level
 */
void block_mine(block_t *block)
{
	if (!block || block->info.difficulty == 0)
		return;

	block->info.nonce = 0;

	block_hash(block, block->hash);
	while (!hash_matches_difficulty(block->hash, block->info.difficulty))
	{
		block->info.nonce += 1;
		block_hash(block, block->hash);
	}
}
