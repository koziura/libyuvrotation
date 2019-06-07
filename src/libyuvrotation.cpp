#include "libyuvrotation.h"
#include "libyuvcore.h"

namespace libyuvrotate {
static libyuv_t* libyuv_core = nullptr;

bool
initialize(void)
{
	if ( libyuv_core ) {
		return false;
	}
	libyuv_core = libyuv_t::instance().getInstance();
	return libyuv_core != nullptr;
}

void
shutdown(void)
{
	if ( libyuv_core ) {
		libyuv_core->destroy();
		libyuv_core = nullptr;
	}
}

void
stop(void)
{
	if ( libyuv_core ) {
		libyuv_core->quit();
	}
}

int
exec(void)
{
	if ( libyuv_core ) {
		return libyuv_core->exec();
	}
	return -1;
}

void
set_srcimage(yuvimage_t image)
{
	if ( !libyuv_core ) {
		return;
	}
	libyuv_core->setYuvImage(image);
}

void
set_srcpath(const char* path)
{
	if ( !libyuv_core ) {
		return;
	}
	libyuv_core->setSourcePath(path);
}

void
set_dstpath(const char* path)
{
	if ( !libyuv_core ) {
		return;
	}
	libyuv_core->setDestinationPath(path);
}

void
set_threadhum(int num)
{
	if ( !libyuv_core ) {
		return;
	}
	libyuv_core->setThreadCount(num);
}
} // namespace libyuvrotate


