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

#include "bitset.h"
#include <stdlib.h>
#include <string.h>

static const int SHORT_INT_SIZE = sizeof(unsigned short int);
static const int BUCKET_SIZE = sizeof(unsigned short int) * 8;

static int calculate_buffer_size(int bitset_size) {
    int total_buckets = (bitset_size / BUCKET_SIZE) + 1;
    return total_buckets * SHORT_INT_SIZE;
}

int bitset_create(bitset** bset, unsigned int size) {
	bitset* retval = malloc(sizeof(bitset));
	if(retval == NULL) {
		(*bset) = NULL;
		return BITSET_ALLOC_ERROR;
	}

	retval->total_bits = 0;

    int buffer_size = calculate_buffer_size(size);

    retval->bit_buffer = malloc(buffer_size);
    if(retval->bit_buffer == NULL) {
    	free(retval);
    	(*bset) = NULL;
    	return BITSET_ALLOC_ERROR;
    }

    memset(retval->bit_buffer, 0, buffer_size);
    retval->total_bits = size;
    (*bset) = retval;
    return BITSET_SUCCESS;
}

void bitset_destroy(bitset** bset) {
	free((*bset)->bit_buffer);
	free((*bset));
	(*bset) = NULL;
}

int bitset_set_bit(bitset* bset, unsigned int bit) {
	if(bit >= bset->total_bits) {
		return BITSET_OUT_OF_BOUNDS;
	}

    int bucket = bit / BUCKET_SIZE;
    int bit_index = BUCKET_SIZE - (bit - bucket * BUCKET_SIZE);

    int mask = 1 << (bit_index - 1);
    bset->bit_buffer[bucket] |= mask;

	return BITSET_SUCCESS;
}

int bitset_clear_bit(bitset* bset, unsigned int bit) {
	if(bit >= bset->total_bits) {
		return BITSET_OUT_OF_BOUNDS;
	}

    int bucket = bit / BUCKET_SIZE;
    int bit_index = BUCKET_SIZE - (bit - bucket * BUCKET_SIZE);

    int mask = ~(1 << (bit_index - 1));
    bset->bit_buffer[bucket] &= mask;

	return BITSET_SUCCESS;
}

int bitset_get_bit(bitset* bset, unsigned int bit, unsigned int* value) {
	if(bit >= bset->total_bits) {
		return BITSET_OUT_OF_BOUNDS;
	}

    int bucket = bit / BUCKET_SIZE;
    int bit_index = BUCKET_SIZE - (bit - bucket * BUCKET_SIZE);

    int mask = 1 << (bit_index - 1);
    (*value) = (bset->bit_buffer[bucket] & mask) != 0;	

	return BITSET_SUCCESS;
}

int bitset_resize(bitset* bset, int new_size) {

    int prev_buffer_size = calculate_buffer_size(bset->total_bits);

	int buffer_size = calculate_buffer_size(new_size);
	
    if(prev_buffer_size < buffer_size) {

		unsigned short int* temp = realloc(bset->bit_buffer, buffer_size); 

		if(temp == NULL) {
			return BITSET_ALLOC_ERROR;
		}

    	int prev_total_buckets = (prev_buffer_size / SHORT_INT_SIZE);
    	int new_buckets = (buffer_size / SHORT_INT_SIZE) - prev_total_buckets;

    	memset(temp + prev_total_buckets, 0, new_buckets * SHORT_INT_SIZE);

	    bset->bit_buffer = temp;
    }

    bset->total_bits = new_size;

    return BITSET_SUCCESS;
}

int bitset_copy(bitset* copy_of, bitset** copy) {
	int creation_status = bitset_create(copy, copy_of->total_bits);

	if(creation_status != BITSET_SUCCESS) {
		return creation_status;
	}

	int num_buckets = (copy_of->total_bits / BUCKET_SIZE) + 1;

	memcpy((*copy)->bit_buffer, copy_of->bit_buffer, num_buckets * SHORT_INT_SIZE);

	return BITSET_SUCCESS;
}

int bitset_copy_bits(bitset* copy_of, bitset** copy, int n_bits) {
    
	if(n_bits > copy_of->total_bits) {
		n_bits = copy_of->total_bits;
	}

    int creation_status = bitset_create(copy, n_bits);

	if(creation_status != BITSET_SUCCESS) {
		return creation_status;
	}

	int num_buckets = (n_bits / BUCKET_SIZE) + 1;

	memcpy((*copy)->bit_buffer, copy_of->bit_buffer, num_buckets * SHORT_INT_SIZE);

	return BITSET_SUCCESS;
}


void bitset_serialize(bitset* bset, FILE* fp) {

	fwrite(&bset->total_bits, 1, sizeof(unsigned int), fp);

	int buffer_size = calculate_buffer_size(bset->total_bits);
	fwrite(bset->bit_buffer, buffer_size, 1, fp);
}


int bitset_deserialize(bitset** bset, FILE* fp) {
	
	int bitset_size;
	fread(&bitset_size, 1, sizeof(unsigned int), fp);

	bitset* out;
	int creation_status = bitset_create(&out, bitset_size);
	if(creation_status != BITSET_SUCCESS) {
		return creation_status;
	} 

	int buffer_size = calculate_buffer_size(bitset_size);
	fread(out->bit_buffer, buffer_size, 1, fp);

	(*bset) = out;
	return BITSET_SUCCESS;
}

