/**
 * Copyright (c) 2011-2014, Vasileios Daras. All rights reserved.
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
 
#include "huffman_tree.h"
#include "binary_heap.h"
#include "bitset.h"

#include <stdio.h>
#include <stdlib.h>

static int compare_huffman_nodes(void* node1, void* node2) {
    huffman_node* huff_node_1 = (huffman_node*) node1;
    huffman_node* huff_node_2 = (huffman_node*) node2;
    
    return huff_node_1->frequency < huff_node_2->frequency ? 1 :
            huff_node_1->frequency == huff_node_2->frequency ? 0 : -1;
}

int huffman_node_create(huffman_node** node) {
    (*node) = malloc(sizeof(huffman_node));
    if((*node) == NULL) {
        return HUFFMAN_ALLOC_ERROR;
    }
    (*node)->which_char = 0;
    (*node)->frequency = 0;
    (*node)->is_leaf = 1;
    (*node)->left = (*node)->right = NULL;
    (*node)->parent = NULL;
    return HUFFMAN_SUCCESS;
}

static void huffman_node_destroy(void* node) {
    if(node == NULL)
        return;
    
    huffman_node* h_node = (huffman_node*) node;
    
    huffman_node_destroy(h_node->right);
    huffman_node_destroy(h_node->left);
    free(h_node);
}

/**
 * This function creates a Huffman tree using a heap. For each byte that has a frequency
 * greater than 0, a node is created that stores the byte and its frequency and gets
 * inserted to a heap. To construct the tree, two nodes with the highest priority
 * are extracted from the heap and a new node is created with the extracted nodes as
 * its left and right children. This is repeated until only one node is left in the heap 
 * which is the root of the Huffman tree.
 */
int huffman_tree_create(huffman_node** root, unsigned int frequencies[256]) {
    
    int retval = HUFFMAN_TREE_EMPTY;
    binary_heap* heap = NULL;
    int heap_creation_status = binary_heap_create(&heap, compare_huffman_nodes, huffman_node_destroy);
    if(heap_creation_status == BINARYHEAP_ALLOC_ERROR) {
        return HUFFMAN_ALLOC_ERROR;
    }
    
    int node_count = 0;
    for(unsigned int i = 0; i < 256; i++) {
        huffman_node* node;
        
        if(frequencies[i] != 0) {
            retval = huffman_node_create(&node);
            if(retval == HUFFMAN_ALLOC_ERROR) {
                binary_heap_destroy(&heap);
                return HUFFMAN_ALLOC_ERROR;
            }
            
            node->which_char = (unsigned char) i;
            node->frequency = frequencies[i];
            
            retval = binary_heap_insert(heap, node);
            if(retval == BINARYHEAP_ALLOC_ERROR) {
                huffman_node_destroy(node);
                binary_heap_destroy(&heap);
                return HUFFMAN_ALLOC_ERROR;
            }
            
            node_count++;
        }
    }
    
    for(int i = 0; i < node_count - 1; i++) {
        huffman_node* new_node = NULL;
        retval = huffman_node_create(&new_node);
       
        if(retval == BINARYHEAP_ALLOC_ERROR) {
            binary_heap_destroy(&heap);
            return HUFFMAN_ALLOC_ERROR;
        }
        
        void* data;
        huffman_node* left;
        huffman_node* right;
        
        binary_heap_extract(heap, &data);
        left = (huffman_node*) data;
        new_node->left = left;
        
        binary_heap_extract(heap, &data);
        right = (huffman_node*) data;
        new_node->right = right;

        left->parent = right->parent = new_node;
        
        new_node->frequency = left->frequency + right->frequency;
        new_node->is_leaf = 0;
        
        if(binary_heap_insert(heap, new_node) == BINARYHEAP_ALLOC_ERROR) {
            //this will destroy the right and left subtrees too
            huffman_node_destroy(new_node);

            binary_heap_destroy(&heap);
            return HUFFMAN_ALLOC_ERROR;
        }
    }
    
    void* root_huffman_node = NULL;

    int extraction_status = binary_heap_extract(heap, &root_huffman_node);
    if(extraction_status != BINARYHEAP_EMPTY) {
        retval = HUFFMAN_SUCCESS;
    }

    binary_heap_destroy(&heap);
    
    (*root) = (huffman_node*) root_huffman_node;
    return retval;
}

void huffman_tree_destroy(huffman_node** root) {
    huffman_node_destroy(*root);
    
    (*root) = NULL;
}

static int check_and_resize_bitset(bitset* bset, int bits_stored) {
    if(bits_stored >= bset->total_bits) {
        int resize_status = bitset_resize(bset, bset->total_bits * 2);
        if(resize_status == BITSET_ALLOC_ERROR) {
            return HUFFMAN_ALLOC_ERROR;
        }
    }

    return HUFFMAN_SUCCESS;
} 

/**
 * This function creates a binary representation of the huffman tree in order to serialize it to a file. 
 * The binary representation is created recursively by traversing the tree in an in-order fashion.
 * For each non-leaf node encountered a 0 is stored in the binary representation, whereas for the leaf
 * nodes an 1 is stored followed by the byte assigned to the node.
 */
