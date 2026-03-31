#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    // Payload is an array of bytes.
    uint8_t data[32];
    size_t len;
} packet;

typedef enum { CALL_ABOVE, CALL_BELOW } states;

void make_pkt(packet *pkt, const uint8_t *data, size_t len) {
    if (!pkt) return;
    if (len > sizeof pkt->data) len = sizeof pkt->data;
    memcpy(pkt->data, data, len);
    pkt->len = len;
}

void extract(const packet *pkt, uint8_t *data, size_t *len) {
    if (!pkt || !data || !len) return;
    memcpy(data, pkt->data, pkt->len);
    *len = pkt->len;
}

packet sender(const uint8_t *data, size_t len, states state) {
    packet pkt = { .len = 0 };
    if (state == CALL_ABOVE) {
        make_pkt(&pkt, data, len);
    }
    return pkt;
}

// Returns true if a response packet is produced (in resp), false otherwise.
bool receiver(const packet *pkt, states state, uint8_t *data, size_t *len, packet *resp) {
    if (state == CALL_ABOVE) {
        if (!pkt) return false;
        extract(pkt, data, len);
    }
    return false;
}

int main() {
    const uint8_t msg[] = "Hello, World!";
    // Subtract 1 to remove NUL byte from string.
    // String is just used for convenient data creation.
    const size_t size = sizeof msg-1;

    printf("== SENDER ==\n");
    // `%zu` is for size_t.
    printf("Sending %zu bytes: ", size);
    fwrite(msg, 1, size, stdout);
    printf("\n");
    printf("============\n\n");

    packet pkt = sender((const uint8_t *)msg, size, CALL_ABOVE);

    // Allocate memory on receiver side.
    uint8_t recv_buf[32];
    size_t recv_len = 0;
    packet resp = { .len = 0 };
    bool has_resp = receiver(&pkt, CALL_ABOVE, recv_buf, &recv_len, &resp);

    printf("== RECEIVER ==\n");
    printf("Receiver got %zu bytes: ", recv_len);
    fwrite(recv_buf, 1, recv_len, stdout);
    printf("\n");
    printf("Response packet? %d\n", has_resp);
    printf("==============\n");

    return 0;
}
