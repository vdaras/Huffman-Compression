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

#ifndef BITSET_H
#define BITSET_H

#include <stdio.h>

#define BITSET_SUCCESS 0
#define BITSET_ALLOC_ERROR -1
#define BITSET_OUT_OF_BOUNDS -2

typedef struct {
    unsigned short int *bit_buffer;
    unsigned int total_bits;
} bitset;

/**
 * Creates a bitset.
 *
 * @param bset(out) Created bitset is stored here. NULL if creation fails.
 * @param size Bitset's size.
 * @return A flag indicating if creation was successful.
 */
int bitset_create(bitset **bset, unsigned int size);

/**
 * Destroys a bitset.
 *
 * @param bset Bitset to destroy, set to NULL after the call.
 */
void bitset_destroy(bitset **bset);

/**
 * Sets a bit in the bitset to 1.
 *
 * @param bset 
 * @param bit Bit to set.
 * @return A flag indicating if setting the bit was successful.
 */
int bitset_set_bit(bitset* bset, unsigned int bit);

/**
 * Clears a bit in the bitset to 0.
 *
 * @param bset 
 * @param bit Bit to clear.
 * @return A flag indicating if setting the bit was successful.
 */
int bitset_clear_bit(bitset* bset, unsigned int bit);

/**
 * Gets a bit's value from a bitset.
 *
 * @param bset The bitset.
 * @param bit Bit to get value from.
 * @param value(out) Bit's value is stored in here.
 * @return A flag indicating if retrieving the bit was successful.
 */
int bitset_get_bit(bitset* bset, unsigned int bit, unsigned int* value);

/**
 * Resizes a bitset. If size is smaller the new bits
 *
 * @param bset Bitset to resize.
 * @param new_size Bitset's new size.
 */
int bitset_resize(bitset* bset, int new_size);

/**
 * Copies a bitset.
 * 
 * @param copy_of Copies this bitset.
 * @param copy(out) Stores copy in here.
 * @return A flag indicating if copying the bitset was successful.
 */
int bitset_copy(bitset* copy_of, bitset** copy);


/**
 * Copies first n bits of a bitset.
 * 
 * @param copy_of Copies this bitset.
 * @param copy(out) Stores copy in here.
 * @return A flag indicating if copying the bitset was successful.
 */
int bitset_copy_bits(bitset* copy_of, bitset** copy, int n_bits);

/**
 * Serializes the bitset to a binary file. First writes an integer
 * with the size of the bitset and then the bitset's buffer.
 *
 * @param bset Bitset to serialize.
 * @param fp File to serialize the bitset to.
 */
void bitset_serialize(bitset* bset, FILE* fp);

/**
 * Reads a bitset from a binary file. 
 * 
 * @param bset(out) Deserialized bitset is stored in here.
 * @param fp File to read bitset from.
 * @return A flag indicating if deserialization was successful.
 */
int bitset_deserialize(bitset** bset, FILE* fp);

#endif