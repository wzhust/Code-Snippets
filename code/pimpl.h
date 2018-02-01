// show pimpl idiom - Compilation firewall
#ifndef PIMPL_H_
#define PIMPL_H_
#include <memory>
#include <string>
#include <vector>

class KoIme
{
public:
	KoIme();
	~KoIme();

	KoIme( KoIme&& );
	KoIme& operator=( KoIme&& );

	std::vector<std::u16string> Conv( const std::u16string& input ) const;

private:
	class Impl;
	static void Deleter( Impl* );
	// struct Deleter{ void operator()( Impl* ) };
	std::unique_ptr<Impl, decltype(&Deleter)> impl_; // use Functor Deleter to reduce KoIme size
};

#endif
