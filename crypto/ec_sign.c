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
	uint8_t digest[SHA256_DIGEST_LENGTH];
	unsigned int sig_len;

	if (!key || !msg || !sig)
		return (NULL);

	/* Compute the hash of the message */
	if (!SHA256(msg, msglen, digest))
		return (NULL);

	/* Sign the message digest */
	sig->len = SIG_MAX_LEN;
	if (!ECDSA_sign(0, digest, SHA256_DIGEST_LENGTH, sig->sig, &sig_len,
				(EC_KEY *)key))
		return (NULL);

	sig->len = sig_len;

	return (sig->sig);
}
