#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

typedef struct Block {
    int index;
    char previous_hash[65];
    char hash[65];
    char data[256];
    time_t timestamp;
    int difficulty;
    unsigned long nonce;
    struct Block* next;
} Block;

void sha256(const char* str, char* hash) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str, strlen(str), digest);
    
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hash[i * 2], "%02x", digest[i]);
    }
}

void calculate_hash(Block* block) {
    char str[1024];
    snprintf(str, sizeof(str), "%d%ld%s%s%lu", block->index, block->timestamp, block->previous_hash, block->data, block->nonce);
    sha256(str, block->hash);
}

Block* create_block(int index, int difficulty, const char* previous_hash, const char* data) {
    Block* new_block = (Block*)malloc(sizeof(Block));
    new_block->index = index;
    strcpy(new_block->data, data);
    strcpy(new_block->previous_hash, previous_hash);
    new_block->difficulty = difficulty;
    new_block->nonce = 0;
    new_block->timestamp = time(0);
    new_block->next = NULL;

    calculate_hash(new_block);

    return new_block;
}

void add_block(Block** head, int index, int difficulty, const char* data) {
    if (*head == NULL) {
        *head = create_block(index, difficulty, "0", data);
        return;
    }
    
    Block* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = create_block(index, difficulty, current->hash, data);
}

void print_blockchain(Block* head) {
    Block* current = head;
    while (current != NULL) {
        printf("Index: %d\n", current->index);
        printf("Previous Hash: %s\n", current->previous_hash);
        printf("Hash: %s\n", current->hash);
        printf("Timestamp: %ld\n", current->timestamp);
        printf("Difficulty: %d\n", current->difficulty);
        printf("Nonce: %lu\n", current->nonce);
        printf("Data: %s\n", current->data);
        
        current = current->next;
    }
}

int is_valid_hash(const char* hash, int difficulty) {
    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != '0') {
            return 0;
        }
    }
    return 1;
}

void mine_block(Block* block) {
    char str[2048];
    char hash[65];
    
    while (1) {
        snprintf(str, sizeof(str), "%d%ld%s%s%lu", block->index, block->timestamp, block->previous_hash, block->data, block->nonce);
        
        sha256(str, hash);
        
        if (is_valid_hash(hash, block->difficulty)) {
            strcpy(block->hash, hash);
            printf("Block mined: %s\n", hash);
            break;
        }
        
        block->nonce++;
    }
}

int main() {
    Block* blockchain = NULL;

    add_block(&blockchain, 1, 4, "Data 1");
    add_block(&blockchain, 2, 3, "Data 2");
    add_block(&blockchain, 3, 3, "Data 3");

    Block* current = blockchain;
    while (current != NULL) {
        mine_block(current);
        current = current->next;
    }

    print_blockchain(blockchain);

    Block* next;
    current = blockchain;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    return 0;
}
