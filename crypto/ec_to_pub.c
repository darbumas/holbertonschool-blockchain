#include "hblk_crypto.h"

/**
 * ec_to_pub - Extracts the public key from an EC_KEY opaque structure
 * @key: Pointer to the EC_KEY structure to retrieve the key from
 * @pub: Address at which to store the extracted public key
 * Return: Pointer to pub, otherwise NULL
 */

uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
	const EC_POINT *pub_key;
	const EC_GROUP *group;
	size_t pub_len;

	if (!key || !pub)
		return (NULL);

	pub_key = EC_KEY_get0_public_key(key);
	if (!pub_key)
		return (NULL);

	group = EC_KEY_get0_group(key);
	if (!group)
		return (NULL);

	/* Convert the pub key to a binary and store it in the pub array */
	pub_len = EC_POINT_point2oct(group, pub_key,
			POINT_CONVERSION_UNCOMPRESSED, pub, EC_PUB_LEN, NULL);
	if (pub_len == 0)
		return (NULL);

	return (pub);
}
