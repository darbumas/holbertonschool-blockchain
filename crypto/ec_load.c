#include <sys/stat.h>
#include <unistd.h>
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

	if (access(folder, F_OK) != 0)
		return (NULL);

	/* Open file containing the priv key */
	sprintf(path, "%s/%s", folder, "key.pem");
	file = fopen(path, "r");
	if (!file)
		return (NULL);

	/* Read the private key */
	key = PEM_read_ECPrivateKey(file, NULL, NULL, NULL);
	fclose(file);
	if (!key)
		return (NULL);

	/* Open file containing the pub key */
	sprintf(path, "%s/%s", folder, "key_pub.pem");
	file = fopen(path, "r");
	if (!file)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	/* Read the pub key */
	if (!PEM_read_EC_PUBKEY(file, &key, NULL, NULL))
	{
		EC_KEY_free(key);
		fclose(file);
		return (NULL);
	}
	fclose(file);

	return (key);
}
