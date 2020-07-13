# Fast FEC Transport Protocol library (f2tp)

*STATUS*: pre-Alpha

This is a network transport library with the goal of providing low-overhead
data transfer:

- over either uni- or bi-directional links
- split over multiple links
- as unicast or multicast (per-link)
- employing FEC to reduce retransmission requirements
    (e.g. <https://github.com/siriobalmelli/fastfec>)
- guaranteeing data integrity (checksumming)
- pipelining: block queue, network pipe always full
- performant: can saturate very fast links (eg 10gbit)
- respecting linux kernel internals and TC queues: no packet drop

## Caveats

- linux-only
- IPv4 (may be extended, but not conducive to lowest-overhead)
- not encrypted, for use on internal networks
    (provisionally: this may change if an effective AEAD mode is convenient)
- not a standardized network protocol at this time

## Design Concepts

1. All data can be expressed as *slices*:
    ```yaml
    0xab1234ff:  # AEAD of slice contents?
        - pki: 0xffaabbccddee  # public key of tx
        - seq: 42  # disambiguate repeating slices with changed data
        - src: /mnt/media/file.mp4  # read path @tx
        - dst: /mnt/san/file.mp4  # write path @rx
        - oft: 0  # beginning of file
        - len: 724545676  # end of file
    ```

    - UUID must not depend on data or hash thereof

### Zero-copy architecture

1. Combination of a circular buffer (eg <https://github.com/siriobalmelli/memorywell>)
and `splice()`

1. Work directly with `io_uring` buffers (see <https://unixism.net/loti>)

### IP Protocol

*TODO* evaluate between:

- UDP with [protobuf](https://github.com/protocolbuffers/protobuf) headers
- [SCTP](https://linux.die.net/man/7/sctp)
- [NEAT](https://github.com/NEAT-project/neat)

### Other Ideas

1. fast in-line compression to improve bandwidth usage?
    - https://github.com/lz4/lz4
    - see https://i.stack.imgur.com/LPCSe.png

## Comparison with other file/data transfer technologies:

1. FTP/SSH (and layers above, eg. rsync):
    - TCP: congestion; cannot do unidirectional links

1. QUIC:
    - see https://lwn.net/Articles/814522/ and https://github.com/lucas-clemente/quic-go
    - FEC is still academic/experimental: http://export.arxiv.org/pdf/1904.11326
    - no explicit support for unidirectional links

1. [uftp](https://github.com/shoki/uftp):
    - no inherent support for queuing multiple files (filling the pipe)
    - generalized for cross-platform (no zero-copy),
        unsuitable for saturating very fast links >1gbps

1. [toss](https://github.com/zerotier/toss):
    - TCP; unclear whether it can be quickly extended to UDP

1. [wormhole](https://github.com/warner/magic-wormhole):
    - not performance oriented
    - may be very useful to examine for its crypto/ID concepts

1. [Syncthing](https://github.com/syncthing/syncthing):
    - not a library architecture
    - TCP only (except for discovery done via UDP)
    - may be useful to examine how it handles partner discovery
