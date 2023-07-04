#include "transaction.h"

/**
 * tx_out_create - Allocates and initializes a transaction output structure
 * @amount:	the amount of the transaction
 * @pub:	the public key of the transaction receiver
 *
 *  Return: Pointer to created transaction output, or NULL upon failure
 */
tx_out_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN])
{

}
