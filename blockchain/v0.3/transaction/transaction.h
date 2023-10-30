#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "blockchain.h"
#include "hblk_crypto.h"


/**
 * struct transaction_s - Transaction output.
 *
 * @amount:	Amount of the transaction
 * @pub:	Receiver's public key
 * @hash:	The hash of @amount + @pub = output ID.
 */
typedef struct tx_out_s
{
	uint32_t amount;
	uint8_t pub[EC_PUB_LEN];
	uint8_t hash[SHA256_DIGEST_LENGTH];
} transaction_t;

/* Prototypes */
transaction_t *tx_out_create(uint32_t amount, uint8_t const pub[EC_PUB_LEN]);
#endif /* _TRANSACTION_H_ */
