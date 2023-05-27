#include "hblk_crypto.h"

/**
 * ec_sign - Signs a given set of bytes using EC_KEY private key
 * @key: points to the EC_KEY structure containing the private key
 * @msg: points to the msglen characters to be signed
 * @msglen: count of bytes constituted in the message
 * @sig: holds the address at which to store the signature
 * Return: pointer to the signature buffer, otherwise NULL
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen,
		sig_t *sig)
{
	unsigned int len;

	if (!key || !msg || !sig)
		return (NULL);

	/* Allocate memory for the signature */
	sig->_sig = malloc(ECDSA_size((EC_KEY *)key));
	if (!sig->sig)
		return (NULL);

	/* Sign the message digest */
	if (!ECDSA_sign(0, msg, msglen, sig->sig, &len, (EC_KEY *)key))
	{
		free(sig->sig);
		return (NULL);
	}

	sig->len = len;

	return (sig->sig);
}
