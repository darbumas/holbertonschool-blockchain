#include "../blockchain.h"

/**
 * find_unspent - Iterates through the list of unspent transaction outputs
 * and compares the transaction output hash with the provided hash
 * @block_hash: the hash of the Block where the transaction output is stored
 * @tx_id: the hash of the transaction containing the transaction output
 * @all_unspent: the list of all unspent transaction outputs to date
 * Return: If the transaction output is found - a pointer to it, otherwise NULL
 *
 * Note: You have to use memcmp(3) to compare tx_out_hash to hash
 */
unspent_tx_out_t *find_unspent(uint8_t const block_hash[SHA256_DIGEST_LENGTH],
		uint8_t const tx_id[SHA256_DIGEST_LENGTH], llist_t *all_unspent)
{
	ssize_t i = 0;
	unspent_tx_out_t *utxo = NULL;

	/* Iterate through the list of unspent transaction outputs */
	for (; i < llist_size(all_unspent); i++)
	{
		/* Get the current unspent transaction output */
		utxo = (unspent_tx_out_t *)llist_get_node_at(all_unspent, i);

		/* Check if the block hash and tx_id match */
		if (memcmp(utxo->block_hash, block_hash, SHA256_DIGEST_LENGTH)
				== 0 && memcmp(utxo->tx_id, tx_id,
					SHA256_DIGEST_LENGTH) == 0)
			return (utxo);
	}
	return (NULL);
}

/**
 * tx_in_sign - Signs an input transaction
 * @in: points to the tx_in to sign
 * @tx_id: contains the ID (hash) of the transaction containing @in
 * @sender: contains the private key of the receiver of the coins contained
 * in the transaction output referenced by the input
 * @all_unspent: is the list of all unspent transaction outputs to date
 * Return: A pointer to the resulting signature structure upon success,
 * otherwise NULL
 *
 * Note: tx_in_sign signs only the transaction output referenced by @in
 * (No other transaction outputs in the transaction should be modified)
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
		EC_KEY const *sender, llist_t *all_unspent)
{
	unspent_tx_out_t *utxo = NULL;
	uint8_t pub[EC_PUB_LEN] = {0};

	/* Validate parameters */
	if (!in || !tx_id || !sender || !all_unspent)
	{
		fprintf(stderr, "tx_in_sign: NULL parameter\n");
		return (NULL);
	}

	/* Find the transaction output referenced by @in */
	utxo = find_unspent(in->block_hash, in->tx_id, all_unspent);
	if (!utxo)
	{
		fprintf(stderr, "find_unspent: cannot find unspent output\n");
		return (NULL);
	}

	/*
	 * Verify that the public key derived from the private key in @sender
	 * matches the public key contained in @utxo
	 */
	if (!ec_to_pub(sender, pub) ||
		memcmp(pub, utxo->out.pub, EC_PUB_LEN) != 0)
	{
		fprintf(stderr, "tx_in_sign: invalid public key\n");
		return (NULL);
	}

	/* Sign the transaction with the private key in @sender */
	if (!ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &in->sig))
	{
		fprintf(stderr, "tx_in_sign: sign failed\n");
		return (NULL);
	}

	/* Return the signature structure */
	return (&in->sig);
}
