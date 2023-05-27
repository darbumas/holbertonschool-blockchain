#include "hblk_crypto.h"

uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH])
{
	if (!digest)
		return (NULL);

	SHA256_CTX context;

	if (!SHA256_Init(&context))
		return (NULL);

	if (!SHA256_Update(&context, s, len))
		return (NULL);

	if (!SHA256_Final(digest, &context))
		return (NULL);

	return (digest);
}
