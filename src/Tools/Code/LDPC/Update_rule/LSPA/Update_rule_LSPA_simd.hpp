#ifndef UPDATE_RULE_LSPA_SIMD_HPP
#define UPDATE_RULE_LSPA_SIMD_HPP

#include <sstream>
#include <cassert>
#include <vector>
#include <limits>
#include <string>
#include <mipp.h>

#include "Tools/Exception/exception.hpp"

namespace aff3ct
{
namespace tools
{
template <typename R = float>
class Update_rule_LSPA_simd // Log Sum Product Algorithm
{
protected:
	const std::string name;
	const mipp::Msk<mipp::N<R>()> false_msk;
	const mipp::Reg<R> zero;
	const mipp::Reg<R> half;
	const mipp::Reg<R> two;
	const mipp::Reg<R> min;
	const mipp::Reg<R> epsilon1m;
	std::vector<mipp::Reg<R>> values;
	mipp::Msk<mipp::N<R>()> sign;
	mipp::Reg<R> sum;

	int n_ite;
	int ite;

public:
	Update_rule_LSPA_simd(const unsigned max_chk_node_degree)
	: name("LSPA"), false_msk(false), zero((R)0), half((R)0.5), two((R)2), min(std::numeric_limits<R>::min()),
	  epsilon1m((R)1.0 - std::numeric_limits<R>::epsilon()), values(max_chk_node_degree), sign(false), sum(zero),
	  n_ite(0), ite(0)
	{
		if (max_chk_node_degree == 0)
		{
			std::stringstream message;
			message << "'max_chk_node_degree' has to greater than 0.";
			throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
		}

		if (typeid(R) != typeid(double) && typeid(R) != typeid(float))
		{
			std::stringstream message;
			message << "The 'LSPA' update rule supports only 'float' or 'double' datatypes.";
			throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
		}
	}

	virtual ~Update_rule_LSPA_simd()
	{
	}

	std::string get_name() const
	{
		return this->name;
	}

	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------

	inline void begin_decoding(const int n_ite)
	{
		this->n_ite = n_ite;
	}

	inline void begin_ite(const int ite)
	{
		this->ite = ite;
	}

	// incoming values from the variable nodes into the check nodes
	inline void begin_chk_node_in(const int chk_id, const int chk_degree)
	{
		assert(chk_degree <= (int)this->values.size());

		this->sign = this->false_msk;
		this->sum  = this->zero;
	}

	inline void compute_chk_node_in(const int var_id, const mipp::Reg<R> var_val)
	{
		const auto var_abs     = mipp::abs(var_val);
		const auto tan_var_abs = mipp::tanh(var_abs * this->half);
		const auto res         = mipp::blend(mipp::log(tan_var_abs), this->min, tan_var_abs != this->zero);
		const auto var_sign    = mipp::sign(var_val);

		this->sign          ^= var_sign;
		this->sum           += res;
		this->values[var_id] = res;
	}

	inline void end_chk_node_in()
	{
	}

	// outcomming values from the check nodes into the variable nodes
	inline void begin_chk_node_out(const int chk_id, const int chk_degree)
	{
	}

	inline mipp::Reg<R> compute_chk_node_out(const int var_id, const mipp::Reg<R> var_val)
	{
		      auto res_tmp = this->sum - this->values[var_id];
		           res_tmp = mipp::blend(mipp::exp(res_tmp), epsilon1m, res_tmp != this->zero);
		const auto res_abs = this->two * mipp::atanh(res_tmp);
		const auto res_sgn = this->sign ^ mipp::sign(var_val);

		return mipp::copysign(res_abs, res_sgn);
	}

	inline void end_chk_node_out()
	{
	}

	inline void end_ite()
	{
	}

	inline void end_decoding()
	{
	}
};
}
}

#endif /* UPDATE_RULE_LSPA_SIMD_HPP */
