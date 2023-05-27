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
	EC_KEY *key;
	char path[512];

	if (!folder)
		return (NULL);

	/* Avoid potential buffer overflow */
	if (strlen(folder) > sizeof(path) - strlen(PRI_FILENAME) -
			strlen(PUB_FILENAME) - 2)
		return (NULL);

	/* Open file containing the priv key */
	snprintf(path, sizeof(path), "%s/%s", folder, PRI_FILENAME);
	file = fopen(path, "r");
	if (!file)
		return (NULL);

	/* Read the private key */
	key = PEM_read_ECPrivateKey(file, NULL, NULL, NULL);
	fclose(file);
	if (!key)
		return (NULL);

	/* Open file containing the pub key */
	snprintf(path, sizeof(path), "%s/%s", folder, PUB_FILENAME);
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
