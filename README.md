# QOIfy

Quite OK Image (QOI) format CLI encoder/decoder to and from RAW images.

## Building
```
$ make
```
P.S.: If you're using a compiler other than `gcc`, make sure to modify the `Makefile` accordingly. The program also uses `getopt`, so it will not work on Windows.

## Usage
Note: the program isn't yet actually usable, i.e. it will execute, but it won't be doing much of anything.

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
