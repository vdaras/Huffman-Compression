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

#include "binary_heap.h"
#include <stdlib.h>

static int binary_heap_get_parent(int node) {
    return (node - 1) / 2;   
}

static int binary_heap_get_left(int node) {
    return node * 2 + 1;
}

static int binary_heap_get_right(int node) {
    return node * 2 + 2;
}

static int binary_heap_increase_storage(binary_heap* heap) {
    
    if(heap->storage_size == 0) {
        heap->tree = malloc(sizeof(void *));
        if(heap->tree == NULL) {
            return BINARYHEAP_ALLOC_ERROR;
        }
        heap->storage_size = 1;
    } else {
        if(heap->size + 1 >= heap->storage_size) {
            int new_storage_size = heap->storage_size * 2;
            void **temp_storage = realloc(heap->tree, sizeof(void *) * new_storage_size);
            if(temp_storage == NULL) {
                return BINARYHEAP_ALLOC_ERROR;
            }
            
            heap->tree = temp_storage;
            heap->storage_size = new_storage_size;
        }
    }    
    
    return BINARYHEAP_SUCCESS;
}

static int binary_heap_decrease_storage(binary_heap* heap) {
    
    if(heap->storage_size == 0) {
        return BINARYHEAP_EMPTY_ERROR;
    }
    else if(heap->storage_size == 1) {
        free(heap->tree);
        heap->tree = 0;
        heap->storage_size = 0;
    }
    else {
        if(heap->size - 1 < heap->storage_size / 2) {
            int new_storage_size = heap->storage_size / 2;
            void **temp_storage = realloc(heap->tree, sizeof(void *) * new_storage_size);
            if(temp_storage == NULL) {
                return BINARYHEAP_ALLOC_ERROR;
            }
            
            heap->tree = temp_storage;
            heap->storage_size = new_storage_size;
        }
    }    
    
    return BINARYHEAP_SUCCESS;
}

int binary_heap_create(binary_heap** heap, int (*comparator)(void*, void*), void (*destroy)(void*)) {

    (*heap) = malloc(sizeof(binary_heap));
    if((*heap) == NULL) {
        return BINARYHEAP_ALLOC_ERROR;
    }
    
    (*heap)->size = 0;
    (*heap)->storage_size = 0;
    (*heap)->tree = NULL;
    
    (*heap)->comparator = comparator;
    (*heap)->destroy = destroy;
    
    return BINARYHEAP_SUCCESS; 
}

void binary_heap_destroy(binary_heap** heap) {
    
    binary_heap* temp = (*heap);
    
    if(temp->tree != NULL) {
        if(temp->destroy != NULL) {
            for(int i = 0; i < temp->size; i++) {
                temp->destroy(temp->tree[i]);
            }
        }
    
        free(temp->tree);
    }
    
    free(temp);
    (*heap) = NULL;
}

int binary_heap_insert(binary_heap* heap, const void* data) {

    int storage_status = binary_heap_increase_storage(heap);
    
    if(storage_status != BINARYHEAP_SUCCESS) {
        return storage_status;
    }
    
    int new_element_pos = heap->size;
    int parent_element_pos = binary_heap_get_parent(new_element_pos);
    
    heap->tree[new_element_pos] = (void *) data;
    
    while(new_element_pos > 0 
       && heap->comparator(heap->tree[parent_element_pos], heap->tree[new_element_pos]) < 0) {
        void* temp = heap->tree[parent_element_pos];
        heap->tree[parent_element_pos] = heap->tree[new_element_pos];
        heap->tree[new_element_pos] = temp;
        
        new_element_pos = parent_element_pos;
        parent_element_pos = binary_heap_get_parent(new_element_pos);
    }
    
    heap->size++;
    
    return 0;
}

int binary_heap_extract(binary_heap* heap, void** data) {
    if(heap->size == 0) {
        return BINARYHEAP_EMPTY;
    }
    
    (*data) = heap->tree[0];
    void* save = heap->tree[heap->size - 1];

    int storage_status = binary_heap_decrease_storage(heap);
    
    if(storage_status != BINARYHEAP_SUCCESS) {
        return storage_status;
    }
 
    heap->size--;
   
    int m_pos = 0;
    
    if(heap->size > 0) {
        
        heap->tree[0] = save;
        
        int curr_pos = 0;
        
        while(1) {
            int left_pos = binary_heap_get_left(curr_pos);
            int right_pos = binary_heap_get_right(curr_pos);
            
            if(left_pos < heap->size 
            && heap->comparator(heap->tree[left_pos], heap->tree[curr_pos]) > 0) {
                m_pos = left_pos;
            } else {
                m_pos = curr_pos;
            }
            
            if(right_pos < heap->size 
            && heap->comparator(heap->tree[right_pos], heap->tree[m_pos]) > 0) {
                m_pos = right_pos;
            }            
            
            if(m_pos == curr_pos) {
                break;
            } else {
                void* temp = heap->tree[m_pos];
                heap->tree[m_pos] = heap->tree[curr_pos];
                heap->tree[curr_pos] = temp;
                
                curr_pos = m_pos;
            }
        }
    }
    
    return BINARYHEAP_SUCCESS;
}
