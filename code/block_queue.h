// BlockQueue
#ifndef BLOCK_QUEUE_H_
#define BLOCK_QUEUE_H_
#include <vector>
#include <mutex>

template<typename T>
class BlockQueue
{
public:
	explicit BlockQueue( size_t max_elem );

	template<typename E>
	void Put( E&& elem );

	template<typename E, typename Rep, typename Period>
	bool Put( E&& elem, const std::chrono::duration<Rep, Period>& rel_time );

	T Take();

	template<typename Rep, typename Period>
	bool Take( const std::chrono::duration<Rep, Period>& rel_time, T& elem );

	size_t Size() const;
	bool Empty() const;

	BlockQueue( const BlockQueue& ) = delete;
	BlockQueue& operator=( const BlockQueue& ) = delete;

private:
	mutable std::mutex mutex_;
	std::condition_variable cv_not_full_;
	std::condition_variable cv_not_empty_;
	std::vector<T> vq_;
	size_t take_index_ = 0;
	size_t put_index_ = 0;
	size_t count_ = 0;
	size_t max_elem_ = 0;
};

template<typename T>
BlockQueue<T>::BlockQueue( size_t max_elem ) 
	: vq_( max_elem ), max_elem_( max_elem )
{
}

template<typename T>
template<typename E>
void BlockQueue<T>::Put( E&& elem )
{
	std::unique_lock<decltype(mutex_)> lock( mutex_ );
	while( count_ == max_elem_ )
		cv_not_full_.wait( lock );

	vq_[put_index_] = std::forward<E>( elem );
	++put_index_;
	if( put_index_ == max_elem_ )
		put_index_ = 0;
	++count_;
	
	cv_not_empty_.notify_one();
}

template<typename T>
template<typename E, typename Rep, typename Period>
bool BlockQueue<T>::Put( E&& elem, const std::chrono::duration<Rep, Period>& rel_time )
{
	std::unique_lock<decltype(mutex_)> lock( mutex_ );
	while( count_ == max_elem_ )
	{
		if( cv_not_full_.wait_for( lock, rel_time ) == std::cv_status::timeout && 
			count_ == max_elem_ )
		{
			return false;
		}
	}

	vq_[put_index_] = std::forward<E>( elem );
	++put_index_;
	if( put_index_ == max_elem_ )
		put_index_ = 0;
	++count_;

	cv_not_empty_.notify_one();
	return true;
}

template<typename T>
T BlockQueue<T>::Take()
{
	std::unique_lock<decltype(mutex_)> lock( mutex_ );
	while( count_ == 0 )
		cv_not_empty_.wait( lock );

	T front( std::move( vq_[take_index_] ) );
	++take_index_;
	if( take_index_ == max_elem_ )
		take_index_ = 0;
	--count_;

	cv_not_full_.notify_one();
	return front;
}

template<typename T>
template<typename Rep, typename Period>
bool BlockQueue<T>::Take( const std::chrono::duration<Rep, Period>& rel_time, T& elem )
{
	std::unique_lock<decltype(mutex_)> lock( mutex_ );
	while( count_ == 0 )
	{
		if( cv_not_empty_.wait_for( lock, rel_time ) == std::cv_status::timeout && 
			count_ == 0 )
		{
			return false;
		}
	}

	elem = std::move( vq_[take_index_] );
	++take_index_;
	if( take_index_ == max_elem_ )
		take_index_ = 0;
	cv_not_full_.notify_one();
	--count_;

	return true;
}

template<typename T>
size_t BlockQueue<T>::Size() const
{
	std::lock_guard<decltype(mutex_)> lock( mutex_ );
	return count_;
}

template<typename T>
bool BlockQueue<T>::Empty() const
{
	std::lock_guard<decltype(mutex_)> lock( mutex_ );
	return count_ == 0;
}

#endif
