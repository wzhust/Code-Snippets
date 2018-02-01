// show singleton code
#ifndef SINGLETON_H_
#define SINGLETON_H_
#include <string>
#include <stdio.h>
#include <mutex>

class Config
{
public:
	static Config& GetInstance();
	static void DestroyInstance();

	void SetEnv( std::string env );
	const std::string& GetEnv() const;

private:
	Config();
	~Config();
	Config( const Config& ) = delete;
	Config& operator=( const Config& ) = delete; // move operator is also deleted

	// sync for multi-threads
	static Config* single_confg_;
	static unsigned int flag_;
	static std::mutex mutex_;
	static std::condition_variable cv_;

	std::string env_;
	FILE* config_file_ = nullptr;
};

#endif
