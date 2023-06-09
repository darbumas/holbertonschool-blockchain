#include "blockchain.h"

/**
 * char2int - Converts a hex character to its integer value
 * @hex: hex character to be converted
 *
 * Return: integer value of the hex
 */
int char2int(char hex)
{
	if (hex >= '0' && hex <= '9')
		return (hex - '0');
	if (hex >= 'A' && hex <= 'F')
		return (hex - 'A' + 10);
	if (hex >= 'a' && hex <= 'f')
		return (hex - 'a' + 10);
	return (0);
}

/**
 * hex2bin - Converts a hex string to byte data
 * @src: source string in hexadecimal format
 * @target: target byte array where the converted values will be stored
 *
 * Note: This function assumes that src has an even number length and target
 * has enough space to store len(src)/2 bytes.
 */
void hex2bin(const char *src, uint8_t *target)
{
	while (*src && src[1])
	{
		*(target++) = char2int(*src) * 16 + char2int(src[1]);
		src += 2;
	}
}

/**
 * blockchain_create - Creates a blockchain structure and initializes it.
 *
 * Return: pointer to the created blockchain, otherwise NULL.
 */
blockchain_t *blockchain_create(void)
{
	block_t *genesis_block;
	char *genesis_data;
	char *genesis_hash;
	blockchain_t *block;

	block = malloc(sizeof(blockchain_t));
	if (!block)
		return (NULL);

	/* Create the linked list for the blockchain's chain */
	block->chain = llist_create(MT_SUPPORT_FALSE);
	if (!block->chain)
	{
		free(block);
		return (NULL);
	}

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
	genesis_block->data.buffer[strlen(genesis_data)] = '\0';
	genesis_block->data.len = strlen(genesis_data);

	/* Convert the string representation of the hash into bytes */
	genesis_hash =
	"c52c26c8b5461639635d8edf2a97d48d0c8e0009c817f2b1d3d7ff2f04515803";
	hex2bin(genesis_hash, genesis_block->hash);

	/* Add the genesis block to the blockchain's chain */
	if (llist_add_node(block->chain, genesis_block, ADD_NODE_REAR) != 0)
	{
		llist_destroy(block->chain, 1, NULL);
		free(block);
		return (NULL);
	}
	return (block);
}
