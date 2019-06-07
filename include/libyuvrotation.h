#ifndef _LIBYUVROTATION_H_
#define _LIBYUVROTATION_H_

#ifdef _WIN32
#	ifdef YUVROTATE_LIBRARY_BUILD
#		define _libyuvrotate_exports __declspec(dllexport)
#	else
#		define _libyuvrotate_exports __declspec(dllimport)
#	endif
#else
#	ifdef YUVROTATE_LIBRARY_BUILD
#		define _libyuvrotate_exports __attribute__((visibility("default")))
#	else
#		define _libyuvrotate_exports
#	endif
#endif

namespace libyuvrotate {
/*!
 * \brief The yuvimage_t struct
 */
struct yuvimage_t {
	int width, height;
	int rotate_mode;
};
/*!
 * \brief initialize
 * \return
 */
bool _libyuvrotate_exports initialize(void);
/*!
 * \brief stop
 */
void _libyuvrotate_exports stop(void);
/*!
 * \brief shutdown
 */
void _libyuvrotate_exports shutdown(void);
/*!
 * \brief set_srcimage
 * \param image
 */
void _libyuvrotate_exports set_srcimage(struct yuvimage_t image);
/*!
 * \brief set_srcpath
 * \param path
 */
void _libyuvrotate_exports set_srcpath(const char* path);
/*!
 * \brief set_dstpath
 * \param path
 */
void _libyuvrotate_exports set_dstpath(const char* path);
/*!
 * \brief set_threadhum
 * \param num
 */
void _libyuvrotate_exports set_threadhum(int num);
/*!
 * \brief exec
 */
int _libyuvrotate_exports exec(void);

} // namespace libyuvrotate

#endif // _LIBYUVROTATION_H_
