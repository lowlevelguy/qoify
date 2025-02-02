# QOIfy

Quite OK Image (QOI) format CLI encoder/decoder to and from RAW images.

## Usage
Note: the program isn't yet actually usable, i.e. it will execute, but it won't be doing much of anything.

```
qoify - Convert between RAW and QOI image formats

Usage: qoify [mode] input output

Modes:
  -d    Decode QOI file to RAW format
  -e    Encode RAW file to QOI format

Examples:
  qoify -e image.raw image.qoi    # Encode RAW to QOI
  qoify -d image.qoi image.raw    # Decode QOI to RAW

Note: RAW files must be uncompressed RGB/RGBA data
```
