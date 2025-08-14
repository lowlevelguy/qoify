# QOIfy

Quite OK Image (QOI) format CLI encoder/decoder from and to raw RGB/RGBA images with 8-bit color depths.

## Building
To build the project, simply run:
```
make
```
The binary will then be generated in the project root folder with the name `qoify`.

P.S.: If you're using a compiler other than `gcc`, make sure to modify the `Makefile` accordingly. The program also makes use of [the POSIX-exclusive `getopt()` function](https://man7.org/linux/man-pages/man3/getopt.3.html), so it will not work on Windows; at least not until I replace it with a purely libc-based logic.

## Usage
```
qoify - Convert between RAW and QOI image formats
Usage:
  Encode: qoify -e --width <w> --height <h> [--no-alpha] input output
  Decode: qoify -d input output
Options:
  -d, --decode       Decode QOI file to RAW format
  -e, --encode       Encode RAW file to QOI format
  -w, --width <n>    Width of input image (required for encoding)
  -h, --height <n>   Height of input image (required for encoding)
      --no-alpha     Input image is RGB (no alpha channel)
      --help         Display this help message
Examples:
  qoify -e -w 800 -h 600 input.raw output.qoi     # Encode RGBA RAW to QOI
  qoify -e -w 800 -h 600 --no-alpha in.raw out.qoi    # Encode RGB RAW to QOI
  qoify -d image.qoi image.raw                     # Decode QOI to RAW
Note: RAW files must be uncompressed RGB/RGBA data
```

## Testing
To generate the tests, run:
```
./gen_tests.py
```

This will generate inside the directories `test/raw/rgb` and `test/raw/rgba` multiple raw image files targeted at testing each individual operation the codec is expected to perform:
- `black.rgb(a),red.rgb(a)`: run-length encoding test
- `seen16.rgb(a),seen64.rgb(a)`: seen/index encoding test
- `diff.rgb(a),gradient_diff.rgb(a)`: DIFF encoding test
- `luma.rgb(a)`: LUMA encoding test

To encode the tests, run:
```
./test_enc.sh
```

This will generate, respectively, in the directories `test/encoded` and `test/qoi` the encoded QOI images yielded by my implementation and the official one. To verify our encoder is working correctly, we simply compare the hashes of the two versions.

To then test the decoder, run:
```
./test_dec.sh
```

This will decode the images in `test/encoded` and save the output in `test/decoded`. You can then verify that the decoded pictures are identical to the original raws by comparing their hashes.
