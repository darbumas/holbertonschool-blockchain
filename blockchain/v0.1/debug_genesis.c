#include "blockchain.h"

void test_genesis_block()
{
	block_t genesis_block = GENESIS_BLK;

	printf("Genesis Block:\nIndex: %u\nDifficulty: %u\nTimestamp: %lu\nNonce: %lu\n",
			genesis_block.info.index, genesis_block.info.difficulty,
			genesis_block.info.timestamp, genesis_block.info.nonce);
	printf("Buffer: %s\nLength: %u\n", genesis_block.data.buffer,
			genesis_block.data.len);
}

int main()
{
	test_genesis_block();

	return (0);
}
