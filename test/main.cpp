#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <memory>
#include <atomic>
#include <signal.h>

#include <unistd.h>
#include <uuid/uuid.h>

#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

#include <libyuvrotation.h>

namespace fs = boost::filesystem;

namespace test_helper {
bool unittest_mode = false;
std::string src_path;
libyuvrotate::yuvimage_t image_test;
auto proc_threads = 0u;
std::atomic_bool terminate;
typedef std::unique_ptr<std::thread> thread_uniq_t;

#ifndef SIMD_ALIGNED
#if defined(_MSC_VER) && !defined(__CLR_VER)
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#elif defined(__GNUC__) && !defined(__pnacl__)
#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
#else
#define SIMD_ALIGNED(var) var
#endif
#endif

static __inline int Abs(int v) {
  return v >= 0 ? v : -v;
}

#define align_buffer_page_end(var, size)                                \
  uint8_t* var##_mem =                                                  \
	  reinterpret_cast<uint8_t*>(malloc(((size) + 4095 + 63) & ~4095)); \
  uint8_t* var = reinterpret_cast<uint8_t*>(                            \
	  (intptr_t)(var##_mem + (((size) + 4095 + 63) & ~4095) - (size)) & ~63)

#define free_aligned_buffer_page_end(var) \
  free(var##_mem);                        \
  var = nullptr

unsigned int fastrand_seed = 0xfb;
inline int fastrand() {
  fastrand_seed = fastrand_seed * 214013u + 2531011u;
  return static_cast<int>((fastrand_seed >> 16) & 0xffff);
}
} // namespace test_helper

class BenchmarkRotation:
		public ::testing::Test
{
public:
	BenchmarkRotation() {
		/* init protected members here */
	}

	~BenchmarkRotation() {
		/* free protected members here */
	}

	void SetUp() {
		/* called before every test */
	}

	void TearDown() {
		/* called after every test */
	}

protected:
	/* none yet */
};
/*!
 * \brief TEST_F
 */
TEST_F(BenchmarkRotation, SimulateImageSource)
{
	using namespace test_helper;
	auto cputhreads = std::thread::hardware_concurrency();

	if ( !proc_threads || (proc_threads && proc_threads > cputhreads) ) {
		proc_threads = cputhreads;
	}

	std::vector< thread_uniq_t > thread_list;
	for ( auto i=0u; i < proc_threads; ++i ) {
		if ( terminate ) {
			break;
		}
		thread_list.emplace_back( new std::thread( []
		{
			if ( terminate ) {
				return;
			}
			uuid_t uuid;
			char str[37];

			uuid_generate(uuid);
			uuid_unparse(uuid, str);

			int src_width = image_test.width;
			int src_height = image_test.height;

			int src_i420_y_size = src_width * Abs(src_height);
			int src_i420_uv_size = ((src_width + 1) / 2) * ((Abs(src_height) + 1) / 2);
			auto src_i420_size = src_i420_y_size + src_i420_uv_size * 2u;

			align_buffer_page_end(src_i420, src_i420_size);
			for ( auto i=0u; i < src_i420_size; ++i ) {
				src_i420[i] = fastrand() & 0xff;
			}

			std::string filename = src_path + "/" + std::string(str) + ".yuv";

			FILE* src_file = std::fopen(filename.c_str(), "wb");

			ASSERT_TRUE(src_file);
			if ( !src_file ) {
				fprintf(stderr, "error: file not creating: %s\n", filename.c_str());
				return;
			}
			auto wr_sz = std::fwrite(src_i420, 1, src_i420_size, src_file);

			ASSERT_EQ(wr_sz, src_i420_size);
			if ( wr_sz != src_i420_size ) {
				fprintf(stderr, "error: the memory size is not equal to the file write size: %s\n", filename.c_str());
			}

			std::fclose(src_file);
			free_aligned_buffer_page_end(src_i420);
			ASSERT_FALSE(src_i420);
		}) );
	}

	for (auto& t: thread_list) {
		t->join();
		t.reset();
	}
	thread_list.clear();
}

/*!
 * \brief The CmdArgsParser class
 */
class CmdArgsParser {
public:
	/*!
	 * \brief CmdArgsParser
	 * \param argc_
	 * \param argv_
	 * \param switches_on_
	 */
	CmdArgsParser(int argc_, char* argv_[], bool switches_on_ = false) {
		argc = argc_;
		argv.resize( argc );
		std::copy( argv_, argv_ + argc, argv.begin() );
		switches_on = switches_on_;

		//map the switches to the actual
		//arguments if necessary
		if ( switches_on ) {
			std::vector<std::string>::iterator it1, it2;
			it1 = argv.begin();
			it2 = it1 + 1;

			while ( true ) {
				if ( it1 == argv.end() || it2 == argv.end() ) {
					if ( it1 != argv.end() ) {
						if ( (*it1)[0] == '-' ) {
							switch_map[*it1] = "true";
						}
					}
					break;
				}
				if ( (*it1)[0] == '-' ) {
					switch_map[*it1] = *(it2);
				}
				++it1;
				++it2;
			}
		}
	}
	/*!
	 * \brief getArg
	 * \param arg
	 * \return
	 */
	inline std::string getArg(const int& arg) {
		std::string res;

		if ( arg >= 0 && arg < argc ) {
			res = argv[arg];
		}

		return res;
	}
	/*!
	 * \brief getArg
	 * \param arg
	 * \return
	 */
	inline std::string getArg(const std::string& arg) {
		std::string res;

		if ( !switches_on ) {
			return res;
		}

		if ( switch_map.find(arg) != switch_map.end() ) {
			res = switch_map[arg];
		}

		return res;
	}
	/*!
	 * \brief size
	 * \return
	 */
	inline int size() const {
		return static_cast<int>( switch_map.size() );
	}

private:
	int argc;
	std::vector<std::string> argv;
	bool switches_on;
	std::map<std::string, std::string> switch_map;
};
/*!
 * \brief breakable_handler
 * \param sig
 */
static inline void breakable_handler(int sig) {
	printf("\ntry to shutdown from signal: %d\n", sig);
	test_helper::terminate = true;
	libyuvrotate::stop();
}
/*!
 * \brief g_init
 */
void g_init()
{
	ASSERT_TRUE( libyuvrotate::initialize() );
}
/*!
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
	CmdArgsParser cmd_args(argc, argv, true);
	{
		using namespace std;
		cout << "\n* help {begin} *" << endl;
		cout << "* Welcome Image-Rotator YUV 4:2:0 *" << endl;
		cout << "\n[arguments]" << endl;
		cout << "  --i [input path] path for scan source yuv images." << endl;
		cout << "  --o [output path] path for write rotated yuv images." << endl;
		cout << "  --w [width] set width value source yuv image." << endl;
		cout << "  --h [height] set height value source yuv image." << endl;
		cout << "  --r [rotate] set rotate mode (90/180/270)." << endl;
		cout << "  --t [threads] set thread count for rotation performance. (default: all available CPU-threads)" << endl;
		cout << "\n[results]" << endl;
		cout << "  the output of results will be written thread address with performance time for once image." << endl;
		cout << "  for stop process press Ctrl+C." << endl;
		cout << "\n[tests]" << endl;
		cout << "  --b [unit-test] set benchmark iterations." << endl;
		cout << "  -h [gtest] output googletest helper (set with '--b' arg)." << endl;
		cout << "\n* help {end} *\n" << endl;
	}
	if ( cmd_args.size() < 5 ) {
		fprintf(stderr, "!error: not enough input arguments for run!\n");
		return 0;
	}

	test_helper::terminate = false;

	boost::system::error_code ec;

	if ( argc ) {
		fs::path app_path((std::string(argv[0])));

		app_path = fs::absolute(app_path);
		app_path = app_path.normalize();
		app_path = app_path.parent_path();
		fs::current_path(app_path, ec);
	}

	test_helper::unittest_mode = false;

	printf("Starting Image-Rotator...\n");

	std::string temp;
	temp = cmd_args.getArg("--b");
	if ( !temp.empty() ) {
		test_helper::unittest_mode = true;
	}

	if ( test_helper::unittest_mode ) {
		printf("Unit-test mode: enabled.\n");
		::testing::InitGoogleTest(&argc, argv);
		g_init();
	} else {
		printf("Unit-test mode: disabled.\n");
		libyuvrotate::initialize();
	}

	temp = cmd_args.getArg("--i");
	if ( temp.empty() ) {
		fprintf(stderr, "error: empty input path argument.\n");
		return 0;
	}
	if ( !test_helper::unittest_mode ) {
		if ( !fs::exists(temp, ec) ) {
			std::cout << ec.message() << '\n';
			return 0;
		}
		if ( !fs::is_directory(temp, ec) ) {
			std::cout << ec.message() << '\n';
			return 0;
		}
	} else {
		fs::path fspath(temp);
		boost::system::error_code ec;

		if ( !fs::exists(fspath, ec) ) {
			fs::create_directories(fspath, ec);
		}
		temp = fs::absolute(fspath).string();
	}
	libyuvrotate::set_srcpath(temp.c_str());
	test_helper::src_path = temp;

	temp = cmd_args.getArg("--o");
	if ( temp.empty() ) {
		fprintf(stderr, "error: empty output path argument.\n");
		return 0;
	}
	libyuvrotate::set_dstpath(temp.c_str());

	libyuvrotate::yuvimage_t image;
	temp = cmd_args.getArg("--w");
	if ( temp.empty() ) {
		fprintf(stderr, "error: empty width value argument.\n");
		return 0;
	}
	image.width = std::stoi(temp);

	temp = cmd_args.getArg("--h");
	if ( temp.empty() ) {
		fprintf(stderr, "error: empty height value argument.\n");
		return 0;
	}
	image.height = std::stoi(temp);

	temp = cmd_args.getArg("--t");
	if ( !temp.empty() ) {
		test_helper::proc_threads = std::stoi(temp);
		libyuvrotate::set_threadhum(test_helper::proc_threads);
	}

	temp = cmd_args.getArg("--r");
	if ( temp.empty() ) {
		fprintf(stderr, "error: empty rotate mode.\n");
		return 0;
	}
	image.rotate_mode = std::stoi(temp);

	signal(SIGABRT, &breakable_handler);
	signal(SIGTERM, &breakable_handler);
	signal(SIGINT, &breakable_handler);

#ifdef __linux__
	signal(SIGQUIT, &breakable_handler);
#endif

	libyuvrotate::set_srcimage(image);
	test_helper::image_test = image;

	int res = 0;

	test_helper::thread_uniq_t test_thread;
	if ( test_helper::unittest_mode ) {
		test_thread.reset( new std::thread(&RUN_ALL_TESTS) );
	}

	res = libyuvrotate::exec();

	if ( test_thread ) {
		test_thread->join();
		test_thread.reset();
	}

	libyuvrotate::shutdown();

	printf("\n--- this is correct way to shutdown ---\n");

	return res;
}
