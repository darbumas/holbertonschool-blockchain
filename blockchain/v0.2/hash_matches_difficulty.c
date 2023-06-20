#include "blockchain.h"

/**
 * hash_matches_difficulty - Checks whether a given hash matches a given
 * difficulty
 * @hash: the hash to check
 * @difficulty: the minimum difficulty the hash should match
 *
 * Return: 1 if difficulty is respected, otherwise 0
 */

int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
		uint32_t difficulty)
{
	uint8_t i;
	uint8_t mask = 0xff;

	/* loop through the first difficulty / 8 bytes of the hash */
	for (i = 0; i < difficulty / 8; i++)
	{
		/* If any of these bytes NOT zero, return 0 */
		if (hash[i] != 0)
			return (0);
	}

	/* create mask with difficulty % 8 leading 1 bits */
	mask <<= 8 - difficulty % 8;

	/* check if the next hash byte ANDed with the mask is non-zero */
	if ((hash[i] & mask) != 0)
		return (0);

	return (1);
}
