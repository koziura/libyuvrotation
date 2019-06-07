#include "libyuvcore.h"
#include "filemonitor.h"
#include "yuvrotate.h"
#include <thread>
#include <iostream>
#include <stdio.h>

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

namespace fs = boost::filesystem;

auto ends_with = [](const std::string& main, const std::string& match)->bool {
	if ( main.size() < match.size() ) {
		return false;
	}
	return !main.compare( main.size() - match.size(), match.size(), match );
};

libyuv_t::libyuv_t():
	m_quit(false), m_corecount(0)
{
	m_image = {0,0,0};
}

libyuv_t::~libyuv_t()
{
	quit();
}

void
libyuv_t::quit()
{
	m_quit = true;
}

int
libyuv_t::exec()
{
	if ( m_image.width < 2 && m_image.height < 2 ) {
		fprintf(stderr, "error: incorrect image resolution.\n");
		return -1;
	}

	switch ( m_image.rotate_mode ) {
	case 90: case 180: case 270: case 0:
	break;
	default:
		fprintf(stderr, "error: incorrect rotate mode resolution.\n");
		return -1;
	}

	m_quit = true;

	if ( m_filewatcher ) {
		m_filewatcher->join();
		m_filewatcher.reset();
	}

	for ( auto& t : m_thread_list ) {
		t->join();
	}

	m_thread_list.clear();

	m_quit = false;

	setupFileQueue();

	m_filewatcher.reset( new std::thread(&libyuv_t::fileWatcherThread, this) );

	auto cputhreads = std::thread::hardware_concurrency();

	if ( !m_corecount ) {
		m_corecount = cputhreads;
	}

	for ( int i=0; i < m_corecount; ++i ) {
		m_thread_list.emplace_back( new std::thread(&libyuv_t::imageRotateThread, this) );
	}

	std::chrono::milliseconds wait100ms(100);
	while ( !m_quit ) {
		std::this_thread::sleep_for(wait100ms);
	}

	clearQueueFile();

	m_filewatcher->join();
	m_filewatcher.reset();

	for ( auto& t : m_thread_list ) {
		t->join();
	}
	m_thread_list.clear();
	return 0;
}

void
libyuv_t::setupFileQueue()
{
	fs::path path(m_srcpath);
	boost::system::error_code ec;

	if ( !fs::exists(path, ec) ) {
		return;
	}

	fs::directory_iterator end;

	m_queue_lock.lock();
	for ( fs::directory_iterator it(path); it != end; ++it ) {
		if ( fs::is_regular_file(it->path()) && ends_with(it->path().string(), "yuv") ) {
			auto fp = fs::absolute(it->path()).string();
			m_image_queue.push_back(fp);
		}
	}
	m_queue_lock.unlock();
}

void
libyuv_t::setThreadCount(unsigned int num)
{
	if (num > 24) {
		num = 0;
	}
	m_corecount = num;
}

void
libyuv_t::setSourcePath(std::string path)
{
	m_srcpath = path;

	if ( m_srcpath.empty() ) {
		m_srcpath = "./";
	} else {
		m_srcpath += "/";
	}
}

void
libyuv_t::setDestinationPath(std::string path)
{
	m_dstpath = path;

	fs::path fspath(m_dstpath);
	boost::system::error_code ec;

	if ( !fs::exists(path, ec) ) {
		printf( "setDestinationPath [what]: %s\n", ec.message().c_str() );

		if ( !fs::create_directories(fspath, ec) ) {
			printf( "setDestinationPath [what]: %s\n", ec.message().c_str() );
		}
	}
	m_dstpath = fs::absolute(fspath).string();
}

void
libyuv_t::addFileAction(std::string dir, std::string filename, int action)
{
	printf("AddAction DIR: \"%s\";\n FILE: \"%s\";\nEVENT: %d\n",
		   dir.c_str(), filename.c_str(), action);

	fs::path path(dir + "/" + filename);

	if ( fs::is_regular_file(path) && ends_with(path.string(), "yuv") ) {
		addQueueFile( fs::absolute(path).string() );
	}
}

void
libyuv_t::addQueueFile(std::string filename)
{
	if ( m_quit ) {
		return;
	}
	m_queue_lock.lock();
	m_image_queue.push_back(filename);
	m_queue_lock.unlock();
}

void
libyuv_t::clearQueueFile()
{
	m_queue_lock.lock();
	m_image_queue.clear();
	m_queue_lock.unlock();
}

std::string
libyuv_t::getQueueFile()
{
	m_queue_lock.lock();

	if ( !m_image_queue.size() ) {
			m_queue_lock.unlock();
		return std::string();
	}
	auto filename = m_image_queue.front();
	m_image_queue.pop_front();
	m_queue_lock.unlock();
	return filename;
}

bool
libyuv_t::alreadyProcessed(std::string filename)
{
	m_processed_lock.lock();
	auto res = m_image_processed.find(filename) != m_image_processed.end();
	m_processed_lock.unlock();
	return res;
}

void
libyuv_t::appendProcessed(std::string filename)
{
	m_processed_lock.lock();
	m_image_processed.insert(filename);
	m_processed_lock.unlock();
}

void
libyuv_t::setYuvImage(libyuvrotate::yuvimage_t image)
{
	m_image = image;
}

void
libyuv_t::fileWatcherThread()
{
	try {
		// create the listener (before the file watcher - so it gets destroyed after the file watcher)
		FileMonitor listener;

		// create the file watcher object
		FW::FileWatcher fwatcher;

		// add a watch to the system
		// the file watcher doesn't manage the pointer to the listener - so make sure you don't just
		// allocate a listener here and expect the file watcher to manage it - there will be a leak!
		FW::WatchID watch_id = fwatcher.addWatch(m_srcpath, &listener, true);
		(void)&watch_id;

		// loop until a key is pressed
		while ( !m_quit ) {
			fwatcher.update();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	} catch( std::exception& e ) {
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
	}
}

void
libyuv_t::imageRotateThread()
{
	boost::timer::cpu_timer cpu_timer;
	std::thread::id this_id = std::this_thread::get_id();
	std::string src_filename;
	std::string dst_filename = m_dstpath + "/rotate" + std::to_string(m_image.rotate_mode) + "_";
	bool res;

	while ( !m_quit ) {
		src_filename = getQueueFile();

		if ( !src_filename.empty() && !alreadyProcessed(src_filename) ) {
			auto dstgen_end = dst_filename + fs::path(src_filename).filename().string();

			cpu_timer.start();
			res = yuv420i420Rotate(src_filename, dstgen_end,
								   m_image.width, m_image.height,
								   m_image.rotate_mode, -1);
			cpu_timer.stop();

			if ( res ) {
				appendProcessed( src_filename );
				printf("thread id [0x%lX]: rotate is elapsed from: %.1fms, file %s\n",
					   *(uint64_t*)&this_id, cpu_timer.elapsed().wall / 1000000., src_filename.c_str() );
			}
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
