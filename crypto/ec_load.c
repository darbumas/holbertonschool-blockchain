#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "hblk_crypto.h"

/**
 * ec_load - Loads an EC key pair from the disk
 * @folder: path to the folder from which to load the keys
 *
 * Return: pointer to the created EC key pair, otherwise NULL
 */
EC_KEY *ec_load(char const *folder)
{
	FILE *file;
	EC_KEY *key = NULL;
	char path[512];

	if (!folder)
		return (NULL);

	/* Open file containing the priv key */
	sprintf(path, "%s/%s", folder, PRI_FILENAME);
	file = fopen(path, "r");
	if (!file)
		return (NULL);

	/* Read the private key */
	key = PEM_read_ECPrivateKey(file, NULL, NULL, NULL);
	fclose(file);
	if (!key)
		return (NULL);

	/* Open file containing the pub key */
	sprintf(path, "%s/%s", folder, PUB_FILENAME);
	file = fopen(path, "r");
	if (!file)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	/* Read the pub key */
	if (!PEM_read_EC_PUBKEY(file, &key, NULL, NULL))
	{
		fclose(file);
		EC_KEY_free(key);
		return (NULL);
	}
	fclose(file);

	return (key);
}
