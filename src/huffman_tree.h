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

#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <stdio.h>

#define HUFFMAN_SUCCESS 0
#define HUFFMAN_ALLOC_ERROR -1
#define HUFFMAN_ENCODING_ERROR -2

typedef struct huffman_node_t {
    unsigned char which_char;
    int frequency;
    int is_leaf;
    struct huffman_node_t *parent;
    struct huffman_node_t *left, *right;
} huffman_node;


/**
 * Creates a huffman tree.
 * 
 * @param root Huffman tree root node is stored in here.
 * @param frequencies Array of frequencies for each byte from 0 to 255
 * @return A flag indicating if creation was successful.
 */
int huffman_tree_create(huffman_node** root, unsigned int frequencies[256]);

/**
 * Destroys a huffman tree.
 * 
 * @param root Tree to destroy.
 */
void huffman_tree_destroy(huffman_node** root);

/**
 * Serializes the huffman tree to a file.
 *
 * @param root The root of the huffman tree.
 * @param fp File to write the huffman tree to.
 * @return A flag indicating if serialization was successful.
 */
int huffman_tree_serialize(huffman_node* root, FILE* fp);

/**
 * Deserializes a huffman tree from a file.
 *
 * @param root(out) Huffman tree's root node is stored in here.
 * @param fp File to read the tree from.
 * @return A flag indicating if deserialization was successful.
 */
int huffman_tree_deserialize(huffman_node** root, FILE* fp);

#endif //HUFFMAN_TREE_H