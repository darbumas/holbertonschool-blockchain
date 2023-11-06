#include "../blockchain.h"

/**
 * find_unspent1 - finds an unspent transaction output by input
 * @input: points to the transaction input to verify
 * @all_unspent: is the list of all unspent transaction outputs to date
 * Return: a pointer to the unspent transaction output
 */
unspent_tx_out_t *find_unspent1(tx_in_t const *input, llist_t *all_unspent)
{
	/*
	 * Implement a search in the 'all_unspent' list to find the unspent
	 * output based on the input's block_hash, tx_id, and tx_out_hash.
	 */
	ssize_t i;
	unspent_tx_out_t *utxo = NULL;

	for (i = 0; i < llist_size(all_unspent); i++)
	{
		utxo = llist_get_node_at(all_unspent, i);
		if (!memcmp(input->block_hash, utxo->block_hash,
					SHA256_DIGEST_LENGTH) &&
			!memcmp(input->tx_id, utxo->tx_id, SHA256_DIGEST_LENGTH)
			&& !memcmp(input->tx_out_hash, utxo->out.hash,
				SHA256_DIGEST_LENGTH))
			return (utxo);
	}

	return (NULL);
}

/**
 * transaction_is_valid - verifies that a transaction is valid
 * @transaction: points to the transaction to verify
 * @all_unspent: is the list of all unspent transaction outputs to date
 * Return: 1 if valid transaction, 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction,
		llist_t *all_unspent)
{
	uint8_t computed_hash[SHA256_DIGEST_LENGTH];
	uint32_t tot_in = 0, tot_out = 0;
	ssize_t i;
	tx_in_t *txi = NULL;
	unspent_tx_out_t *utxo = NULL;

	if (!transaction || !all_unspent)
		return (0);

	/* Compute hash */
	if (!transaction_hash(transaction, computed_hash))
		return (0);
	/* Validate tx inputs */
	if (memcmp(computed_hash, transaction->id, SHA256_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "transaction_is_valid: No match\n");
		return (0);
	}

	/* Iterate through the inputs in the transaction */
	for (i = 0; i < llist_size(transaction->inputs); i++)
	{
		txi = llist_get_node_at(transaction->inputs, i);
		/* Get the referenced unspent tx output */
		utxo = find_unspent1(txi, all_unspent);
		if (!utxo)
		{
			fprintf(stderr, "transaction_is_valid: utxo not found\n");
			return (0);
		}
		/* Accumulate the total input value */
		tot_in += utxo->out.amount;
	}

	/* Iterate through the outputs in the transaction */
	for (i = 0; i < llist_size(transaction->outputs); i++)
	{
		/* Accumulate the total output value */
		tot_out += ((tx_out_t *)llist_get_node_at(transaction->outputs,
					i))->amount;
	}

	/* Validate tx amount */
	if (tot_in != tot_out)
	{
		fprintf(stderr, "transaction_is_valid: tot_in != tot_out\n");
		return (0);
	}

	return (1);
}

