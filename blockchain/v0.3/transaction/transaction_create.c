#include "../blockchain.h"

/**
* select_unspent - Selects unspent transactions belonging to address
*
* @node:	current node in @utxo
* @i:		current index of @node
* @args:	arguments passed to the callback function
*		args[0] = uint8_t pub[EC_PUB_LEN]
*		args[1] = llist_t *inputs
*		args[2] = uint32_t *amount
* Return: 0 on success, 1 on failure
*/
int select_unspent(llist_node_t node, unsigned int i, void *args)
{
	unspent_tx_out_t *unspent = node;
	void **ptr = args;
	tx_in_t *tx_in;
	llist_t *inputs = ptr[1];
	uint32_t *amount = ptr[2];

	(void)i;

	/* Create tx_in for each unspent transaction */
	if (!memcmp(unspent->out.pub, ptr[0], EC_PUB_LEN))
	{
		tx_in = tx_in_create(unspent);
		llist_add_node(inputs, tx_in, ADD_NODE_REAR);
		*amount += unspent->out.amount;
	}
	return (0);
}

/**
* sign_current_tx - Signs current transaction input
* @node:	current node in @inputs
* @i:		index of @node
* @args:	arguments:
*		args[0] = int8_t const tx_id[SHA256_DIGEST_LENGTH]
*		args[1] = EC_KEY const *sender
*		args[2] = llist_t *all_unspent
* Return: 0 on success, 1 on failure
*/
int sign_current_tx(llist_node_t node, unsigned int i, void *args)
{
	void **ptr = args;
	tx_in_t *tx_in = node;

	(void)i;

	/* Sign transaction input */
	if (!tx_in_sign(tx_in, ptr[0], ptr[1], ptr[2]))
	{
		fprintf(stderr, "sign_current_tx: tx_in_sign failed\n");
		return (1);
	}

	return (0);
}

/**
 * create_outputs - Creates outputs for transaction. If change remains,
 * it is sent back to sender
 * @sender: sender's private key
 * @receiver: receiver's public key
 * @amount: amount to send to @receiver
 * @total: total amount of selected unspent transactions
 * Return: a new list of outputs or NULL on failure
*/
llist_t *create_outputs(EC_KEY const *sender, EC_KEY const *receiver,
			uint32_t amount, uint32_t total)
{
	llist_t *outputs;
	tx_out_t *tx_out;
	uint8_t pub[EC_PUB_LEN];

	/* Create transaction output for receiver */
	ec_to_pub(receiver, pub);
	tx_out = tx_out_create(amount, pub);
	if (!tx_out)
	{
		fprintf(stderr, "create_outputs: tx_out_create failed\n");
		return (NULL);
	}
	outputs = llist_create(MT_SUPPORT_FALSE);
	llist_add_node(outputs, tx_out, ADD_NODE_REAR);

	/* Create transaction output for sender if total exceeds amount */
	if (total > amount)
	{
		fprintf(stderr, "sending change...\n");
		ec_to_pub(sender, pub);
		tx_out = tx_out_create(total - amount, pub);
		if (!tx_out)
		{
			fprintf(stderr, "create_outputs: tx_out_create fail\n");
			return (NULL);
		}
		llist_add_node(outputs, tx_out, ADD_NODE_REAR);
	}
	fprintf(stderr, "create_outputs: success\n");

	return (outputs);
}

/**
* transaction_create - creates a transaction
* @sender: contains the private key of the transaction sender
* @receiver: contains the public key of the transaction receiver
* @amount: amount to send to @receiver
* @all_unspent: all unspent transactions, balance of @sender
* Return: pointer to the created transaction upon success, or NULL
*/
transaction_t *transaction_create(EC_KEY const *sender,
	EC_KEY const *receiver, uint32_t amount, llist_t *all_unspent)
{
	uint8_t sender_pub[EC_PUB_LEN];
	uint32_t unspent = 0;
	transaction_t *tx;
	llist_t *tx_in, *tx_out;
	void *args[3];

	if (!sender || !receiver || !all_unspent)
		return (NULL);
	/* Init transaction */
	tx = calloc(1, sizeof(*tx));
	if (!tx)
		return (NULL);

	/* Create transaction inputs (step: 1-3) */
	tx_in = llist_create(MT_SUPPORT_FALSE);
	ec_to_pub(sender, sender_pub);
	args[0] = sender_pub, args[1] = tx_in, args[2] = &unspent;
	llist_for_each(all_unspent, select_unspent, args);
	if (unspent < amount)
	{
		fprintf(stderr, "transaction_create: insufficient funds\n");
		llist_destroy(tx_in, 1, NULL);
		free(tx);
		return (NULL);
	}
	/* Create transaction outputs (step: 4-5) */
	tx_out = create_outputs(sender, receiver, amount, unspent);
	if (!tx_out)
	{
		llist_destroy(tx_in, 1, NULL);
		free(tx);
		return (NULL);
	}
	tx->inputs = tx_in, tx->outputs = tx_out;
	/* Compute transaction ID and sign transaction (step: 6-7) */
	transaction_hash(tx, tx->id);
	args[0] = tx->id, args[1] = (void *)sender, args[2] = all_unspent;
	llist_for_each(tx->inputs, sign_current_tx, args);
	return (tx);
}