static int huffman_tree_serialize_recurse(huffman_node* curr_node, bitset* tree_binary_rep, int *bits_stored) {

    int resize_status = check_and_resize_bitset(tree_binary_rep, *bits_stored);
    if(resize_status != HUFFMAN_SUCCESS) {
        return resize_status;
    }

    if(curr_node->is_leaf) {
        bitset_set_bit(tree_binary_rep, *bits_stored);
        (*bits_stored)++;

        unsigned char byte = curr_node->which_char;
        for(int i = 0; i < 8; i++) {
            unsigned char mask = 1 << (8 - i - 1);
            int bit = (byte & mask) != 0;
            
            resize_status = check_and_resize_bitset(tree_binary_rep, *bits_stored);
            if(resize_status != HUFFMAN_SUCCESS) {
                return resize_status;
            }

            if(bit == 1) {
                bitset_set_bit(tree_binary_rep, *bits_stored);
            } else {
                bitset_clear_bit(tree_binary_rep, *bits_stored);
            }

            (*bits_stored)++;
        }

    } else {
        bitset_clear_bit(tree_binary_rep, *bits_stored);
        (*bits_stored)++;

        int left_status = huffman_tree_serialize_recurse(curr_node->left, tree_binary_rep, bits_stored);
        if(left_status != HUFFMAN_SUCCESS) {
            return left_status;
        }

        int right_status = huffman_tree_serialize_recurse(curr_node->right, tree_binary_rep, bits_stored);
        if(right_status != HUFFMAN_SUCCESS) {
            return right_status;
        }
    }
 
    return HUFFMAN_SUCCESS;
}

int huffman_tree_serialize(huffman_node* root, FILE* fp) {

    bitset* bset;
    int bitset_creation_status = bitset_create(&bset, 30);
    if(bitset_creation_status == BITSET_ALLOC_ERROR) {
        return HUFFMAN_ALLOC_ERROR;
    }

    int bits_stored = 0;
    int serialization_status = huffman_tree_serialize_recurse(root, bset, &bits_stored);
    if(serialization_status != HUFFMAN_SUCCESS) {
        bitset_destroy(&bset);
        return serialization_status;
    }

    bitset_serialize(bset, fp);
    bitset_destroy(&bset);
    return HUFFMAN_SUCCESS;
}


static int huffman_tree_deserialize_recurse(huffman_node* curr_node, bitset* tree_binary_rep, int *bits_read) {
    if((*bits_read) >= tree_binary_rep->total_bits) {
        return HUFFMAN_SUCCESS;
    }

    int node_creation_status, deserialization_status;

    unsigned int curr_bit;
    bitset_get_bit(tree_binary_rep, *bits_read, &curr_bit);
    (*bits_read)++;

    if(curr_bit == 0) {
        
        huffman_node* new_node;
        node_creation_status = huffman_node_create(&new_node);
        if(node_creation_status != HUFFMAN_SUCCESS) {
            return node_creation_status;
        }

        deserialization_status = huffman_tree_deserialize_recurse(new_node, tree_binary_rep, bits_read);
        if(deserialization_status != HUFFMAN_SUCCESS) {
            huffman_node_destroy(new_node);
            return deserialization_status;
        }

        curr_node->left = new_node;

        node_creation_status = huffman_node_create(&new_node);
        if(node_creation_status != HUFFMAN_SUCCESS) {
            return node_creation_status;
        }

        deserialization_status = huffman_tree_deserialize_recurse(new_node, tree_binary_rep, bits_read);
        if(deserialization_status != HUFFMAN_SUCCESS) {
            huffman_node_destroy(new_node);
            return deserialization_status;
        }

        curr_node->right = new_node;

        curr_node->is_leaf = 0;
    } else {

        unsigned char byte = 0;

        
        for(int i = 0; i < 8; i++) {
            
            unsigned int bit;
            int bit_read_status = bitset_get_bit(tree_binary_rep, *bits_read, &bit);
            if(bit_read_status == BITSET_OUT_OF_BOUNDS) {
                return HUFFMAN_ENCODING_ERROR;
            }
            (*bits_read)++;

            if(bit == 1) {  
                unsigned char mask = 1 << (8 - i - 1);      
                byte |= mask;
            }
        }

        curr_node->which_char = byte;
    }

    return HUFFMAN_SUCCESS;
}

int huffman_tree_deserialize(huffman_node** root, FILE* fp) {

    bitset* tree_binary_rep;
    int deserialization_status = bitset_deserialize(&tree_binary_rep, fp);
    if(deserialization_status == BITSET_ALLOC_ERROR) {
        return HUFFMAN_ALLOC_ERROR;
    }

    huffman_node* temp_root;
    int node_creation_status = huffman_node_create(&temp_root);
    if(node_creation_status == HUFFMAN_ALLOC_ERROR) {
        bitset_destroy(&tree_binary_rep);
        return HUFFMAN_ALLOC_ERROR;
    }

    int bits_read = 0;
    deserialization_status = huffman_tree_deserialize_recurse(temp_root, tree_binary_rep, &bits_read);
    if(deserialization_status != HUFFMAN_SUCCESS) {
        huffman_tree_destroy(&temp_root);
        bitset_destroy(&tree_binary_rep);
        return deserialization_status;
    }

    (*root) = temp_root;
    bitset_destroy(&tree_binary_rep);
    return HUFFMAN_SUCCESS;
}
