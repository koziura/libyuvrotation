#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <FileWatcher/FileWatcher.h>

/// Processes a file action
class FileMonitor:
		public FW::FileWatchListener
{
public:
	/*!
	 * \brief FileMonitor
	 */
	FileMonitor();
	/*!
	 * \brief handleFileAction
	 * \param watchid
	 * \param dir
	 * \param filename
	 * \param action
	 */
	void handleFileAction(FW::WatchID watchid,
						  const FW::String& dir,
						  const FW::String& filename,
						  FW::Action action);
};


#endif // FILEMONITOR_H
