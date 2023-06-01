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
	FILE *fp = NULL;
	EC_KEY *key = NULL;
	char path[512] = {0};

	if (!folder || access(folder, F_OK) != 0)
		return (NULL);

	key = EC_KEY_new_by_curve_name(EC_CURVE);

	/* Open file containing the private key */
	sprintf(path, "%s/%s", folder, PRI_FILENAME);
	fp = fopen(path, "r");
	if (!fp)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	/* Read the private key */
	if (!PEM_read_ECPrivateKey(fp, &key, NULL, NULL))
	{
		fclose(fp);
		EC_KEY_free(key);
		return (NULL);
	}
	fclose(fp);

	return (key);
}
