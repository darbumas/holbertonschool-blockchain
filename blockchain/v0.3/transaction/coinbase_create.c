#include "../blockchain.h"

/**
 * coinbase_create - creates a coinbase transaction
 * @receiver: contains the public key of the miner
 * @block_index: the index of the Block the coinbase transaction will belong to
 * Return: a pointer to the created transaction upon success, or NULL
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	tx_in_t *input;
	tx_out_t *output;
	transaction_t *coinbase_tx = malloc(sizeof(transaction_t));
	uint8_t pub[EC_PUB_LEN];

	if (!coinbase_tx)
	return (NULL);
	coinbase_tx->inputs = llist_create(MT_SUPPORT_FALSE);
	coinbase_tx->outputs = llist_create(MT_SUPPORT_FALSE);
	/* Create a transaction input */
	input = malloc(sizeof(tx_in_t));
	if (!input)
	{
		free(coinbase_tx);
		return (NULL);
	}
	memset(input, 0, sizeof(tx_in_t));
	/* Copy the block index into the first 4 bytes of tx_out_hash */
	memcpy(input->tx_out_hash, &block_index, sizeof(block_index));
	/* Add the input to the transaction */
	llist_add_node(coinbase_tx->inputs, input, ADD_NODE_REAR);
	/* Create a transaction output */
	output = tx_out_create(COINBASE_AMOUNT, ec_to_pub(receiver, pub));
	if (!output)
	{
		free(coinbase_tx);
		free(input);
		return (NULL);
	}
	/* Add the output to the transaction */
	llist_add_node(coinbase_tx->outputs, output, ADD_NODE_REAR);
	/* Calculate the transaction hash and store it in the id field */
	if (!transaction_hash(coinbase_tx, coinbase_tx->id))
	{
		free(coinbase_tx);
		free(input);
		free(output);
		return (NULL);
	}
	return (coinbase_tx);
}
