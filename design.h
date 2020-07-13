/**
 * design.h
 *
 * data structure scratchpad
 * (c) 2020 Sirio Balmelli
 */

#include <stdint.h>


/**
 * Link capability flags
 */
enum link_cap {
	LINK_CAP_BIDIR		= 0x1,
	LINK_CAP_MULTICAST	= 0x2,
	LINK_CAP_MGMT		= 0x4,
	LINK_CAP_DATA		= 0x8,
};

/**
 * Link: unicast or multicast 'interface -> interface' path
 */
struct link {
	/* TODO: source IP/port (2020-07-12, Sirio Balmelli) */
	/* TODO: dest IP/port (2020-07-12, Sirio Balmelli) */
	enum link_cap	capability;
	/* TODO: per-link loss/quality data (2020-07-12, Sirio Balmelli) */
};


enum block_flags {
	BLOCK_LAST	= 0x1, /** _next points to first block */
};

typedef uint8_t hash_t[32];
typedef uint8_t sig_t[64];

/**
 * A single FEC block, containing a message and necessary padding
 *
 * TODO: must be < FEC symbol size
 */
struct block {
	uint16_t	id; /** sequential */
	uint16_t	id_next; /** allow id rollover with a large message */
	/* TODO: how large does block_id need to be not to rollover? (2020-07-12, Sirio Balmelli) */
	/* TODO: can use block_id as IV for block FEC? (2020-07-12, Sirio Balmelli) */
	uint16_t	data_cnt; /** how many symbols contain data (vs. how many are padding) */
	uint16_t	last_len; /** actual data length in last symbol (followed by random padding) */
	/* TODO: what to use as seed for padding generation? (2020-07-12, Sirio Balmelli) */

	hash_t		hash;
	hash_t		hash_next;
	sig_t		signature; /** validate this structure */

	uint8_t		random[]; /** pad to symbol size */
};


/**
 * Channel: a bundle of links (and their respective capabilities),
 *	and associated FEC parameters.
 *
 * TODO: needs retransmission after channel negotiation?
 */
struct channel {
	/* TODO: FEC params (2020-07-12, Sirio Balmelli) */
	/* TODO: bloom filters (2020-07-12, Sirio Balmelli) */
	/* TODO: blocks cbuf (2020-07-12, Sirio Balmelli) */
	struct link	links[];
};


/**
 * A message always *begins* a FEC block.
 */
struct message {
	uint16_t	len;
	/* TODO: multi-message tracking (2020-07-13, Sirio Balmelli) */
	/* TODO: can coalesce into block (using a chain?) (2020-07-13, Sirio Balmelli) */
};
