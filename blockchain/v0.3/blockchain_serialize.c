#include "blockchain.h"
#include "provided/endianness.h"

/**
 * write_single_block - Callback function to write a single block to a file
 *
 * @ptr: pointer to block to write
 * @index: index of block in linked list
 * @arg: FILE pointer to write to (must be already open)
 * Return: 0 on success else -1 on error
 */
int write_single_block(llist_node_t ptr, unsigned int index, void *arg)
{
	FILE *fp = arg;
	block_t *block = ptr;

	(void)index;

	if (!arg || !block)
		return (-1);

	fwrite((void *)&block->info, sizeof(block->info), 1, fp);
	fwrite((void *)&block->data.len, sizeof(block->data.len), 1, fp);
	fwrite(block->data.buffer, block->data.len, 1, fp);
	fwrite(block->hash, sizeof(block->hash), 1, fp);

	return (0);
}

/**
 * blockchain_serialize - serializes blockchain to file
 * @blockchain: pointer to blockchain to be serialized
 * @path: path to save file
 * Return: 0 on success else -1 on error
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *fp;
	hblk_file_hdr_t header;

	if (!blockchain || !path)
		return (-1);

	memcpy(header.hblk_magic, "HBLK", 4);
	memcpy(header.hblk_version, "0.3", 3);
	header.hblk_endian = _get_endianness();
	header.hblk_blocks = llist_size(blockchain->chain);
	if ((int)header.hblk_blocks == -1)
		return (-1);

	/* open file for writing */
	fp = fopen(path, "w");
	if (!fp)
		return (-1);

	/* write header to file */
	fwrite((void *)&header, sizeof(header), 1, fp);
	if (llist_for_each(blockchain->chain, write_single_block, fp) == -1)
	{
		fclose(fp);
		return (-1);
	}

	fclose(fp);
	return (0);
}
