/**
 * Copyright (c) 2011, Vasileios Daras. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 */

#include "huffman_encoding.h"
#include "binary_heap.h"
#include "huffman_tree.h"
#include "bitset.h"
#include <stdlib.h>

static void count_frequencies(FILE* fp, unsigned int frequencies[256]) {
    
    unsigned char byte;
    
    for(int i = 0; i < 256; i++) {
        frequencies[i] = 0;
    }
    
    while(fread(&byte, 1, sizeof(unsigned char), fp) != 0) {
        frequencies[(unsigned int)byte]++;
    }

    fseek(fp, 0, SEEK_SET);
}

static int create_lookup_recurse(huffman_node* curr_node, bitset* lookup[256], bitset* curr_path, int is_left, int depth) {
    if(is_left) {
        bitset_clear_bit(curr_path, depth);
    } else {
        bitset_set_bit(curr_path, depth);
    }

    if(curr_node->is_leaf) {
        bitset_copy_bits(curr_path, &lookup[curr_node->which_char], depth + 1);
    } else {
        create_lookup_recurse(curr_node->left, lookup, curr_path, 1, depth + 1);
        create_lookup_recurse(curr_node->right, lookup, curr_path, 0, depth + 1);
    }

    return HUFFMAN_SUCCESS;
}

static int create_lookup(huffman_node* huffman_root, bitset* lookup[256]) {
    bitset *curr_path;
    int creation_status = bitset_create(&curr_path, 30);
    if(creation_status == BITSET_ALLOC_ERROR) {
        return HUFFMAN_ALLOC_ERROR;
    }

    create_lookup_recurse(huffman_root->left, lookup, curr_path, 1, 0);
    create_lookup_recurse(huffman_root->right, lookup, curr_path, 0, 0);

    bitset_destroy(&curr_path); 

    return HUFFMAN_SUCCESS;
}

int huffman_compress_file(FILE* in, FILE* out, huffman_node* root) {

    bitset* lookup[256] = { NULL };
    int create_lookup_status = create_lookup(root, lookup);
    if(create_lookup_status != HUFFMAN_SUCCESS) {
        for(int i = 0; i < 256; i++) {
            if(lookup[i] != NULL) {
                bitset_destroy(&lookup[i]);
            }
        }

        return create_lookup_status;
    }

    fseek(in, 0, SEEK_SET);
    unsigned char byte, byte_out = 0;
    int bits_written = 0;
    int total_written = 0;

    huffman_tree_serialize(root, out);

    while(fread(&byte, 1, sizeof(unsigned char), in) != 0) {
        if(lookup[byte] == NULL) {
            return HUFFMAN_UNMAPPED_BYTE;
        }

        unsigned char mask;

        for(int i = 0; i < lookup[byte]->total_bits; i++) {
            unsigned int bit;
            bitset_get_bit(lookup[byte], i, &bit);

            mask = bit << (8 - bits_written - 1);
            bits_written++;

            byte_out |= mask;

            if(bits_written >= 8) {
                total_written += fwrite(&byte_out, 1, sizeof(unsigned char), out);
                bits_written = 0;
                byte_out = 0;
            }
        }
    }

    if(bits_written != 0) {
        total_written += fwrite(&byte_out, 1, sizeof(unsigned char), out);
    }

    for(int i = 0; i < 256; i++) {
        if(lookup[i] != NULL) {
            bitset_destroy(&lookup[i]);
        }
    }

    return HUFFMAN_SUCCESS;
}


int huffman_decompress_file(FILE* in, FILE* out) {

    fseek(in, 0, SEEK_SET);

    huffman_node* huffman_root;
    int deserialization_status = huffman_tree_deserialize(&huffman_root, in);
    if(deserialization_status != HUFFMAN_SUCCESS) {
        return deserialization_status;        
    } 

    int bits_read = 0;
    int total_read = 0;
    int curr_read = 0;

    huffman_node* curr = huffman_root;
    unsigned char byte;
    while((curr_read = fread(&byte, 1, sizeof(unsigned char), in)) != 0) {
        total_read += curr_read;
        bits_read = 0;
        while(bits_read < 8) {

            if(curr->is_leaf) {
                fwrite(&curr->which_char, 1, sizeof(unsigned char), out);
                curr = huffman_root;
            } else {
                int mask = 1 << (8 - bits_read - 1);
                int bit = (byte & mask) != 0; 
                if(bit == 0) {
                    curr = curr->left;
                } else {
                    curr = curr->right;
                }
                bits_read++;
            }
        } 
    }

    huffman_tree_destroy(&huffman_root);
    return HUFFMAN_SUCCESS;
}

int huffman_encode(FILE* in, FILE* out) {
    
    unsigned int frequencies[256];
    count_frequencies(in, frequencies);
    
    huffman_node* root;
    int retval = huffman_tree_create(&root, frequencies);
    if(retval != HUFFMAN_SUCCESS) {
        return retval;
    }

    int compression_status = huffman_compress_file(in, out, root);

    huffman_tree_destroy(&root);
    
    return compression_status;
}

int huffman_decode(FILE* in, FILE* out) {
    
    int decompression_status = huffman_decompress_file(in, out);

    return decompression_status;
}