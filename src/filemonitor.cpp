#include "filemonitor.h"
#include "libyuvcore.h"

FileMonitor::FileMonitor()
{
}

void
FileMonitor::handleFileAction(FW::WatchID,
							  const FW::String& dir,
							  const FW::String& filename,
							  FW::Action action)
{
	if ( action == FW::Actions::Modified ) {
		libyuv_t::instance().addFileAction(dir, filename, action);
	}
}

