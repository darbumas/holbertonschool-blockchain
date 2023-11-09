/* Description: Coinbase transaction contains 2 inputs */

#include <stdlib.h>

#include "../transaction.h"

EC_KEY *_ec_load(char const *folder);

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	transaction_t *coinbase;
	EC_KEY *owner;
	tx_in_t in = {{0}, {0}, {0}, {{0}, 0}};

	owner = _ec_load("_keys/alex");

	coinbase = coinbase_create(owner, 98);
	llist_add_node(coinbase->inputs, &in, ADD_NODE_REAR);
	if (coinbase_is_valid(coinbase, 98))
		printf("Coinbase transaction valid\n");
	else
		printf("Coinbase transaction invalid\n");

	/* Cleanup */
	EC_KEY_free(owner);

	return (EXIT_SUCCESS);
}
