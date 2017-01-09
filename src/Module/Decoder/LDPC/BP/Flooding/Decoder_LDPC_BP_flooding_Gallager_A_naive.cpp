#include "Decoder_LDPC_BP_flooding_Gallager_A_naive.hpp"

#include <limits>

#include "Tools/Display/bash_tools.h"
#include "Tools/Math/utils.h"

template <typename B, typename R>
Decoder_LDPC_BP_flooding_Gallager_A_naive<B,R>
::Decoder_LDPC_BP_flooding_Gallager_A_naive(const int &K, const int &N, const int& n_ite, const AList_reader &H,
                                            const int n_frames, const std::string name)
: Decoder_SISO<B,R>(K, N, n_frames, 1, name   ),
  n_ite            (n_ite                     ),
  H                (H                         ),
  transpose        (H.get_branches_transpose()),
  Y_N              (N                         ),
  C_to_V           (H.get_n_branches(),      0),
  V_to_C           (H.get_n_branches(),      0)
{
	assert(this->N == (int)H.get_n_VN());
	assert(n_ite > 0);
}

template <typename B, typename R>
Decoder_LDPC_BP_flooding_Gallager_A_naive<B,R>
::~Decoder_LDPC_BP_flooding_Gallager_A_naive()
{
}

template <typename B, typename R>
void Decoder_LDPC_BP_flooding_Gallager_A_naive<B,R>
::load(const mipp::vector<R>& Y_N_chn)
{
	assert(Y_N_chn.size() >= Y_N.size());

	for (auto i = 0; i < this->N; i++)
		Y_N[i] = Y_N_chn[i] < 0;
}

template <typename B, typename R>
void Decoder_LDPC_BP_flooding_Gallager_A_naive<B,R>
::hard_decode()
{
	for (auto ite = 0; ite < n_ite; ite++)
	{
		auto C_to_V_ptr = C_to_V.data();
		auto V_to_C_ptr = V_to_C.data();

		// V -> C
		for (auto i = 0; i < H.get_n_VN(); i++)
		{
			const auto length = (int)H.get_VN_to_CN()[i].size();
			for (auto j = 0; j < length; j++)
			{
				auto cur_state = Y_N[i];
				if (ite > 0)
				{
					auto count = 0;
					for (auto k = 0; k < length; k++)
						if (k != j && C_to_V_ptr[k] != cur_state)
							count++;

					cur_state = count == (length -1) ? !cur_state : cur_state;
				}

				V_to_C_ptr[j] = cur_state;
			}

			C_to_V_ptr += length; // jump to the next node
			V_to_C_ptr += length; // jump to the next node
		}

		// C -> V
		auto syndrome = 0;
		auto transpose_ptr = transpose.data();
		for (auto i = 0; i < H.get_n_CN(); i++)
		{
			const auto length = (int)H.get_CN_to_VN()[i].size();

			// accumulate the incoming information in CN
			auto acc = 0;
			for (auto j = 0; j < length; j++)
				acc ^= V_to_C[transpose_ptr[j]];

			// regenerate the CN outcoming values
			for (auto j = 0; j < length; j++)
				C_to_V[transpose_ptr[j]] = acc ^ V_to_C[transpose_ptr[j]];

			transpose_ptr += length;
			syndrome = syndrome || acc;
		}

		// stop criterion
		if (syndrome == 0)
			break;
	}
}

template <typename B, typename R>
void Decoder_LDPC_BP_flooding_Gallager_A_naive<B,R>
::store(mipp::vector<B>& V_K) const
{
	assert((int)V_K.size() >= this->K);

	// majority vote
	auto C_to_V_ptr = C_to_V.data();
	for (auto i = 0; i < this->K; i++)
	{
		const auto length = (int)H.get_VN_to_CN()[i].size();
		auto count = 0;

		for (auto j = 0; j < length; j++)
			count += C_to_V_ptr[j] ? 1 : -1;

		if (length % 2 == 0)
			count += Y_N[i] ? 1 : -1;

		// take the hard decision
		V_K[i] = count > 0 ? 1 : 0;

		C_to_V_ptr += length;
	}
}

template <typename B, typename R>
void Decoder_LDPC_BP_flooding_Gallager_A_naive<B,R>
::soft_decode(const mipp::vector<R> &sys, const mipp::vector<R> &par, mipp::vector<R> &ext)
{
	std::cerr << bold_red("(EE) This decoder does not support this interface.") << std::endl;
	std::exit(-1);
}

template <typename B, typename R>
void Decoder_LDPC_BP_flooding_Gallager_A_naive<B,R>
::_soft_decode(const mipp::vector<R> &Y_N1, mipp::vector<R> &Y_N2)
{
	std::cerr << bold_red("(EE) This decoder does not support this interface.") << std::endl;
	std::exit(-1);
}

// ==================================================================================== explicit template instantiation 
#include "Tools/types.h"
#ifdef MULTI_PREC
template class Decoder_LDPC_BP_flooding_Gallager_A_naive<B_8,Q_8>;
template class Decoder_LDPC_BP_flooding_Gallager_A_naive<B_16,Q_16>;
template class Decoder_LDPC_BP_flooding_Gallager_A_naive<B_32,Q_32>;
template class Decoder_LDPC_BP_flooding_Gallager_A_naive<B_64,Q_64>;
#else
template class Decoder_LDPC_BP_flooding_Gallager_A_naive<B,Q>;
#endif
// ==================================================================================== explicit template instantiation