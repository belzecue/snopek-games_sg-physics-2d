/*************************************************************************/
/* Copyright (c) 2021 David Snopek                                       */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef SG_FIXED_NUMBER_INTERNAL_H
#define SG_FIXED_NUMBER_INTERNAL_H

#include <core/typedefs.h>
#include <core/error_macros.h>

int64_t sg_sqrt_64(int64_t num);

// To avoid overflow related to flipping the sign on twos-complement
// representation of of the minimum integer, we effectively limit values to
// 1 above the minimum.
#define INT64_EFFECTIVE_MIN (INT64_MIN + 1)

#ifdef DEBUG_ENABLED
#define SG_FIXED_MATH_CHECKS
#endif

struct fixed {
	int64_t value;

	_FORCE_INLINE_ fixed() : value(0) {}

	explicit _FORCE_INLINE_ fixed(int64_t p_initial_value)
		: value(p_initial_value) {}

	static const fixed ZERO;
	static const fixed ONE;
	static const fixed HALF;
	static const fixed TWO;
	static const fixed NEG_ONE;
	static const fixed PI;
	static const fixed TAU;
	static const fixed PI_DIV_4;
	static const fixed EPSILON;
	static const fixed ARITHMETIC_OVERFLOW;

	static _FORCE_INLINE_ fixed from_int(int64_t p_int_value) {
		return fixed(p_int_value << 16);
	}
	
	static _FORCE_INLINE_ fixed from_float(float p_float_value) {
		return fixed(p_float_value * 65536);
	}

	static _FORCE_INLINE_ bool is_equal_approx(fixed a, fixed b) {
		if (a == b) {
			return true;
		}
		fixed tolerance = fixed::EPSILON * a.abs();
		if (tolerance < fixed::EPSILON) {
			tolerance = fixed::EPSILON;
		}
		return (a - b).abs() < tolerance;
	}

	static _FORCE_INLINE_ bool is_equal_approx(fixed a, fixed b, fixed tolerance) {
		if (a == b) {
			return true;
		}
		return (a - b).abs() < tolerance;
	}

	_FORCE_INLINE_ int64_t to_int() const {
		return value >> 16;
	}

	_FORCE_INLINE_ float to_float() const {
		return (float)value / 65536.0f;
	}

	_FORCE_INLINE_ fixed operator+(const fixed& p_other) const {
		#ifdef SG_FIXED_MATH_CHECKS
		ERR_FAIL_COND_V_MSG(p_other.value > 0 && (value > INT64_MAX - p_other.value),
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point addition overflow");
		ERR_FAIL_COND_V_MSG(p_other.value < 0 && (value < INT64_EFFECTIVE_MIN - p_other.value),
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point addition overflow");
		#endif

		return fixed(value + p_other.value);
	}

	_FORCE_INLINE_ void operator+=(const fixed& p_other) {
		value = (*this + p_other).value;
	}

	_FORCE_INLINE_ fixed operator-(const fixed& p_other) const {
		#ifdef SG_FIXED_MATH_CHECKS
		ERR_FAIL_COND_V_MSG(p_other.value < 0 && (value > INT64_MAX + p_other.value),
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point subtraction overflow");
		ERR_FAIL_COND_V_MSG(p_other.value > 0 && (value < INT64_EFFECTIVE_MIN + p_other.value),
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point subtraction overflow");
		#endif

		return fixed(value - p_other.value);
	}

	_FORCE_INLINE_ void operator-=(const fixed& p_other) {
		value = (*this - p_other).value;
	}

	_FORCE_INLINE_ fixed operator*(const fixed& p_other) const {
		#ifdef SG_FIXED_MATH_CHECKS
		ERR_FAIL_COND_V_MSG(value == -1 && p_other.value == INT64_MAX,
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point multiplication overflow");
		ERR_FAIL_COND_V_MSG(p_other.value == -1 && value == INT64_MIN,
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point multiplication overflow");
		ERR_FAIL_COND_V_MSG(p_other.value > 0 && (value > (INT64_MAX / p_other.value) || value < (INT64_EFFECTIVE_MIN / p_other.value)),
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point multiplication overflow");
		ERR_FAIL_COND_V_MSG(p_other.value < 0 && (value < (INT64_MAX / p_other.value) || value > (INT64_EFFECTIVE_MIN / p_other.value)),
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point multiplication overflow");
		#endif

		return fixed((value * p_other.value) >> 16);
	}

	_FORCE_INLINE_ void operator*=(const fixed& p_other) {
		value = (*this * p_other).value;
	}

	_FORCE_INLINE_ fixed operator/(const fixed& p_other) const {
		#ifdef SG_FIXED_MATH_CHECKS
		ERR_FAIL_COND_V_MSG(value == -1 && p_other.value == INT64_MAX,
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point division overflow");
		ERR_FAIL_COND_V_MSG(p_other.value == -1 && value == INT64_MIN,
			fixed::ARITHMETIC_OVERFLOW, "Fixed-point division overflow");
		#endif

		return fixed((value << 16) / p_other.value);
	}

	_FORCE_INLINE_ void operator/=(const fixed& p_other) {
		value = (*this / p_other).value;
	}

	_FORCE_INLINE_ bool operator==(const fixed &p_other) const { return value == p_other.value; }
	_FORCE_INLINE_ bool operator!=(const fixed &p_other) const { return value != p_other.value; }
	_FORCE_INLINE_ bool operator<=(const fixed &p_other) const { return value <= p_other.value; }
	_FORCE_INLINE_ bool operator>=(const fixed &p_other) const { return value >= p_other.value; }
	_FORCE_INLINE_ bool operator< (const fixed &p_other) const { return value <  p_other.value; }
	_FORCE_INLINE_ bool operator> (const fixed &p_other) const { return value >  p_other.value; }

	_FORCE_INLINE_ fixed abs() const { return (value < 0) ? fixed(-value) : *this; }
	_FORCE_INLINE_ fixed operator-() const { return fixed(-value); }
	_FORCE_INLINE_ fixed sqrt() const { return fixed(sg_sqrt_64(value << 16)); }

	fixed  sin() const;
	fixed  cos() const;
	fixed  tan() const;
	fixed asin() const;
	fixed acos() const;
	fixed atan() const;
	fixed atan2(const fixed &inY) const;
};

#define FIXED_SGN(m_v) (((m_v) < fixed::ZERO) ? fixed::NEG_ONE : fixed::ONE)

#endif
