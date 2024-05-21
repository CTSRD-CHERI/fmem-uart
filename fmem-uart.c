// Based on https://github.com/CTSRD-CHERI/fmem/blob/main/fmem.c, which has this license
/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Ruslan Bukin <br@bsdpad.com>
 * Copyright (c) 2024 Samuel W. Stark <sws35@cam.ac.uk>
 *
 * This work was supported by Innovate UK project 105694, "Digital Security
 * by Design (DSbD) Technology Platform Prototype".
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

// Basics
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

// Safety/error detection
#include <errno.h>
#include <string.h>

// For opening/closing fmem file
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// For accessing fmem file
#include "fmem.h"

// Print the help message for this program and exit
void print_help(const char* argv0) {
    fprintf(stderr, 
            "\nUsage:\t%s { device } { uart-offset }\n"
            "\tdevice : fmem device to act against\n"
            "\tuart-offset: the offset of the UART Receiver Buffer Register/Transmitter Holding Register\n"
            "\t             (parsed with strtoul(base=16), must be in hex)\n"
            "\t or:\t%s --help to read this help message\n",
            argv0, argv0);
    exit(EXIT_FAILURE);
}

// Print an error that occurred when parsing/using the arguments to this program, then print the help message and exit
void print_arg_error(const char* context, const char* argv0) {
    if (errno) {
        fprintf(stderr, "Error encountered while %s\nerrno %d (%s)\n\n", context, errno, strerror(errno));
    } else {
        fprintf(stderr, "Error encountered while %s\n\n", context);
    }
    print_help(argv0);
}

int main(int argc, const char **argv) {
    // Make sure we have 3 arguments
    assert(argc >= 1 && "argv must contain path to this program as first arg");

    if (argc < 3) {
        print_help(argv[0]);
    }

    if (strcmp("-h", argv[1]) || strcmp("--help", argv[1])) {
        print_help(argv[0]);
    }

    // Parse uart-offset
    errno = 0;
    char* last_char_of_uart_offset = NULL;
    uint32_t uart_offset = strtoul(argv[2], &last_char_of_uart_offset, 16);
    if (errno) {
        print_arg_error("parsing uart-offset", argv[0]);
    }
    // strtoul may also parse nothing if it can't find valid content, at which point errno is not set but 
    // last_char will be set to the start of the string.
    if (last_char_of_uart_offset == argv[2]) {
        print_arg_error("parsing uart-offset", argv[0]);
    }

    // Open the fmem file
    errno = 0;
    int fmem_dev = open(argv[1], O_RDWR);
    if (fmem_dev < 0) {
        print_arg_error("opening fmem device", argv[0]);
    }

    // Read from the UART until the file disappears
    int error = 0;
    do {
        uint8_t ascii = 0;
        error = fmem_read8(fmem_dev, uart_offset, &ascii);
        fprintf(stdout, "%c", ascii);
    } while (error == 0);

    // Don't EXIT_FAILURE if fmem returns an error - that can just mean the file was closed while we're passively using it
    fprintf(stderr, "fmem-uart stopped because of fmem error %d\n", error);

    // Close the fmem file
    close(fmem_dev);

    // Done
    return EXIT_SUCCESS;
}