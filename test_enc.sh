#!/usr/bin/sh

raw_dir="test/raw"
enc_dir="test/encoded"
qoi_dir="test/qoi"

### Encode RGB images
for file in $raw_dir/rgb/*; do
	filename=$(basename "$file" .rgb)

	## My encoder
	./qoify -e --width 600 --height 400 --no-alpha "$file" "$enc_dir/rgb/$filename.qoi"
	
	## Official QOI encoder
	# The official encoder takes in png images, so convert accordingly first
	convert -size 600x400 -depth 8 "rgb:$file" tmp.png
	qoiconv tmp.png "$qoi_dir/rgb/$filename.qoi"
	rm tmp.png
done;

### Encode RGBA images
for file in $raw_dir/rgba/*; do
	filename=$(basename "$file" .rgba)

	## My encoder
	./qoify -e --width 600 --height 400 "$file" "$enc_dir/rgba/$filename.qoi"
	
	## Official QOI encoder
	convert -size 600x400 -depth 8 "rgba:$file" tmp.png
	qoiconv tmp.png "$qoi_dir/rgba/$filename.qoi"
	rm tmp.png
done;
