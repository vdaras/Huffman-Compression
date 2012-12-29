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
 
#ifndef HUFFMAN_ENCODING_H
#define HUFFMAN_ENCODING_H

#include <stdio.h>

#define HUFFMAN_UNMAPPED_BYTE -2

/**
 * Encodes a file using huffman code.
 * 
 * @param in file to encode
 * @param out output file
 * @return A flag indicating if encoding was successful.
 */
int huffman_encode(FILE* in, FILE* out);

/**
 * Decodes a file using huffman code.
 * 
 * @param in file to decode
 * @param out output file
 */
int huffman_decode(FILE* in, FILE* out);

#endif //HUFFMAN_ENCODING_H
