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

#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#define BINARYHEAP_EMPTY 1
#define BINARYHEAP_SUCCESS 0
#define BINARYHEAP_ALLOC_ERROR -1
#define BINARYHEAP_EMPTY_ERROR -2


typedef struct {
    int size;
    int storage_size;
    void** tree;
    
    int (*comparator)(void* node1, void* node2);
    void (*destroy)(void* data);
} binary_heap;

/**
 * Creates a binary heap.
 * 
 * @param heap(out) heap is stored here. NULL if creation fails.
 * @param comparator function to compare nodes to each other.
 * @param destroy function used to destroy each node inserted.
 * @return A flag indicating if creation was successful.
 */ 
int binary_heap_create(binary_heap** heap, int (*comparator)(void*, void*), void (*destroy)(void*));

/**
 * Destroys a binary heap.
 * 
 * @param heap(out): Binary heap to destroy, set to NULL after destruction.
 */
void binary_heap_destroy(binary_heap** heap);

/**
 * Inserts a new element to the binary heap.
 * 
 * @param heap
 * @param data
 */
int binary_heap_insert(binary_heap* heap, const void* data);


/**
 * Extracts element from the heap.
 * 
 * @param heap Heap to extract element from.
 * @param data(out) Data extracted stored in here.
 * @return A flag indicating if extraction was successful.
 */
int binary_heap_extract(binary_heap* heap, void** data);

#endif //BINARY_HEAP_H