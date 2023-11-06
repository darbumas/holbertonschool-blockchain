#include "../blockchain.h"

/**
 * transaction_init - initialize transaction structure
 *
 * Return: pointer to transaction structure
 */
transaction_t *transaction_init(void)
{
	transaction_t *tx = calloc(1, sizeof(*tx));

	if (!tx)
	{
		fprintf(stderr, "transaction_init: calloc failed\n");
		return (NULL);
	}

	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || !tx->outputs)
	{
		fprintf(stderr, "transaction_init: llist_create failed\n");
		llist_destroy(tx->inputs, 0, NULL);
		llist_destroy(tx->outputs, 0, NULL);
		free(tx);
		return (NULL);
	}

	return (tx);
}

/**
 * select_unspent - select unspent transaction outputs
 *
 * @inputs: list of input transactions
 * @all_unspent: list of all unspent transaction outputs
 * @amount: amount needed
 * @sender_pub: public key of transaction sender
 * Return: total amount of selected unspent transaction outputs or 0
 */
uint32_t select_unspent(llist_t *inputs, llist_t *all_unspent, uint32_t amount,
		uint8_t sender_pub[EC_PUB_LEN])
{
	unspent_tx_out_t *utx_out = NULL;
	tx_in_t *txi = NULL;
	int i = 0;
	uint32_t total_selected = 0;

	for (; i < llist_size(all_unspent); i++)
	{
		utx_out = llist_get_node_at(all_unspent, i);

		/* check if utx is unspent and belongs to sender */
		if (memcmp(utx_out->out.pub, sender_pub, EC_PUB_LEN) == 0)
		{
			/* Create a tx input with each unspent output */
			txi = tx_in_create(utx_out);
			/* Add tx input to inputs list */
			llist_add_node(inputs, txi, ADD_NODE_REAR);
			total_selected += utx_out->out.amount;
			if (total_selected >= amount)
				break;
		}
	}
	return (total_selected);
}

/**
 * clean_up - clean up transaction structure
 * @tx: transaction structure to clean up
 * Return: Always NULL
 */
transaction_t *clean_up(transaction_t *tx)
{
	if (tx->inputs)
		llist_destroy(tx->inputs, 0, free);
	if (tx->outputs)
		llist_destroy(tx->outputs, 0, free);
	free(tx);
	return (NULL);
}

/**
 * transaction_create - create a transaction
 *
 * @sender: private key of transaction sender
 * @receiver: public key of transaction receiver
 * @amount: amount to send
 * @all_unspent: list of all unspent transaction outputs
 * Return: pointer to transaction structure or NULL
 */
transaction_t *transaction_create(EC_KEY const *sender,
		EC_KEY const *receiver, uint32_t amount, llist_t *all_unspent)
{
	transaction_t *tx = NULL;
	tx_out_t *txo = NULL;
	tx_in_t *txi = NULL;
	ssize_t i = 0;
	uint8_t key_in[EC_PUB_LEN] = {0}, key_out[EC_PUB_LEN] = {0};
	uint32_t total_selected = 0;

	if (!sender || !receiver || !all_unspent)
	{
		fprintf(stderr, "transaction_create: NULL parameter\n");
		return (NULL);
	}
	ec_to_pub(sender, key_in);
	ec_to_pub(receiver, key_out);
	tx = transaction_init();
	if (!tx)
		return (NULL);
	if (select_unspent(tx->inputs, all_unspent, amount, key_in) < amount)
		return (clean_up(tx));
	txo = tx_out_create(amount, key_out);
	if (!txo)
		return (clean_up(tx));
	llist_add_node(tx->outputs, txo, ADD_NODE_REAR);
	if (total_selected > amount)
	{
		txo = tx_out_create(total_selected - amount, key_in);
		if (!txo)
			return (clean_up(tx));
		llist_add_node(tx->outputs, txo, ADD_NODE_REAR);
	}
	transaction_hash(tx, tx->id);
	/* sign transaction input with sender's private key */
	for (i = 0; i < llist_size(tx->inputs); i++)
	{
		txi = llist_get_node_at(tx->inputs, i);
		if (!tx_in_sign(txi, tx->id, sender, all_unspent))
			return (clean_up(tx));
	}
	return (tx);
}
