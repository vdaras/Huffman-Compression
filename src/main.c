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

#include <stdio.h>
#include <string.h>

#include "huffman_encoding.h"

void print_usage() {
    printf("Usage: huffman_encoding -c[-d] infile outfile.\n");
}

int main(int argc, char **argv) {

    int compress = 0;

    if(argc < 4) {
        printf("Insufficient arguments.\n");
        print_usage();
        return -1;
    }
     
    if(strcmp(argv[1], "-c") == 0) {
        compress = 1;
    } else if(strcmp(argv[1], "-d") == 0) {
        compress = 0;
    } else {
        printf("Unrecognized option %s.\n", argv[1]);
        print_usage();
        return -1;
    }

    FILE* in = fopen(argv[2], "rb");
    if(in == NULL) {
        printf("File %s doesn't exist.\n", argv[2]);
        return -2;
    }

    FILE* out = fopen(argv[3], "wb");
    if(out == NULL) {
        printf("Can't open %s for writing.\n", argv[3]);
        fclose(in);
        return -2;
    }

    int status;
    if(compress == 1) {

        status = huffman_encode(in, out);
        if(status == 0) {
            printf("Compression successful.\n");
        } else {
            printf("Compression failed.\n");
        }

    } else {

        status = huffman_decode(in, out);
        if(status == 0) {
            printf("Decompression successful.\n");
        } else {
            printf("Deompression failed.\n");
        }

    }

    fclose(in);
    fclose(out);
    return 0;
}