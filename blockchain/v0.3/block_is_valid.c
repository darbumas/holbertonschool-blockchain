#include "blockchain.h"

/**
* check_txs - a function that checks whether all transactions are valid
* @node: current node
* @index: index of @node
* @all_unspent: list of unspent transactions
* Return: 0 if transaction is valid, 1 otherwise
*/
int check_txs(llist_node_t node, unsigned int index, void *all_unspent)
{
	if (index == 0)
		return (0);
	if (!transaction_is_valid(node, all_unspent))
		return (1);
	return (0);
}

/**
* validate_block - helper function that verifies if a Block is valid
* @block: block to be verified
* @prev_block: previous block
* @all_unspent: unspent transactions
* Return: -1 if block is not valid, 0 if block is valid
*/
static int validate_block(block_t const *block,
			block_t const *prev_block, llist_t *all_unspent)
{
	uint8_t block_sha[SHA256_DIGEST_LENGTH];
	uint8_t prev_sha[SHA256_DIGEST_LENGTH];

	if (prev_block->info.index != block->info.index - 1)
		return (-1);
	if (!block_hash(prev_block, prev_sha) ||
		memcmp(prev_block->hash, prev_sha, SHA256_DIGEST_LENGTH))
		return (-1);
	if (!block_hash(block, block_sha) ||
		memcmp(block->hash, block_sha, SHA256_DIGEST_LENGTH) ||
		memcmp(block->info.prev_hash, prev_sha, SHA256_DIGEST_LENGTH))
		return (-1);
	if (block->data.len > BLOCKCHAIN_DATA_MAX)
		return (-1);
	if (llist_size(block->transactions) < 1)
		return (-1);
	if (!coinbase_is_valid(llist_get_head(block->transactions),
						   block->info.index))
		return (-1);
	if (llist_for_each(block->transactions, check_txs, all_unspent))
		return (-1);
	return (0);
}

/**
* check_genesis - validates he Genesis block
*
* @block: block to be validated
*
* Return: -1 if block is not valid, 0 if block is valid
*/
static int check_genesis(block_t const *block)
{
	blockchain_t *blockchain = blockchain_create();
	int cmp_result = 0;

	if (!blockchain)
		return (-1);

	/* check that the Genesis block is the first block in the blockchain */
	cmp_result = memcmp(block, llist_get_head(blockchain->chain),
			sizeof(*block)) != 0;
	blockchain_destroy(blockchain);
	return (cmp_result * -1);
}

/**
* block_is_valid - checks that each Block's list of transactions is valid
*
* @block: block to be checked
* @prev_block: previous block
* @all_unspent: unspent transactions
* Return: -1 if block is not valid, 0 if block is valid
*/
int block_is_valid(block_t const *block, block_t const *prev_block,
				   llist_t *all_unspent)
{
	if (!block)
		return (-1);
	if (hash_matches_difficulty(block->hash, block->info.difficulty) == 0)
		return (-1);
	if (!prev_block)
		return (check_genesis(block));
	return (validate_block(block, prev_block, all_unspent));
}

