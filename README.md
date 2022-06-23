# VOXL

An investigation into packing Voxel data into as small a data as possible.

This implementation achieves voxel packing into 4 bytes of data.

		voxel data:
			xyz = 12 bits
			color = 4 bits, 16 color lookup table
			neighbours = 6 bits, 111111 <- neighbour active/inactive
			active = 1 bit
		total size: 4 bytes (32 bit)
		used bits: 23
		remaining bits: 9