#include "blockchain.h"

/**
 * block_hash - Computes the hash of a Block
 * @block: points to the Block to be hashed
 * @hush_buf: byte array to store the resulting hash
 *
 * Return: pointer to the hush buffer it is stored in.
 */
uint8_t *block_hash(block_t const *block,
		uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	SHA256_CTX sha256;
	block_t tmp_block;

	/* Prepare a temp block_info that has zeroed hash field */
	tmp_block = *block;

	/* Zero out entire temp block structure (debugging valgrind) */
	memset(&tmp_block, 0, sizeof(block_t));
	memset(tmp_block.hash, 0, SHA256_DIGEST_LENGTH);

	SHA256_Init(&sha256);
	SHA256_Update(&sha256, &tmp_block, sizeof(block_t));
	SHA256_Final(hash_buf, &sha256);

	return (hash_buf);
}
