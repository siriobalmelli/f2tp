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



/**
 * Header: packet header (sent in the clear)
 */
struct header {
union {
struct {
	uint32_t	block_id; /** monotonic, rollover */
	uint32_t	esi;
};
	uint64_t	seed1;
};

union {
struct {
	uint32_t	block_len; /** data length (excluding 'struct leader' and padding), in Bytes */
	uint32_t	random; /** entropy */
};
	uint64_t	seed2;
};
}



/**
 * Packet: data packet over the wire
 */
struct packet {
	struct header	head; /** AEAD: authenticated */
	uint8_t		symbol[]; /** sym_len; encrypted */
	/* TODO: AEAD hash (2020-07-13, Sirio Balmelli) */
};



/**
 * Leader (aka block header): first symbol in a block describes the block (esi == 0)
 */
struct leader {
	struct header	head;

	uint8_t		hash[32]; /* data hash (excluding 'struct leader' and padding) */

	uint32_t	prev; /* block_id of previous block; self.head.block_id == beginning of sequence */
	uint32_t	next; /* block_id of next block; self.head.block_id == end of sequence */

	uint8_t		pad[]; /** pad to sym_len */
};



/**
 * Channel: a bundle of links (and their respective capabilities),
 *	and associated FEC parameters.
 *
 * TODO: needs retransmission after channel negotiation?
 */
struct channel {
	/* TODO: struct ffec_params (2020-07-13, Sirio Balmelli) */
	double		fec_ratio;
	uint32_t	sym_len;

	/* TODO: encryption keys (2020-07-13, Sirio Balmelli) */
	/* TODO: bloom filters (2020-07-12, Sirio Balmelli) */
	/* TODO: blocks cbuf (2020-07-12, Sirio Balmelli) */
	struct link	links[];
};



/* TODO: track filename, metadata, etc and send it as its own block (2020-07-13, Sirio Balmelli) */
/* TODO: some kind of a "protocol version" field or "protocol capabilities"? (2020-07-13, Sirio Balmelli) */
