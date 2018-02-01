#include "pimpl.h"

class KoIme::Impl
{
public:
	std::vector<std::u16string> Conv( const std::u16string& input ) const;

private:
	std::vector<std::u16string> result_;
};

std::vector<std::u16string> KoIme::Impl::Conv( const std::u16string & input ) const
{
	return result_;
}

void KoIme::Deleter( Impl* p )
{
	delete p;
}

KoIme::KoIme() : impl_( std::unique_ptr<Impl, decltype(&Deleter)>( new Impl, &Deleter ) )
{
}

KoIme::~KoIme() = default; // necessary to prevent compiler to produce default inline destructor

KoIme::KoIme( KoIme&& ) = default;
KoIme& KoIme::operator=( KoIme&& ) = default; // necessary if you want it to be movable

std::vector<std::u16string> KoIme::Conv( const std::u16string& input ) const
{
	return impl_->Conv( input );
}
