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

	for (i = 0; i < difficulty / 8; i++)
	{
		if (hash[i] != 0)
			return (0);
	}

	mask <<= 8 - difficulty % 8;
	if ((hash[i] & mask) != 0)
		return (0);

	return (1);
}
