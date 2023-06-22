#include "blockchain.h"

/**
 * block_mine - mines a Block in order to insert it in the Blockchain
 * @block: points to the Block to be mined
 *
 * Note: must find a hash matching the difficulty level
 */
void block_mine(block_t *block)
{
	uint8_t nonce = 0;

	if (!block)
		return;

	/* Loop until a hash is found that matches the difficulty */
	do {
		/* Increment nonce */
		block->info.nonce = nonce;
		/* Compute hash of block content */
		block_hash(block, block->hash);
		nonce++;
	} while (!hash_matches_difficulty(block->hash, block->difficulty));
}
