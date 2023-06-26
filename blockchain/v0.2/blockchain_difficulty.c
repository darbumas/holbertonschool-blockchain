#include "blockchain.h"

/**
 * blockchain_difficulty - computes the difficulty to assign to a potential
 * next Block in the Blockchain
 * @blockchain: points to the Blockchain to analyze
 *
 * Return: the difficulty to be assigned to a potential next Block in the
 * Blockchain
 */
uint32_t blockchain_difficulty(blockchain_t const *blockchain)
{
	block_t *last_block, *adjusted_block;
	uint64_t expected_time, actual_time;
	uint32_t new_difficulty;

	if (!blockchain)
		return (0);

	/* Get last block in the Blockchain and compare the adjstmt interval */
	last_block = llist_get_tail(blockchain->chain);
	if (!last_block)
		return (0);

	if (last_block->info.index == 0 || last_block->info.index %
			DIFFICULTY_ADJUSTMENT_INTERVAL != 0)
		return (last_block->info.difficulty);

	/* Get the block at which the last diff. adjustment was made */
	adjusted_block = llist_get_node_at(blockchain->chain,
		last_block->info.index - DIFFICULTY_ADJUSTMENT_INTERVAL);
	if (!adjusted_block)
		return (0);

	/* Compute the expected time for the blocks to have been mined. */
	expected_time = BLOCK_GENERATION_INTERVAL *
			DIFFICULTY_ADJUSTMENT_INTERVAL;

	/* Calculate the actual time it took to mine the blocks */
	actual_time = last_block->info.timestamp - adjusted_block->info.timestamp;

	/* Adjust difficulty based on the actual time it took to mine the blocks*/
	if (actual_time * 2 < expected_time)
		new_difficulty = last_block->info.difficulty + 1;
	else if (actual_time > 2 * expected_time)
		new_difficulty = last_block->info.difficulty > 0 ?
			last_block->info.difficulty - 1 : 0;
	else
		new_difficulty = last_block->info.difficulty;

	return (new_difficulty);
}
