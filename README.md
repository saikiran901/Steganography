# Steganography
A Image Steganography project developed using C programming. It allows users to hide secret text messages inside BMP images using the Least Significant Bit (LSB) technique and retrieve the hidden data through decoding without significantly changing the original image.


# Image Steganography in C

## Overview
This project implements Image Steganography using the Least Significant Bit (LSB) technique in C programming.

It hides secret text file data inside a BMP image and retrieves the hidden information through decoding.

## Features
- Encode secret text into BMP images.
- Decode and extract hidden text.
- LSB-based data hiding technique.
- Supports BMP image files.
- Command-line interface.

## Technologies Used
- C Programming
- File Handling
- Bitwise Operations
- LSB Steganography
- BMP Image Processing

## Usage

### Encoding
```bash
./a.out -e beautiful.bmp secret.txt output.bmp
```

### Decoding
```bash
./a.out -d output.bmp
```
