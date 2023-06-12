#include "blockchain.h"

/**
 * blockchain_serialize - serializes blockchain to file
 * @blockchain: pointer to blockchain to be serialized
 * @path: path to save file
 * Return: 0 on success else -1 on error
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *fp;
	size_t num_blocks, i;
	block_t *block;
	uint8_t endianness;
	int8_t buffer[BLOCKCHAIN_DATA_MAX];

	if (!blockchain || !path)
		return (-1);

	num_blocks = llist_size(blockchain->chain);
	fp = fopen(path, "w");
	if (!fp)
		return (-1);

	/* Write file header */
	fwrite(HBLK_MAGIC, strlen(HBLK_MAGIC), 1, fp);
	fwrite(HBLK_VERSION, strlen(HBLK_VERSION), 1, fp);
	endianness = (sizeof(int) == 4) ? HBLK_LITTLE_ENDIAN : HBLK_BIG_ENDIAN;
	fwrite(&endianness, sizeof(endianness), 1, fp);
	fwrite(&num_blocks, sizeof(num_blocks), 1, fp);

	/* Serialize each block */
	for (i = 0; i < num_blocks; i++)
	{
		block = llist_get_node_at(blockchain->chain, i);
		/* Serialize block info */
		fwrite(&block->info, sizeof(block_info_t), 1, fp);

		/* Serialize block data */
		memset(buffer, 0, BLOCKCHAIN_DATA_MAX);
		memcpy(buffer, block->data.buffer, block->data.len);
		fwrite(&block->data.len, sizeof(block->data.len), 1, fp);
		fwrite(buffer, block->data.len, 1, fp);

		/* Serialize block hash */
		fwrite(block->hash, SHA256_DIGEST_LENGTH, 1, fp);
	}

	fclose(fp);
	return (0);
}
