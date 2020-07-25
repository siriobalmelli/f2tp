/**
 * design.h
 *
 * data structure scratchpad
 * (c) 2020 Sirio Balmelli
 */

#include <stdint.h>
#include <stddef.h>



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
 * Channel: a bundle of links (and their respective capabilities),
 *	and associated FEC parameters.
 *
 * TODO: needs retransmission after channel negotiation?
 */
struct channel {
	uint64_t	id;
	/* TODO: bloom filters (2020-07-12, Sirio Balmelli) */
	/* TODO: block buffer(s) (2020-07-12, Sirio Balmelli) */
	struct link	links[];
};

uint64_t channel_new();



typedef uint8_t hash_t[32]; /* TODO: fix this (2020-07-21, Sirio Balmelli) */

enum header_version {
	HEADER_PLAIN_BLAKE2b  = 0x1,	/** plaintext transmission, keyless crypto_generichash() (BLAKE2b) */
}__attribute__((packed));

/**
 * Header: leading bytes in packet (sent in the clear)
 */
struct header {
union {
struct {
	uint32_t	block_id;	/** monotonic, rollover */
	uint16_t	sym_len;	/** in Bytes */
	uint8_t		fec_percent;	/**
					 * FEC percentage (multiplier in hundredths above 1.00):
					 * 'fec_percent * 0.01 + 1.0 == fec_ratio'
					 * 1 == 1.01; 30 == 1.3; 101 == 2.01; 255 == 3.55;
					 */
	enum header_version version;	/** verify protocol version compatibility */
}__attribute__((packed));
	uint64_t	seed1;		/** FEC seed1 */
};

union {
struct {
	uint32_t	payload_len;	/** in Bytes */
	uint32_t	entropy;	/** random number generated per-block */
}__attribute__((packed));
uint64_t		seed2;		/** FEC seed2 (random data) */
};

struct {
	uint32_t	esi;
}__attribute__((packed));

hash_t			hash;		/** packet hash (future: AEAD signature) */
};
/* TODO: assert compile-time length (2020-07-21, Sirio Balmelli) */



/**
 * Packet: data packet over the wire
 */
struct packet {
	struct header	head; /** (future: cleartext IV + AD) */
	uint8_t		symbol[]; /** sym_len (future: cyphertext) */
};



/** cumulative flags */
enum message_stages {
	MESSAGE_TX_START	= 0x0001,	/** message logged for TX */
	MESSAGE_TX_KNOWN	= 0x0002,	/** at least one 'struct message' sent (seen by network) */
	MESSAGE_TX_DATA		= 0x0004,	/** all 'data' blocks sent */
	MESSAGE_TX_META		= 0x0008,	/** all 'meta' blocks sent */
	MESSAGE_CANCEL		= 0x0010,	/** cancelled by TX */

	/* reserved 0x1001 */
	MESSAGE_RX_KNOWN	= 0x1002,	/** at least one 'struct message' received */
	MESSAGE_RX_DATA		= 0x1004,	/** all 'data' blocks decoded (or none present) */
	MESSAGE_RX_META		= 0x1008,	/** all 'meta' blocks decoded (or none present) */
};

/**
 * Message: higher-level atom of data transfer
 */
struct message {
	uint64_t		id; /** arbitrary, unique */

	uint32_t		blk_data_id; /** ID of first data block */
	uint32_t		blk_data_cnt; /** how many data blocks */

	uint32_t		blk_meta_id;
	uint32_t		blk_meta_cnt;
};



/**
 * Leader (aka block header): first symbol in a block describes the block (esi == 0)
 */
struct leader {
	struct header	head;

	hash_t		hash; /** data hash (excluding 'struct leader' and padding) */
	uint32_t	id_prev; /** block_id of previous block; self.head.block_id == beginning of sequence */
	uint32_t	id_next; /** block_id of next block; self.head.block_id == end of sequence */

	struct message	message; /** all blocks belonging to a message carry the same message */
	enum message_stages stages; /** current stage at time of transfer */

	uint8_t		pad[]; /** pad to sym_len */
};



typedef void (*message_status_t)(uint64_t message_id, enum message_stages stages, void *context);

/** actively poll for status */
message_status_t message_status;

/* returns message_id */
uint64_t message_tx(struct channel *chan,
			const void *data, size_t data_len,
			const void *meta, size_t meta_len,
			message_status_t callback, void *context);

int message_listen(struct channel *chan, message_status_t callback, void *context);

int message_rx(struct channel *chan, uint64_t message_id,
			void **data, size_t *data_len,
			void **meta, size_t *meta_len);
