#ifndef _BLOCKCHAIN_H_
#define _BLOCKCHAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <llist.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <unistd.h>

#define HBLK_MAGIC "HBLK"
#define HBLK_MAGIC_LEN 4
#define HBLK_VERSION "0.3"
#define HBLK_VERSION_LEN 3
#define HBLK_LITTLE_ENDIAN 1
#define HBLK_BIG_ENDIAN 2

#define GENESIS_TS 1537578000
#define GENESIS_DATA "Holberton School"
#define GENENIS_DAT_LEN 16
#define GENESIS_HASH \
	"\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf\x2a\x97\xd4\x8d" \
	"\x0c\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f\x04\x51\x58\x03"

#define GENESIS_BLK { \
	{ /* Info */ \
		0, /* index */ \
		0, /* difficulty */ \
		1537578000, /* timestamp */ \
		0, /* nonce */ \
		{0} /* prev_hash */ \
	}, \
	{ /* data */ \
		"Holberton School", /* buffer */ \
		16 /* length */ \
	}, /* hash */ \
	"\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf\x2a\x97\xd4\x8d" \
	"\x0c\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f\x04\x51\x58\x03" \
}

/*
 * sizeof(block_t.info) + sizeof(block_t.data.len) +
 * GENESIS_BLK.data.len + sizeof(block_t.hash)
 */
#define GEN_BLK_SERIAL_SZ 108

/* Adjusting difficulty */
#define BLOCK_GENERATION_INTERVAL 1
#define DIFFICULTY_ADJUSTMENT_INTERVAL 5

/**
 * struct hblk_file_hdr_s - file header of the blockchain structure
 * @hblk_magic: Identifies the file as a valid serialized blockchain format;
 * "HBLK" (ASCII 48 42 4c 4b); these four bytes constitute the magic number
 * @hblk_version: Identifies the version at which the blockchain has been
 *   serialized. The format is X.Y, where both X and Y are ASCII characters
 *   between 0 and 9
 * @hblk_endian: This byte is set to either 1 or 2 to signify little or big
 *   endianness, respectively. This affects interpretation of multi-byte fields
 * @hblk_blocks: Number of blocks in the blockchain
 */
typedef struct hblk_file_hdr_s
{
	int8_t		hblk_magic[4];
	int8_t		hblk_version[3];
	uint8_t		hblk_endian;
	uint32_t	hblk_blocks;
} hblk_file_hdr_t;

/**
 * struct blockchain_s - Blockchain structure
 *
 * @chain: Linked list of pointers to block_t
 * @unspent: Linked list of unspent transaction outputs
 */
typedef struct blockchain_s
{
	llist_t		*chain;
	llist_t		*unspent;
} blockchain_t;

/**
 * struct block_info_s - Block info structure
 *
 * @index:      Index of the Block in the Blockchain
 * @difficulty: Difficulty of proof of work (hash leading zero bits)
 * @timestamp:  Time the Block was created at (UNIX timestamp)
 * @nonce:      Salt value used to alter the Block hash
 * @prev_hash:  Hash of the previous Block in the Blockchain
 */
typedef struct block_info_s
{
	/*
	 * The order of the elements in this structure should remain the same.
	 * It was designed so every element of this structure is aligned and
	 * doesn't require padding from the compiler.
	 * Therefore, it is possible to use the structure as an array of char,
	 * on any architecture.
	 */

	uint32_t	index;
	uint32_t	difficulty;
	uint64_t	timestamp;
	uint64_t	nonce;
	uint8_t		prev_hash[SHA256_DIGEST_LENGTH];
} block_info_t;


#define BLOCKCHAIN_DATA_MAX 1024
# define COINBASE_AMOUNT    50

/**
 * struct block_data_s - Block data
 *
 * @buffer: Data buffer
 * @len:    Data size (in bytes)
 */
typedef struct block_data_s
{
	/*
	 * @buffer must stay first, so we can directly use the structure as
	 * an array of char
	 */

	int8_t		buffer[BLOCKCHAIN_DATA_MAX];
	uint32_t	len;
} block_data_t;

/**
 * block_s - Block structure
 *
 * @info:         Block info
 * @data:         Block data
 * @transactions: List of transactions
 * @hash:         256-bit digest of the Block, to ensure authenticity
 */
typedef struct block_s
{
	block_info_t	info; /* This must stay first */
	block_data_t	data; /* This must stay second */
	llist_t		*transactions;
	uint8_t		hash[SHA256_DIGEST_LENGTH];
} block_t;

/**
 * struct transaction_s - Transaction structure
 *
 * @id:      Transaction ID. A hash of all the inputs and outputs.
 *           Prevents further alteration of the transaction.
 * @inputs:  List of `tx_in_t *`. Transaction inputs
 * @outputs: List of `tx_out_t *`. Transaction outputs
 */
typedef struct transaction_s
{
	uint8_t     id[SHA256_DIGEST_LENGTH];
	llist_t     *inputs;
	llist_t     *outputs;
} transaction_t;

/**
 * struct tx_out_s - Transaction output
 *
 * @amount: Amount received
 * @pub:    Receiver's public address
 * @hash:   Hash of @amount and @pub. Serves as output ID
 */
typedef struct tx_out_s
{
	uint32_t    amount;
	uint8_t     pub[EC_PUB_LEN];
	uint8_t     hash[SHA256_DIGEST_LENGTH];
} tx_out_t;

/**
 * struct tx_in_s - Transaction input
 *
 * Description: A transaction input always refers to a previous
 * transaction output. The only exception is for a Coinbase transaction, that
 * adds new coins to ciruclation.
 *
 * @block_hash:  Hash of the Block containing the transaction @tx_id
 * @tx_id:       ID of the transaction containing @tx_out_hash
 * @tx_out_hash: Hash of the referenced transaction output
 * @sig:         Signature. Prevents anyone from altering the content of the
 *               transaction. The transaction input is signed by the receiver
 *               of the referenced transaction output, using their private key
 */
typedef struct tx_in_s
{
	uint8_t     block_hash[SHA256_DIGEST_LENGTH];
	uint8_t     tx_id[SHA256_DIGEST_LENGTH];
	uint8_t     tx_out_hash[SHA256_DIGEST_LENGTH];
	sig_t       sig;
} tx_in_t;

/**
 * struct unspent_tx_out_s - Unspent transaction output
 *
 * Description: This structure helps identify transaction outputs that were not
 * used in any transaction input yet, making them "available".
 *
 * @block_hash: Hash of the Block containing the transaction @tx_id
 * @tx_id:      ID of the transaction containing @out
 * @out:        Copy of the referenced transaction output
 */
typedef struct unspent_tx_out_s
{
	uint8_t     block_hash[SHA256_DIGEST_LENGTH];
	uint8_t     tx_id[SHA256_DIGEST_LENGTH];
	tx_out_t    out;
} unspent_tx_out_t;

/* ƒ() prototypes */
blockchain_t *blockchain_create(void);
char *strE_LLIST(E_LLIST code);
block_t *block_create(block_t const *prev, int8_t const *data,
		uint32_t data_len);
void block_destroy(block_t *block);
void blockchain_destroy(blockchain_t *blockchain);
uint8_t *block_hash(block_t const *block,
		uint8_t hash_buf[SHA256_DIGEST_LENGTH]);
int blockchain_serialize(blockchain_t const *blockchain, char const *path);
blockchain_t *blockchain_deserialize(char const *path);
int block_is_valid(block_t const *block, block_t const *prev_block);

int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
		uint32_t difficulty);
void block_mine(block_t *block);
#endif /* _BLOCKCHAIN_H_ */
