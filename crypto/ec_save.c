#include <sys/stat.h>
#include <sys/types.h>

#include "hblk_crypto.h"

/**
 * ec_save - Saves and existing EC key pair on the disk
 * @key: pointer to the EC key pair to be saved on disk
 * @folder: path to the folder in which to save the keys
 * Return: 1 upon success, 0 on failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	FILE *file;
	char path[512];

	if (!key || !folder)
		return (0);

	/* Create folder */
	if (mkdir(folder, 0700) == -1 && errno != EEXIST)
		return (0);

	/* Write the private key */
	sprintf(path, "%s/%s", folder, PRI_FILENAME);
	file = fopen(path, "w");
	if (!file)
		return (0);

	if (!PEM_write_ECPrivateKey(file, key, NULL, NULL, 0, NULL, NULL))
	{
		fclose(file);
		return (0);
	}

	/* Write the public key */
	sprintf(path, "%s/%s", folder, PUB_FILENAME);
	file = fopen(path, "w");
	if (!file)
		return (0);

	if (!PEM_write_EC_PUBKEY(file, key))
	{
		fclose(file);
		return (0);
	}
	fclose(file);

	return (1);
}
