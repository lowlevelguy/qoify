#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "qoi_decode.h"
#include "qoi_encode.h"

void print_usage() {
    printf(
        "qoify - Convert between RAW and QOI image formats\n\n"
        "Usage:\n"
        "  Encode: qoify -e --width <w> --height <h> [--no-alpha] input output\n"
        "  Decode: qoify -d input output\n\n"
        "Options:\n"
        "  -d, --decode       Decode QOI file to RAW format\n"
        "  -e, --encode       Encode RAW file to QOI format\n"
        "  -w, --width <n>    Width of input image (required for encoding)\n"
        "  -h, --height <n>   Height of input image (required for encoding)\n"
        "      --no-alpha     Input image is RGB (no alpha channel)\n"
        "      --help         Display this help message\n\n"
        "Examples:\n"
        "  qoify -e -w 800 -h 600 input.raw output.qoi     # Encode RGBA RAW to QOI\n"
        "  qoify -e -w 800 -h 600 --no-alpha in.raw out.qoi    # Encode RGB RAW to QOI\n"
        "  qoify -d image.qoi image.raw                     # Decode QOI to RAW\n\n"
        "Note: RAW files must be uncompressed RGB/RGBA data\n"
    );
}


int main (int argc, char** argv) {
	static struct option long_options[] = {
        {"decode", no_argument, 0, 'd'},
        {"encode", no_argument, 0, 'e'},
        {"width", required_argument, 0, 'w'},
        {"height", required_argument, 0, 'H'},  // Using 'H' since 'h' is taken by --help
        {"no-alpha", no_argument, 0, 'a'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int mode = 0;  // 0 = unset, 1 = encode, 2 = decode
    int width = -1;
    int height = -1;
    int has_alpha = 1;  // Default to RGBA
    int opt;

    while ((opt = getopt_long(argc, argv, "dew:H:ah", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
                if (mode != 0) {
                    fprintf(stderr, "Error: Cannot specify both encode and decode\n");
                    return -1;
                }
                mode = 2;
                break;
            case 'e':
                if (mode != 0) {
                    fprintf(stderr, "Error: Cannot specify both encode and decode\n");
                    return -1;
                }
                mode = 1;
                break;
            case 'w':
                width = atoi(optarg);
                if (width <= 0) {
                    fprintf(stderr, "Error: Invalid width value\n");
                    return -1;
                }
                break;
            case 'H':
                height = atoi(optarg);
                if (height <= 0) {
                    fprintf(stderr, "Error: Invalid height value\n");
                    return -1;
                }
                break;
            case 'a':
                has_alpha = 0;
                break;
            case 'h':
                print_usage();
                return 0;
            default:
                print_usage();
                return -1;
        }
    }

    // Need two more arguments (input and output files) after options
    if (argc - optind != 2) {
        fprintf(stderr, "Error: Missing input/output file arguments\n");
        print_usage();
        return -1;
    }

    // Get input and output filenames
    char *input_file = argv[optind];
    char *output_file = argv[optind + 1];

    // Check if mode was specified
    if (mode == 0) {
        fprintf(stderr, "Error: Must specify either --encode or --decode\n");
        return -1;
    }

    // Validate encoding requirements
    if (mode == 1) {  // encode mode
        if (width == -1) {
            fprintf(stderr, "Error: Width must be specified for encoding\n");
            return -1;
        }
        if (height == -1) {
            fprintf(stderr, "Error: Height must be specified for encoding\n");
            return -1;
        }
        qoi_encode(input_file, output_file, width, height, has_alpha);
    } else {
        // In decode mode, ignore width/height/alpha settings
        if (width != -1 || height != -1 || !has_alpha) {
            fprintf(stderr, "Warning: Width, height, and alpha settings are ignored in decode mode\n");
        }
        qoi_decode(input_file, output_file);
    }
	
	return 0;
}
