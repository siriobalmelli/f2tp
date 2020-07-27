/**
 * test_hash.c
 *
 * Test libsodium hashing at various lengths,
 * for docs see: https://libsodium.gitbook.io/doc/hashing/generic_hashing
 *
 * (c) 2020 Sirio Balmelli
 */
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
 *
 * Notice the use of sodium-specific malloc, free and memcmp.
 */
int test_hash(unsigned int hash_len)
{
	int err_cnt = 0;
	int res;

	unsigned char *hash_single = alloca(hash_len);
	unsigned char *hash_multi = alloca(hash_len);
	crypto_generichash_state *multi_state = NULL;

	/**
	 * single hash
	 */
	res = crypto_generichash(hash_single, hash_len,
				(void *)hash_data, strlen(hash_data),
				NULL, 0);
	NB_die_if(res, "len %u: Failed to hash single", hash_len);

	/**
	 * multi hash
	 */
	multi_state = sodium_malloc(crypto_generichash_statebytes());
	NB_die_if(!multi_state, "len %u: Failed to alloc multi", hash_len);

	res = crypto_generichash_init(multi_state,
				NULL, 0, hash_len);
	NB_die_if(res, "len %u: Failed to init multi", hash_len);

	res = crypto_generichash_update(multi_state,
				(void *)hash_data, strlen(hash_data));
	NB_die_if(res, "len %u: Failed to update multi", hash_len);

	res = crypto_generichash_final(multi_state,
				hash_multi, hash_len);
	NB_die_if(res, "len %u: Failed to finalize multi", hash_len);

	/**
	 * compare single and multi
	 */
	NB_die_if(sodium_memcmp(hash_single, hash_multi, hash_len),
		"len %u: Hash inconsistency", hash_len);

die:
	sodium_free(multi_state);
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
