#ifndef _LIBYUV_CORE_H_
#define _LIBYUV_CORE_H_
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <deque>
#include <unordered_set>
#include "libyuvrotation.h"
#include "singleton.h"
/*!
 * \brief The spinlock_t class
 */
class spinlock_t {
	std::atomic_flag m_lock_flag = ATOMIC_FLAG_INIT;
public:
	/*!
	 * \brief spinlock_t
	 */
	spinlock_t()
	{
	}
	/*!
	 * \brief lock
	 */
	inline void lock() {
		while( m_lock_flag.test_and_set(std::memory_order_acquire) );
	}
	/*!
	 * \brief try_lock
	 * \return
	 */
	inline bool try_lock() {
		return !m_lock_flag.test_and_set( std::memory_order_acquire );
	}
	/*!
	 * \brief unlock
	 */
	inline void unlock() {
		m_lock_flag.clear( std::memory_order_release );
	}
};
/*!
 * \brief The libyuv_t class
 */
class libyuv_t:
		public Singleton<libyuv_t>
{
public:
	/*!
	 * \brief libyuv_t
	 */
	libyuv_t();
	~libyuv_t();
	/*!
	 * \brief quit
	 */
	void quit();
	/*!
	 * \brief exec Execute main proccess
	 * \return code exit
	 */
	int exec();

	void addFileAction(std::string dir, std::string filename, int action);
	/*!
	 * \brief setSourcePath
	 * \param path
	 */
	void setSourcePath(std::string path);
	/*!
	 * \brief setDestinationPath
	 * \param path
	 */
	void setDestinationPath(std::string path);
	/*!
	 * \brief setThreadCount
	 * \param num
	 */
	void setThreadCount(uint num);
	/*!
	 * \brief setYuvImage
	 * \param image
	 */
	void setYuvImage(libyuvrotate::yuvimage_t image);

private:
	/*!
	 * \brief fileWatcherThread
	 */
	void fileWatcherThread();
	/*!
	 * \brief imageRotateThread
	 */
	void imageRotateThread();
	/*!
	 * \brief addQueueFile
	 * \param filename
	 */
	void addQueueFile(std::string filename);
	/*!
	 * \brief clearQueueFile
	 */
	void clearQueueFile();
	/*!
	 * \brief getQueueFile
	 * \return
	 */
	std::string getQueueFile();
	/*!
	 * \brief alreadyProcessed
	 * \param filename
	 * \return
	 */
	bool alreadyProcessed(std::string filename);
	/*!
	 * \brief appendProcessed
	 * \param filename
	 */
	void appendProcessed(std::string filename);
	/*!
	 * \brief setupFileQueue
	 */
	void setupFileQueue();

private:
	std::atomic_bool m_quit;
	std::atomic_int m_corecount;
	std::string m_srcpath;
	std::string m_dstpath;

	typedef std::unique_ptr<std::thread> thread_uniq_t;
	thread_uniq_t m_filewatcher;
	std::vector< thread_uniq_t > m_thread_list;

	spinlock_t m_queue_lock, m_processed_lock;
	std::deque<std::string> m_image_queue;
	std::unordered_set<std::string> m_image_processed;

	libyuvrotate::yuvimage_t m_image;
};

#endif // _LIBYUV_CORE_H_
