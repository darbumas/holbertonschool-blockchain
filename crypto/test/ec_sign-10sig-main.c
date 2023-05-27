/* Description: Generates 10 signatures of the same string */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hblk_crypto.h"

EC_KEY *_ec_create(void);
int _ec_verify(EC_KEY const *key, uint8_t const *msg,
	size_t msglen, sig_t const *sig);

static int test_ec_sign(EC_KEY const *key)
{
	unsigned char const str[] = "Holberton School";
	sig_t sig;

	if (!ec_sign(key, str, strlen((char *)str), &sig))
	{
		printf("ec_sign() failed\n");
		return (EXIT_FAILURE);
	}

	if (!_ec_verify(key, str, strlen((char *)str), &sig))
	{
		printf("Signature verification failed\n");
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	EC_KEY *key;
	int i;

	/* Prerequisites */
	key = _ec_create();
	if (!key)
	{
		printf("ec_create() failed\n");
		return (EXIT_FAILURE);
	}

	/* Test `ec_sign()` */
	for (i = 0; i < 10; i++)
	{
		if (test_ec_sign(key) != EXIT_SUCCESS)
		{
			EC_KEY_free(key);
			return (EXIT_FAILURE);
		}
	}

	/* Cleanup */
	EC_KEY_free(key);

	return (EXIT_SUCCESS);
}
