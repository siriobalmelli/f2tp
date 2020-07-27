#include <sodium.h>
#include <ndebug.h>
#include <nonlibc.h>

#include "f2tp.h"


const char *hash_data = "Hash this string please";

const unsigned int hash_lengths[] = {
	16,
	24,
	32,
	crypto_generichash_BYTES
};


/**
 * test hash success and consistency for a specific 'hash_len'
 */
int test_hash(unsigned int hash_len)
{
	int err_cnt = 0;
	int res;

	unsigned char *hashes[] = {
		alloca(hash_len),
		alloca(hash_len)
	};

	for (unsigned int i = 0; i < NLC_ARRAY_LEN(hashes); i++) {
		res = crypto_generichash(hashes[i], hash_len,
					(void *)hash_data, strlen(hash_data),
					NULL, 0);
		NB_die_if(res, "len %u: Failed to hash", hash_len);
	}

	NB_die_if(memcmp(hashes[0], hashes[1], hash_len),
		"len %u: Hash inconsistency", hash_len);
die:
	return err_cnt;
}


int main()
{
	int err_cnt = 0;
	int res;

	/**
	 * If this fails, libsodium has changed drastically
	 * and may no longer be compatible.
	 */
	NB_die_if(crypto_generichash_BYTES != 32,
		"Library hash size differs from design");

	res = sodium_init();
	NB_die_if(res < 0, "Failed to init crypto");

	for (unsigned int i = 0; i < NLC_ARRAY_LEN(hash_lengths); i++)
		err_cnt += test_hash(hash_lengths[i]);

die:
	return err_cnt;
}
