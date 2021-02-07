# huffman-coding
In instruction.txt you can find in what order the files are written.
Using compress/decompressed and output.bin for testing.
User is allowed to choose where to place compressed file.
The algorithm is working good when there are many repeating letters.
If there are many letters but they appear only once or twice the compression is not that good.
With too big files it overflows size_t when the whole file is turned into bits.
