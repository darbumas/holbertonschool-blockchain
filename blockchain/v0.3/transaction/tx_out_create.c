#include "transaction.h"

/**
 * tx_out_create - Allocates and initializes a transactionn output structure
 * @amount:	the amount of the transaction
 * @pub:	the public key of the transaction receiver
 *
 *  Return: Pointer to created transaction output, or NULL upon failure
 */
transaction_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{
	transaction_t *trxn = NULL;

	trxn = calloc(1, sizeof(transaction_t));

	if (!trxn)
		return (NULL);

	trxn->amount = amount;
	memcpy(trxn->pub, pub, sizeof(trxn->pub));
	if (!sha256((int8_t const *)trxn, sizeof(trxn->amount) +
				sizeof(trxn->pub), trxn->hash))
	{
		free(trxn);
		return (NULL);
	}
	return (trxn);
}
