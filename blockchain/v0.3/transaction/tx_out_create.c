#include "../blockchain.h"

/**
 * tx_out_create - Allocates and initializes a transaction output structure
 *
 * @amount: The transaction amount
 * @pub: Receiver's public address
 * Return: A pointer to the created transaction output structure, or NULL
 */
tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{
	tx_out_t *tx_out = calloc(1, sizeof(tx_out_t));

	if (!tx_out)
		return (NULL);

	memcpy(tx_out->pub, pub, EC_PUB_LEN);
	tx_out->amount = amount;

	if (!sha256((const int8_t *)tx_out, sizeof(tx_out->amount) + EC_PUB_LEN,
		    tx_out->hash))
	{
		free(tx_out);
		return (NULL);
	}

	return (tx_out);
}
