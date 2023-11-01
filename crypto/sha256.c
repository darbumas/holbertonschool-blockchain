#include "hblk_crypto.h"

/**
 * sha256 - Computes the hash of a sequence of bytes
 *
 * @s: The sequence of bytes to be hashed
 * @len: The number of bytes to hash in s
 * @digest: The resulting hash
 * Return: A pointer to digest or NULL if digest happens to be NULL
 */
uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX context;

	if (!digest)
		return (NULL);

	if (!SHA256_Init(&context))
		return (NULL);

	if (!SHA256_Update(&context, s, len))
		return (NULL);

	if (!SHA256_Final(digest, &context))
		return (NULL);

	return (digest);
}
