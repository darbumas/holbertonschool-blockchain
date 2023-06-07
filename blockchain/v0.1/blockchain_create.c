#include "blockchain.h"

/**
 * blockchain_create - Creates a blockchain structure and initializes it.
 *
 * Return: pointer to the created blockchain, otherwise NULL.
 */
blockchain_t *blockchain_create(void)
{
	block_t *genesis_block;
	char *genesis_data;
	const char *genesis_hash;
	blockchain_t *block;
	size_t i = 0;

	block = malloc(sizeof(blockchain_t));
	if (!block)
		return (NULL);

	genesis_block = malloc(sizeof(block_t));
	if (!genesis_block)
	{
		free(block);
		return (NULL);
	}

	/* Initialize the genesis block with static content */
	genesis_block->info.index = 0;
	genesis_block->info.difficulty = 0;
	genesis_block->info.timestamp = 1537578000;
	genesis_block->info.nonce = 0;
	memset(genesis_block->info.prev_hash, 0, SHA256_DIGEST_LENGTH);

	/* Copy the data into the genesis block */
	genesis_data = "Holberton School";
	memcpy(genesis_block->data.buffer, genesis_data, strlen(genesis_data));
	genesis_block->data.len = strlen(genesis_data);

	/* Convert the string representation of the hash into bytes */
	genesis_hash = "c52c26c8b5461639635d8edf2a97d48d0c8e0009c817f2b1d3d7ff2f04515803";
	for (; i < SHA256_DIGEST_LENGTH; i++)
		scanf(genesis_hash + i * 2, "%2hhx", &genesis_block->hash[i]);

	/* Create a linked list for the blockchain's chain */
	block->chain = llist_create(MT_SUPPORT_FALSE);
	if (!block->chain)
	{
		free(genesis_block);
		free(block);
		return (NULL);
	}

	/* Add the genesis block to the blockchain's chain */
	if (llist_add_node(block->chain, genesis_block, ADD_NODE_REAR) != 0)
	{
		llist_destroy(block->chain, 1, NULL);
		free(block);
		return (NULL);
	}
	return (block);

}
