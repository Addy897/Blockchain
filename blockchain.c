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
    struct Block* next;
} Block;

void sha256(const char* str, char* hash) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str, strlen(str), digest);
    
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hash[i * 2], "%02x", digest[i]);
    }
}

Block* create_block(int index, const char* previous_hash, const char* data) {
    Block* new_block = (Block*)malloc(sizeof(Block));
    new_block->index = index;
    strcpy(new_block->data, data);
    strcpy(new_block->previous_hash, previous_hash);
    new_block->timestamp=time(0);
    
    char str[1024];
    snprintf(str, sizeof(str), "%d%ld%s%s", index,new_block->timestamp, previous_hash, data);
    
    sha256(str, new_block->hash);
    new_block->next = NULL;
    
    return new_block;
}

void add_block(Block** head, int index, const char* data) {
    if (*head == NULL) {
        *head = create_block(index, "0", data);
        return;
    }
    
    Block* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = create_block(index, current->hash, data);
}

void print_blockchain(Block* head) {
    Block* current = head;
    while (current != NULL) {
        printf("Index: %d\n", current->index);
        printf("Previous Hash: %s\n", current->previous_hash);
        printf("Hash: %s\n", current->hash);
        printf("Timestamp: %ld\n", current->timestamp);
        printf("Data: %s\n\n", current->data);
        current = current->next;
    }
}
void verify_block(Block* block){
    char str[1024];
    snprintf(str, sizeof(str), "%d%ld%s%s", block->index,block->timestamp, block->previous_hash, block->data);
    char hash[65];
    sha256(str, hash);
    if(strncmp(block->hash,hash,strlen(hash))==0){
        printf("Block of index %d and data %s is valid\n",block->index,block->data);
    }else{
        printf("Block of index %d and data %s is invalid\n",block->index,block->data);

    }


}
int main() {
    Block* blockchain = NULL;

    add_block(&blockchain, 1, "Data 1");
    add_block(&blockchain, 2, "Data 2");
    verify_block(blockchain->next);

    add_block(&blockchain, 3, "Data 3");

    print_blockchain(blockchain);

    Block* current = blockchain;
    Block* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    return 0;
}

