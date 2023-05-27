#include "hblk_crypto.h"

/**
 * ec_from_pub - Creates a EC_KEY structure given a public key
 * @pub: contains the public key to be converted
 *
 * Return: pointer the created EC_KEY structure, othewise NULL
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
	EC_KEY *key = NULL;
	EC_POINT *pub_key = NULL;

	if (!pub)
		return (NULL);

	key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!key)
		return (NULL);

	/* Create a EC_POINT obj to hold decoded public key */
	pub_key = EC_POINT_new(EC_KEY_get0_group(key));
	if (!pub_key)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	/* Decode the public key from byte array into EC_POINT struct */
	if (!EC_POINT_oct2point(EC_KEY_get0_group(key), pub_key, pub,
				EC_PUB_LEN, NULL))
	{
		EC_POINT_free(pub_key);
		EC_KEY_free(key);
		return (NULL);
	}

	/* Set the public key in the EC_KEY structure */
	if (!EC_KEY_set_public_key(key, pub_key))
	{
		EC_POINT_free(pub_key);
		EC_KEY_free(key);
		return (NULL);
	}

	/* Free the EC_POINT structure, as it's no longer need */
	EC_POINT_free(pub_key);

	return (key);
}
