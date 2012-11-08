struct peer_addr {
	in_addr_t addr;
	short port;
} __attribute__((packed));

struct peer_state {
	struct peer_state *next;
	in_addr_t ip;

	int socket;
	int connected;
	int received_handshake;

	unsigned char* bitfield;

	unsigned char* incoming; // buffer where we store partial messages
	unsigned int incoming_count; // number of bytes currently in the incoming buffer

	unsigned char* outgoing;
	unsigned int outgoing_count;

	int requested_piece;
	int requested_block;
	int balance;

	int choked;
};
