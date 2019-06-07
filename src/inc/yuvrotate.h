#ifndef YUVROTATE_H
#define YUVROTATE_H
#include <string>
/*!
 * \brief Rotate 90/180/270 degrees image YUV 4:2:0
 * \param source file path
 * \param destination file path
 * \param source image width
 * \param source image height
 * \param Rotate mode 90/180/270
 * \param CPU optimization flags (-1)-to enable; (1)-to disable; (0)-to reset state
 */
bool
yuv420i420Rotate
(
	std::string src_filepath,
	std::string dst_path,
	int src_width,
	int src_height,
	int angle,
	int opticpu_flags
);

#endif // YUVROTATE_H
