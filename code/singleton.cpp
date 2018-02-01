#include "singleton.h"

const unsigned int kCompleted = ~1u;
const unsigned int kInitial = 0u;
const unsigned int kInitializing = 1u;

Config* Config::single_confg_ = nullptr;
unsigned int Config::flag_ = kInitial;
std::mutex Config::mutex_;
std::condition_variable Config::cv_;

Config& Config::GetInstance() // handle no exception
{
	if( flag_ != kCompleted )
	{
		std::unique_lock<decltype(mutex_)> lock( mutex_ );
		while( flag_ == kInitializing ) 
			cv_.wait( lock );
		if( flag_ == kInitial )
		{
			flag_ = kInitializing;
			lock.unlock();
			single_confg_ = new Config;
			lock.lock();
			flag_ = kCompleted;
			lock.unlock();
			cv_.notify_all();
		}
	}

	return *single_confg_;
}

void Config::DestroyInstance()
{
	std::lock_guard<decltype(mutex_)> lock( mutex_ );
	delete single_confg_;
	single_confg_ = nullptr;
	flag_ = kInitial;
}

void Config::SetEnv( std::string env )
{
	env_ = env;
}

const std::string& Config::GetEnv() const
{
	return env_;
}

Config::Config()
{
	config_file_ = fopen( "env.cfg", "r+" );
}

Config::~Config()
{
	fclose( config_file_ );
}
