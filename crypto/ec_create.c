#include "hblk_crypto.h"

/**
 * ec_create - Creates a new EC key pair
 *
 * Description: Uses the secp256k1 elliptic curve to create a new EC key pair.
 * Both the private and public keys are generated.
 *
 * Return: A pointer to an EC_KEY structure containing both the public and
 * private keys, or NULL upon failure.
 */
EC_KEY *ec_create(void)
{
	EC_KEY *key;

	key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!key)
		return (NULL);

	if (!EC_KEY_generate_key(key))
	{
		EC_KEY_free(key);
		return (NULL);
	}
	return (key);
}
