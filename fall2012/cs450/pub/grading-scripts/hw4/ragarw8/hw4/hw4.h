struct peer_addr {
	in_addr_t addr;
	short port;
} __attribute__((packed));

struct peer_state {
	struct peer_state *next;
	in_addr_t ip;
	unsigned int port;

	int socket;
	int connected;
	int connecting;
    int connect_retries;
	char* bitfield;
	char* incoming; // buffer where we store partial messages
	char* outgoing; // buffer where we store partial messages
	int requested_piece;

	int count; // number of bytes currently in the incoming buffer
	int send_count;
	int choked;
	int not_interested;
    int handshake_pending;
    int sockid;

    int choked_by_me;

    int pieces_sent;
    int pieces_rxed;
    int last_piece_rarity; /* a lower number means rare (aka piece_count)*/
    int rarity_average; /* a lower number means rare */

    //int piece_download_rate;
    //int piece_upload_rate;
};
