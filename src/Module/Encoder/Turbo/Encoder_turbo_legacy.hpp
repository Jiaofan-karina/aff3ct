#ifndef ENCODER_TURBO_LEGACY_HPP_
#define ENCODER_TURBO_LEGACY_HPP_

#include <vector>
#include <string>

#include "Module/Interleaver/Interleaver.hpp"

#include "Encoder_turbo.hpp"

namespace aff3ct
{
namespace module
{
template <typename B = int>
class Encoder_turbo_legacy : public Encoder_turbo<B>
{
protected:
	const Interleaver<B> &pi;      // the interleaver
	Encoder    <B>       &sub_enc; // sub encoder
	std::vector<B>        X_N_n;   // internal buffer for the encoded    bits in the natural     domain
	std::vector<B>        X_N_i;   // internal buffer for the encoded    bits in the interleaved domain

public:
	Encoder_turbo_legacy(const int& K, const int& N, const Interleaver<B> &pi, Encoder<B> &sub_enc,
	                     const int n_frames = 1);
	virtual ~Encoder_turbo_legacy() {}

	virtual void encode(const B *U_K, B *X_N); using Encoder<B>::encode;

	bool is_codeword(const B *X_N);
};
}
}

#endif // ENCODER_TURBO_LEGACY_HPP_
