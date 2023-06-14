#include "blockchain.h"

/**
 * strE_LLIST - returns error description corresponding to llist_errno value
 * @code: llist shared library llist_errno value
 *
 * Return: llist error description as string, or generic message on failure
 */
char *strE_LLIST(E_LLIST code)
{
	switch (code)
	{
		case LLIST_SUCCESS:
			return ("success");
		case LLIST_NODE_NOT_FOUND:
			return ("node not found");
		case LLIST_NULL_ARGUMENT:
			return ("NULL paraeter(s)");
		case LLIST_OUT_OF_RANGE:
			return ("out of range");
		case LLIST_MALLOC_ERROR:
			return ("malloc error");
		case LLIST_MULTITHREAD_ISSUE:
			return ("multithreading issue");
		default:
			return ("(unknown error code)");
	}
}
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
	block_t *genesis_block = NULL;
	char *genesis_data = "Holberton School";
	char *genesis_hash =
	"c52c26c8b5461639635d8edf2a97d48d0c8e0009c817f2b1d3d7ff2f04515803";
	blockchain_t *blockchain = NULL;

	blockchain = malloc(sizeof(blockchain_t));
	if (!blockchain)
		return (NULL);

	blockchain->chain = llist_create(MT_SUPPORT_FALSE);
	if (!blockchain->chain)
	{
		free(blockchain);
		return (NULL);
	}

	genesis_block = malloc(sizeof(block_t));
	if (!genesis_block)
	{
		free(blockchain);
		return (NULL);
	}
	memset(genesis_block, 0, sizeof(*genesis_block));
	genesis_block->info.index = 0;
	genesis_block->info.difficulty = 0;
	genesis_block->info.timestamp = 1537578000;
	genesis_block->info.nonce = 0;
	memset(genesis_block->info.prev_hash, 0, SHA256_DIGEST_LENGTH);
	memcpy(genesis_block->data.buffer, genesis_data, strlen(genesis_data));
	genesis_block->data.buffer[strlen(genesis_data)] = '\0';
	genesis_block->data.len = strlen(genesis_data);
	hex2bin(genesis_hash, genesis_block->hash);
	if (llist_add_node(blockchain->chain, genesis_block, ADD_NODE_REAR) != 0)
	{
		llist_destroy(blockchain->chain, 1, NULL);
		free(blockchain);
		return (NULL);
	}
	return (blockchain);
}
