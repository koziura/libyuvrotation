#include "yuvrotate.h"
#include <string>
#include <vector>

#include <libyuv/cpu_id.h>
#include <libyuv/rotate.h>

typedef std::vector<uint8_t> bytearray_t;

bool
yuv420i420Rotate(std::string src_filename,
				 std::string dst_filename,
				 int src_width, int src_height,
				 int angle,
				 int opticpu_flags)
{
	if ( src_width < 2 && src_height < 2 ) {
		return false;
	}

	int dst_width;
	int dst_height;

	libyuv::RotationMode mode = (libyuv::RotationMode)angle;

	switch (mode) {
	case libyuv::kRotate90:
	case libyuv::kRotate270:
		dst_width = src_height;
		dst_height = src_width;
	break;
	default:
		dst_width = src_width;
		dst_height = src_height;
	break;

	}

	FILE* src_file = std::fopen(src_filename.c_str(), "rb");

	if ( !src_file ) {
		fprintf(stderr, "error: not found: %s\n", src_filename.c_str());
		return false;
	}

	std::fseek(src_file, 0L, SEEK_END);
	auto src_fsz = std::ftell(src_file);
	std::fseek(src_file, 0L, SEEK_SET);

	int src_i420_y_size = src_width * abs(src_height);
	int src_i420_uv_size = ((src_width + 1) / 2) * ((abs(src_height) + 1) / 2);

	int dst_i420_y_size = dst_width * dst_height;
	int dst_i420_uv_size = ((dst_width + 1) / 2) * ((dst_height + 1) / 2);
	auto dst_i420_size = dst_i420_y_size + dst_i420_uv_size * 2u;

	if ( src_fsz != dst_i420_size ) {
		std::fclose(src_file);
		fprintf(stderr,"error: image scale does not match the size of the input file: %s\n", src_filename.c_str());
		return false;
	}

	bytearray_t src_i420(src_fsz);
	auto rd_sz = (unsigned)std::fread(src_i420.data(), 1, src_fsz, src_file);
	std::fclose(src_file);
	if ( rd_sz != src_fsz ) {
		fprintf(stderr, "error: not equal memory size from file: %s\n", src_filename.c_str());
		return false;
	}

	bytearray_t dst_i420_c(dst_i420_size);

	int res;
	res = libyuv::MaskCpuFlags(opticpu_flags);  // Enable/Disable all CPU optimization.
	res = libyuv::I420Rotate(src_i420.data(), src_width, src_i420.data() + src_i420_y_size,
							 (src_width + 1) / 2, src_i420.data() + src_i420_y_size + src_i420_uv_size,
							 (src_width + 1) / 2, dst_i420_c.data(), dst_width,
							 dst_i420_c.data() + dst_i420_y_size, (dst_width + 1) / 2,
							 dst_i420_c.data() + dst_i420_y_size + dst_i420_uv_size,
							 (dst_width + 1) / 2, src_width, src_height, mode);

	src_i420.clear();

	if ( res ) {
		fprintf(stderr, "error: libyuv::I420Rotate code: %d\n", res);
		return false;
	}

	FILE* dst_file = std::fopen(dst_filename.c_str(), "wb");

	if ( !dst_file ) {
		fprintf(stderr, "error: file not creating: %s\n", dst_filename.c_str());
		return false;
	}
	auto wr_sz = std::fwrite(dst_i420_c.data(), 1, dst_i420_c.size(), dst_file);

	if ( wr_sz != dst_i420_c.size() ) {
		fprintf(stderr, "error: the memory size is not equal to the file write size: %s\n", dst_filename.c_str());
	}
	std::fclose(dst_file);
	return true;
}
