#include "hblk_crypto.h"

/**
 * ec_verify - verify a message's signature
 * @key: points to the EC_KEY structure containing the public key to verify signature
 * @msg: points to characters to verify signature of
 * @msglen: number of characters
 * @sig: points to signature to be checked
 * Return: 1 if signature is valid, 0 otherwise
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen,
		sig_t const *sig)
{
	if (!key || !msg || !sig)
		return (0);

	if (ECDSA_verify(0, msg, msglen, sig->sig, sig->len, (EC_KEY *)key) != 1)
	{
		ERR_print_errors_fp(stderr);
		return (0);
	}

	return (1);
}
