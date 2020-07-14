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
 * Header: leading bytes in packet (sent in the clear)
 */
struct header {
union {
struct {
	uint32_t	block_id;	/** monotonic, rollover */
	uint32_t	esi;
};
	uint64_t	seed1;		/** FEC seed1 */
};

union {
struct {
	uint32_t	payload_len;	/** in Bytes */
	uint16_t	sym_len;	/** in Bytes */
	uint8_t		fec_percent;	/**
					 * FEC percentage (multiplier in hundredths above 1.00):
					 * 'fec_percent * 0.01 + 1.0 == fec_ratio'
					 * 1 == 1.01; 30 == 1.3; 101 == 2.01; 255 == 3.55;
				    	 */
	uint8_t		entropy;	/** random number generated per-block */
};
	uint64_t	seed2; /** FEC seed2 */
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
/* TODO: link data and metadata blockchains? (2020-07-14, Sirio Balmelli) */



/**
 * Channel: a bundle of links (and their respective capabilities),
 *	and associated FEC parameters.
 *
 * TODO: needs retransmission after channel negotiation?
 */
struct channel {
	/* TODO: encryption keys (2020-07-13, Sirio Balmelli) */
	/* TODO: bloom filters (2020-07-12, Sirio Balmelli) */
	/* TODO: block buffer(s) (2020-07-12, Sirio Balmelli) */
	struct link	links[];
};



/* TODO: track filename, metadata, etc and send it as its own block (2020-07-13, Sirio Balmelli) */
/* TODO: some kind of a "protocol version" field or "protocol capabilities"? (2020-07-13, Sirio Balmelli) */
