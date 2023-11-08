/* Description: Create a transaction sending 500 coins from 5 unspent outputs, 50 leftover */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../blockchain.h"

EC_KEY *ec_load(char const *folder);
void _transaction_print(transaction_t const *transaction);

/**
 * _clear_signatures - Clear input signatures to avoid randomness in output
 */
int _clear_signatures(tx_in_t *in, unsigned int idx, void *arg)
{
	memset(in->sig.sig, 0, SIG_MAX_LEN);
	memcpy(in->sig.sig, &idx, sizeof(idx));
	in->sig.len = sizeof(idx);

	(void)arg;
	return (0);
}

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	uint8_t block_hash[SHA256_DIGEST_LENGTH];
	uint8_t transaction_id[SHA256_DIGEST_LENGTH];
	tx_out_t *out;
	uint8_t pub[EC_PUB_LEN];
	EC_KEY *sender, *receiver;
	llist_t *all_unspent;
	unspent_tx_out_t *unspent;
	transaction_t *transaction;

	sha256((int8_t *)"Block test", strlen("Block test"), block_hash);
	sha256((int8_t *)"Transaction test", strlen("Transaction test"), transaction_id);

	sender = ec_load("_keys/alex");
	receiver = ec_load("_keys/guillaume");
	all_unspent = llist_create(MT_SUPPORT_FALSE);

	out = tx_out_create(100, ec_to_pub(sender, pub));
	unspent = unspent_tx_out_create(block_hash, transaction_id, out);
	llist_add_node(all_unspent, unspent, ADD_NODE_REAR);
	free(out);
	out = tx_out_create(200, ec_to_pub(sender, pub));
	unspent = unspent_tx_out_create(block_hash, transaction_id, out);
	llist_add_node(all_unspent, unspent, ADD_NODE_REAR);
	free(out);
	out = tx_out_create(60, ec_to_pub(sender, pub));
	unspent = unspent_tx_out_create(block_hash, transaction_id, out);
	llist_add_node(all_unspent, unspent, ADD_NODE_REAR);
	free(out);
	out = tx_out_create(80, ec_to_pub(sender, pub));
	unspent = unspent_tx_out_create(block_hash, transaction_id, out);
	llist_add_node(all_unspent, unspent, ADD_NODE_REAR);
	free(out);
	out = tx_out_create(110, ec_to_pub(sender, pub));
	unspent = unspent_tx_out_create(block_hash, transaction_id, out);
	llist_add_node(all_unspent, unspent, ADD_NODE_REAR);

	/* Create transaction */
	transaction = transaction_create(sender, receiver, 500, all_unspent);
	if (!transaction)
	{
		printf("Failed to create transaction\n");
		return (EXIT_FAILURE);
	}
	llist_for_each(transaction->inputs, (node_func_t)_clear_signatures, NULL);
	_transaction_print(transaction);

	/* Cleanup */
	EC_KEY_free(sender);
	EC_KEY_free(receiver);
	free(out);
	llist_destroy(all_unspent, 1, free);
	return (EXIT_SUCCESS);
}
