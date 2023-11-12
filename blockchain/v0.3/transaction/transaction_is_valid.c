#include "../blockchain.h"

/**
 * find_unspent_tx_out - Callback to validate the inputs of a transaction by
 * finding the corresponding unspent transaction output.
 * @node:	current node (unspent tx output) being checked.
 * @index:	index of @node
 * @args:	arguments: arg[0] is list of txi, arg[1] is balance
 *			arg[2] is index of @node, arg[3] is tx_ID
 * Return:	0 on success, 1 on failure
 */
int find_unspent_tx_out(llist_node_t node, unsigned int index, void *args)
{
	unspent_tx_out_t *unspent = node;
	void **ptr = args;
	llist_t *list = ptr[0];
	uint32_t *in_idx = ptr[2], *balance = ptr[1];
	tx_in_t *tx_in = llist_get_node_at(list, *in_idx);
	EC_KEY *key;

	(void)index;
	if (!tx_in)
	{
		fprintf(stderr, "find_unspent_tx_out: tx_in is NULL\n");
		return (1);
	}

	/* check if tx_in->tx_out_hash matches unspent->out.hash */
	if (!memcmp(unspent->out.hash, tx_in->tx_out_hash,
				SHA256_DIGEST_LENGTH))
	{
		*in_idx += 1;
		*balance += unspent->out.amount;
		key = ec_from_pub(unspent->out.pub);
		if (!key)
			return (1);

		/* verify signature */
		if (!ec_verify(key, ptr[3], SHA256_DIGEST_LENGTH, &tx_in->sig))
		{
			EC_KEY_free(key);
			return (1);
		}
		EC_KEY_free(key);
	}
	return (0);
}

/**
* validate_txs - Callback to validate total input amount matches or exceeds
* total output amount.
* @node: current node
* @index: index of @node (unused)
* @arg: pointer to the running total input amount
* Return: 0 on success, 1 on failure
*/
int validate_txs(llist_node_t node, unsigned int index, void *arg)
{
	uint32_t *balance = arg;
	tx_out_t *out = node;
	(void)index;

	/* Check if running total input is less than current output amount*/
	if (*balance < out->amount)
	{
		fprintf(stderr, "validate_txs: balance < out->amount\n");
		return (1);
	}
	/* Subtract current output amount from running total input */
	*balance -= out->amount;

	return (0);
}

/**
* transaction_is_valid - validate transaction
* @transaction: transaction to be validated
* @all_unspent: unpspent outputs
* Return: 1 if valid, 0 otherwise
*/
int transaction_is_valid(transaction_t const *transaction,
		llist_t *all_unspent)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];
	void *args[4];
	uint32_t i = 0;
	uint32_t total_input = 0;

	if (!transaction || !all_unspent)
		return (0);
	transaction_hash(transaction, hash_buf);
	if (memcmp(transaction->id, hash_buf, SHA256_DIGEST_LENGTH))
	{
		fprintf(stderr, "transaction_is_valid: invalid tx ID\n");
		return (0);
	}
	args[0] = transaction->inputs, args[1] = &total_input;
	args[2] = &i, args[3] = (void *)&transaction->id;
	if (llist_for_each(all_unspent, find_unspent_tx_out, args)
		|| i != (uint32_t)llist_size(transaction->inputs))
	{
		return (0);
	}
	if (llist_for_each(transaction->outputs, validate_txs, &total_input)
		|| total_input != 0)
	{
		fprintf(stderr, "transaction_is_valid: invalid tx total in\n");
		return (0);
		/* debugging */
		fprintf(stderr, "transaction_is_valid: total_input = %u\n",
			total_input);
		return (0);
	}
	/* debugging */
	fprintf(stderr, "transaction_is_valid: transaction is valid\n");
	return (1);
}
