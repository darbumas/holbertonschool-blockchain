#include "blockchain.h"
#include "provided/endianness.h"
#include <sys/stat.h>
#include <errno.h>

/**
 * path_validate - validates the path and opens a file descriptor in read-only
 * @path: path in which file to be found
 *
 * Return: file descriptor, or -1 upon failure
 */
int path_validate(char const *path)
{
	struct stat status;
	int fd;

	if (lstat(path, &status) == -1)
	{
		fprintf(stderr, "path_validate: lstat '%s': %s\n", path,
				strerror(errno));
		return (-1);
	}

	if ((size_t)(status.st_size) < sizeof(hblk_file_hdr_t) +
			GEN_BLK_SERIAL_SZ)
	{
		fprintf(stderr, "path_validate: %s\n",
			"file is too small to contain a valid blockchain");
		return (-1);
	}
	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "path_validate: open '%s': %s\n", path,
				strerror(errno));
	}
	return (fd);
}

/**
 * read_file_hdr - reads and validates blockchain serialized file header
 * @fd: file descriptor
 * @local_endian: 1 for little, 2 for big
 * @file_endian: 1 for little, 2 for big
 * @blocks: number of blocks in blockchain
 * Return: 0 on success, or 1 on failure
 */
int read_file_hdr(int fd, uint8_t local_endian, uint8_t *file_endian,
		uint32_t *blocks)
{
	hblk_file_hdr_t hdr;

	if (!file_endian || !blocks)
	{
		fprintf(stderr, "read_file_hdr: NULL parameter(s)\n");
		return (1);
	}
	if (local_endian != 1 && local_endian != 2)
	{
		fprintf(stderr, "read_file_hdr: invalid local endianness\n");
		return (1);
	}
	if (read(fd, &hdr, sizeof(hblk_file_hdr_t)) == -1)
	{
		perror("read_file_hdr: read");
		return (1);
	}
	if (strncmp((char *)hdr.hblk_magic, HBLK_MAGIC, HBLK_MAGIC_LEN) != 0)
	{
		fprintf(stderr, "read_file_hdr: invalid magic number\n");
		return (1);
	}
	if (strncmp((char *)hdr.hblk_version, HBLK_VERSION, HBLK_VERSION_LEN)
			!= 0)
	{
		fprintf(stderr, "read_file_hdr: %s\n",
				"Incompatible version number");
		return (1);
	}

	*file_endian = hdr.hblk_endian;
	if (*file_endian != local_endian)
#ifdef __GNUC__
		hdr.hblk_blocks = __builtin_bswap32(hdr.hblk_blocks);
#else
		_swap_endian(&(hdr.hblk_blocks), 32);
#endif
	*blocks = hdr.hblk_blocks;
	return (0);
}

/**
 * bswap_block - reverses endianness of relevant values in block_t struct
 * @block: pointer to the block to have values' byte order reversed
 *
 * Note: __builtin_bswapXX preferred over _swap_endian in
 * provided/_endianness.c
 * for being compiled as only one assembly instruction
 */
void bswap_block(block_t *block)
{
	if (!block)
	{
		fprintf(stderr, "bswap_block: NULL parameter\n");
		return;
	}

#ifdef __GNUC__
	block->info.index = __builtin_bswap32(block->info.index);
	block->info.difficulty = __builtin_bswap32(block->info.difficulty);
	block->info.timestamp = __builtin_bswap64(block->info.timestamp);
	block->info.nonce = __builtin_bswap64(block->info.nonce);
	block->data.len = __builtin_bswap32(block->data.len);
#else
	_swap_endian(&(block->info.index), 32);
	_swap_endian(&(block->info.difficulty), 32);
	_swap_endian(&(block->info.timestamp), 64);
	_swap_endian(&(block->info.nonce), 64);
	_swap_endian(&(block->data.len), 32);
#endif
}

/**
 * deserialize_blocks - deserializes blocks from file into blockchain structure
 * @fd: file descriptor
 * @blockchain: pointer to an empty blockchain struct
 * @local_endian: 1 for little, 2 for big
 * @file_endian: 1 for little, 2 for big
 * @blocks: number of blocks in the blockchain
 *
 * Return: 0 on success, 1 upon failure
 */
int deserialize_blocks(int fd, const blockchain_t *blockchain,
		uint8_t local_endian, uint8_t file_endian, uint32_t blocks)
{
	uint32_t i;
	block_t *block;

	if (!blockchain || !llist_is_empty(blockchain->chain))
	{
		fprintf(stderr, "deserialize_blocks: NULL paramete or empy\n");
		return (1);
	}
	for (i = 0; i < blocks; i++)
	{
		/* assuming header has already been read from fd */
		block = calloc(1, sizeof(block_t));
		if (!block)
		{
			fprintf(stderr, "deserialize_blocks: calloc failed\n");
			return (1);
		}
		if (read(fd, &(block->info), sizeof(block_info_t)) == -1 ||
			read(fd, &(block->data.len), sizeof(uint32_t)) == -1 ||
			read(fd, &(block->data.buffer), block->data.len) == -1 ||
			read(fd, &(block->hash), SHA256_DIGEST_LENGTH) == -1)
		{
			perror("deserialize_blocks: read");
			return (1);
		}
		if (local_endian != file_endian)
			bswap_block(block);
		if (llist_add_node(blockchain->chain, (llist_node_t)block,
					ADD_NODE_REAR) != 0)
		{
			fprintf(stderr, "llist_add_node: %s\n",
				strE_LLIST(llist_errno));
			return (1);
		}
	}
	return (0);
}

/**
 * blockchain_deserialize - deserializes a blockchain from a file
 * @path: full path to file
 *
 * Return: pointer to the deserialized blockchain, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	int fd;
	uint8_t local_endian, file_endian;
	uint32_t blocks;
	blockchain_t *blockchain;
	block_t *genesis;

	if (!path)
	{
		fprintf(stderr, "blockchain_deserialize: NULL parameter\n");
		return (NULL);
	}
	fd = path_validate(path);
	if (fd == -1)
		return (NULL);
	blockchain = blockchain_create();
	if (!blockchain)
	{
		close(fd);
		return (NULL);
	}
	/* remove preloaded Genesis Block at head of list */
	genesis = (block_t *)llist_pop(blockchain->chain);
	if (genesis)
		free(genesis);

	local_endian = _get_endianness();

	if (read_file_hdr(fd, local_endian, &file_endian, &blocks) != 0 ||
		deserialize_blocks(fd, blockchain, local_endian, file_endian,
			blocks) != 0)
	{
		close(fd);
		blockchain_destroy(blockchain);
		return (NULL);
	}
	close(fd);
	return (blockchain);
}
