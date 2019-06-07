#ifndef _CPP11_THREAD_SAFE_SINGLETON_H_
#define _CPP11_THREAD_SAFE_SINGLETON_H_
#if __cplusplus > 199711L
#include <atomic>
#include <mutex>
#include <memory>
/*!
 * \brief The Singleton class
 */
template<class T>
class Singleton
{
public:
	/*!
	 * \brief getInstance
	 * \return
	 */
	static T* getInstance() {
		T* sin = m_instance.load( std::memory_order_acquire );

		if ( !sin ) {
			std::lock_guard<std::mutex> myLock( m_mutex );
			sin = m_instance.load( std::memory_order_relaxed );

			if ( !sin ) {
				sin = new T;
				m_instance.store( sin, std::memory_order_release );
			}
		}
		return sin;
	}
	/*!
	 * \brief instance
	 * \return
	 */
	static T& instance() {
		return *getInstance();
	}

	static void destroy() {
		T* sin = m_instance.load( std::memory_order_acquire );

		if ( sin ) {
			std::lock_guard<std::mutex> myLock( m_mutex );
			delete sin;
			m_instance.store( nullptr, std::memory_order_release );
			m_instance.exchange(nullptr);
		}
	}

	Singleton() = default;
	~Singleton() = default;

private:
	/*!
	 * \brief Singleton
	 */
	Singleton(const Singleton&) = delete;
	/*!
	 * \brief operator =
	 * \return
	 */
	Singleton& operator =(const Singleton&) = delete;

private:
	static std::atomic<T*>	m_instance;
	static std::mutex		m_mutex;
};
/*!
 * \brief Singleton<T>::m_instance
 */
template<class T>
std::atomic<T*> Singleton<T>::m_instance;
/*!
 * \brief Singleton<T>::m_mutex
 */
template<class T>
std::mutex Singleton<T>::m_mutex;
#else
template<class T>
class Singleton
{
public:
	/*!
	 * \brief Singleton
	 */
	inline Singleton() {
		if ( singleton == nullptr ) {
			T* sin = static_cast<T*>(this);
			if ( sin && sin->singleton != nullptr ) {
				singleton = static_cast<T*>(this);
			}
		}
	}
	/*!
	 * \brief ~Singleton
	 */
	inline virtual ~Singleton() {
	}
	/*!
	 * \brief instance
	 * \return
	static inline T& instance() {
		if ( singleton == nullptr ) {
			T* sin = new T;
			singleton = static_cast<T*>(sin);
	 */
		return *singleton;
	}
	/*!
	 * \brief instancePtr
	 * \return
	static inline T* getInstance() {
		return singleton;
	}
	/*!
	 * \brief destroy
	 */
	static inline void destroy() {
		if ( singleton == nullptr ) {
			return;
		}
		T* sin = static_cast<T*>(singleton);
		if ( sin && sin->singleton != nullptr ) {
			delete sin->singleton;
			sin->singleton = nullptr;
		}
		singleton = nullptr;

protected:
	static T* singleton;
};
template<class T>
/*!
 * \brief Singleton<T>::singleton
 */
T* Singleton<T>::singleton = nullptr;
#endif

#endif // _CPP11_THREAD_SAFE_SINGLETON_H_
