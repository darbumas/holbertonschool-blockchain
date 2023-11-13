#include "blockchain.h"
#include "provided/endianness.h"
#include <sys/stat.h>
#include <errno.h>

/**
 * swap_block_header_endian - swap endianness of block header
 *
 * @block: pointer to block to be swapped
 * @direction: direction of swap
 */
void swap_block_header_endian(block_t *block, uint8_t direction)
{
	(void)direction;

	_swap_endian(&block->info.index, sizeof(block->info.index));
	_swap_endian(&block->info.difficulty, sizeof(block->info.difficulty));
	_swap_endian(&block->info.timestamp, sizeof(block->info.timestamp));
	_swap_endian(&block->info.nonce, sizeof(block->info.nonce));
}

/**
 * read_block_header - Reads a block header from a file
 *
 * @file: File pointer
 * @block: pointer to a block to save the data to
 * Return: 0 on success, -1 on failure
 */
int read_block_header(FILE *file, block_t *block)
{
	if (!file || !block)
		return (-1);

	fread(&block->info, sizeof(block->info), 1, file);
	if (ferror(file))
		return (-1);

	return (0);
}

/**
 * read_block_data - Reads a block data from a file
 *
 * @file: File pointer
 * @block: pointer to a block to save the data to
 * @flag_endian: flag to swap endianness
 * Return: 0 on success, -1 on failure
 */
int read_block_data(FILE *file, block_t *block, uint8_t flag_endian)
{
	uint32_t data_len;

	if (!file || !block)
		return (-1);

	fread(&data_len, sizeof(data_len), 1, file);
	if (ferror(file))
	{
		fprintf(stderr, "read_block_data: fread failed\n");
		return (-1);
	}

	if (flag_endian)
		_swap_endian(&data_len, sizeof(data_len));

	fread(&block->data.buffer, data_len, 1, file);
	fread(&block->hash, SHA256_DIGEST_LENGTH, 1, file);

	if (flag_endian)
		swap_block_header_endian(block, 1);

	block->data.len = data_len;
	*(block->data.buffer + data_len) = '\0';

	return (0);
}

/**
 * deserialize_blocks - deserializes all blocks from a file to a blockchain
 *
 * @file: file pointer
 * @blockchain: pointer to blockchain to add blocks to
 * @num_blocks: number of blocks to deserialize
 * Return: 0 on success, -1 on failure
 */
int deserialize_blocks(FILE *file, blockchain_t *blockchain,
			uint32_t num_blocks)
{
	block_t *block;
	long int genesis_size;
	uint32_t i;

	if (!file || !blockchain)
		return (-1);

	genesis_size = sizeof(block->info) + sizeof(block->hash) + 20L;

	fseek(file, genesis_size, SEEK_CUR);

	for (i = 0; i < num_blocks - 1; i++)
	{
		block = calloc(1, sizeof(*block));
		if (!block)
			return (-1);

		if (read_block_header(file, block) == -1 ||
			read_block_data(file, block, 0) == -1)
		{
			free(block);
			return (-1);
		}

		llist_add_node(blockchain->chain, block, ADD_NODE_REAR);
	}

	return (0);
}

/**
 * blockchain_deserialize - deserializes a Blockchain from a file
 *
 * @path: path to a file to load the Blockchain from
 * Return: pointer to the deserialized Blockchain or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *file;
	blockchain_t *blockchain;
	hblk_file_hdr_t header;
	uint8_t hblk_endian;
	int32_t hblk_blocks;

	if (!path || access(path, F_OK) == -1)
		return (NULL);

	file = fopen(path, "r");
	if (!file)
		return (NULL);
	fread(&header, sizeof(header), 1, file);
	if (ferror(file))
		return (fclose(file), NULL);

	if (memcmp(header.hblk_magic, "HBLK", 4) ||
	    memcmp(header.hblk_version, "0.3", 3))
		return (fclose(file), NULL);

	blockchain = blockchain_create();
	if (!blockchain)
		return (fclose(file), NULL);

	hblk_endian = _get_endianness() != header.hblk_endian;
	hblk_blocks = header.hblk_blocks;
	if (hblk_endian)
	{
		_swap_endian(&hblk_blocks, sizeof(hblk_blocks));
		_swap_endian(&header.hblk_blocks, sizeof(header.hblk_blocks));
	}
	if (deserialize_blocks(file, blockchain, hblk_blocks) == -1)
		return (blockchain_destroy(blockchain), fclose(file), NULL);

	fclose(file);
	return (blockchain);
}
