#!/usr/bin/python3
import random

### Run-length encoding test
# Pure black images
open("test/raw/rgba/black.rgba", "wb").write(b"\0\0\0\xFF" * 240000)
open("test/raw/rgb/black.rgb", "wb").write(b"\0\0\0" * 240000)
# Pure red images
open("test/raw/rgba/red.rgba", "wb").write(b"\xFF\0\0\xFF" * 240000)
open("test/raw/rgb/red.rgb", "wb").write(b"\xFF\0\0" * 240000)

### Seen pixel encoding test
# Exactly 64 repeating colors
data_rgba = []
data_rgb = []
for y in range(600):
    for x in range(400):
        # Create 64 unique colors using a simple hash
        color_idx = ((x // 8) + (y // 8)) % 64
        r = (color_idx * 4) % 256
        g = (color_idx * 7) % 256
        b = (color_idx * 11) % 256
        data_rgba.extend([r, g, b, 255])
        data_rgb.extend([r, g, b])

open("test/raw/rgba/seen64.rgba", "wb").write(bytes(data_rgba))
open("test/raw/rgb/seen64.rgb", "wb").write(bytes(data_rgb))

# 16 repeating colors
data_rgba = []
data_rgb = []
colors = [(255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,0,255), (0,255,255),
          (128,0,0), (0,128,0), (0,0,128), (128,128,0), (128,0,128), (0,128,128),
          (255,128,0), (255,0,128), (128,255,0), (0,255,128)]

for y in range(600):
    for x in range(400):
        color_idx = ((x // 25) + (y // 37)) % 16
        r, g, b = colors[color_idx]
        data_rgba.extend([r, g, b, 255])
        data_rgb.extend([r, g, b])

open("test/raw/rgba/seen16.rgba", "wb").write(bytes(data_rgba))
open("test/raw/rgb/seen16.rgb", "wb").write(bytes(data_rgb))

### Diff encoding test
# Random
data_rgba = []
data_rgb = []

# Base color: medium gray
base_r, base_g, base_b = 128, 128, 128

for i in range(240000):
    # Add small random variations within diff range (±2)
    r = max(0, min(255, base_r + random.randint(-2, 2)))
    g = max(0, min(255, base_g + random.randint(-2, 2)))
    b = max(0, min(255, base_b + random.randint(-2, 2)))

    data_rgba.extend([r, g, b, 255])
    data_rgb.extend([r, g, b])

open("test/raw/rgba/diff.rgba", "wb").write(bytes(data_rgba))
open("test/raw/rgb/diff.rgb", "wb").write(bytes(data_rgb))

# Gradient
data_rgba = []
data_rgb = []

for y in range(600):
    for x in range(400):
        # Slow gradient with small steps
        r = (x * 255 // 400)
        g = (y * 255 // 600)
        b = 128

        data_rgba.extend([r, g, b, 255])
        data_rgb.extend([r, g, b])

open("test/raw/rgba/gradient_diff.rgba", "wb").write(bytes(data_rgba))
open("test/raw/rgb/gradient_diff.rgb", "wb").write(bytes(data_rgb))

### Luma encoding test
data_rgba = []
data_rgb = []

base_r, base_g, base_b = 200, 128, 200

for i in range(240000):
    # Green varies much more than red/blue
    r = max(0, min(255, base_r + random.randint(-8, 8)))
    g = max(0, min(255, base_g + random.randint(-32, 32)))
    b = max(0, min(255, base_b + random.randint(-8, 8)))
    
    data_rgba.extend([r, g, b, 255])
    data_rgb.extend([r, g, b])

open("test/raw/rgba/luma.rgba", "wb").write(bytes(data_rgba))
open("test/raw/rgb/luma.rgb", "wb").write(bytes(data_rgb))
