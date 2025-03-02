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
For now, since the decoder logic is not yet implemented, I test by viewing the input RAW and output QOI images in online viewers.
Another possible way would be to use the encoder from the [official repo](https://github.com/phoboslab/qoi) and compare the hashes of its and my outputs.

Once the decoder logic is implemented though, it will be possible to simply encode then decode an image, and check that its unchanged.

P.S. All tests currently included in the `test` directory are successful. 
