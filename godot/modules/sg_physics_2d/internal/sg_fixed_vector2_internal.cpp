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

#include "sg_fixed_vector2_internal.h"

const fixed SGFixedVector2Internal::FIXED_UNIT_EPSILON = fixed(65);

const SGFixedVector2Internal SGFixedVector2Internal::ZERO = SGFixedVector2Internal(fixed::ZERO, fixed::ZERO);

bool SGFixedVector2Internal::operator==(const SGFixedVector2Internal &p_v) const {
	return x == p_v.x && y == p_v.y;
}

bool SGFixedVector2Internal::operator!=(const SGFixedVector2Internal &p_v) const {
	return x != p_v.x || y != p_v.y;
}

fixed SGFixedVector2Internal::angle() const {
	return y.atan2(x);
}

SGFixedVector2Internal SGFixedVector2Internal::rotated(fixed p_rotation) const {
	SGFixedVector2Internal v;
	v.set_rotation(angle() + p_rotation);
	v *= length();
	return v;
}

void SGFixedVector2Internal::normalize() {
	// If a fixed value is less than 256, then squaring it can become 0,
	// causing this method to break with small values. Since only direction
	// matters, we can increase the vector's magnitude to avoid this.
	fixed x_abs = x.abs();
	fixed y_abs = y.abs();
	if ((x.value != 0 && x_abs.value < 256) || (y.value != 0 && y_abs.value < 256)) {
		// Watch out for values that will overflow even 64 bits.
		// 11863283 = sqrt(MAX_SIGNED_64BIT_NUMBER) / 256
		if (x_abs.value >= 11863283) {
			x = fixed::ONE;
			y = fixed::ZERO;
		}
		else if (y_abs.value >= 11863283) {
			x = fixed::ZERO;
			y = fixed::ONE;
		}
		else {
			// Multiply X and Y by 256.
			fixed x_big = fixed(x.value << 8);
			fixed y_big = fixed(y.value << 8);
			fixed l = SGFixedVector2Internal(x_big, y_big).length();
			if (l != fixed::ZERO) {
				x = x_big / l;
				y = y_big / l;
			}
		}
	}
	else {
		fixed l = length();
		if (l != fixed::ZERO) {
			x /= l;
			y /= l;
		}
	}
}

SGFixedVector2Internal SGFixedVector2Internal::normalized() const {
	SGFixedVector2Internal v = *this;
	v.normalize();
	return v;
}

bool SGFixedVector2Internal::is_normalized() const {
	return fixed::is_equal_approx(length_squared(), fixed::ONE, SGFixedVector2Internal::FIXED_UNIT_EPSILON);
}

fixed SGFixedVector2Internal::length() const {
	// By directly using 64-bit integers we can avoid a left shift, since
	// multiplying two fixed point numbers effectively shifts them left.
	return fixed(sg_sqrt_64(x.value * x.value + y.value * y.value));
}

fixed SGFixedVector2Internal::length_squared() const {
	return x * x + y * y;
}

fixed SGFixedVector2Internal::distance_to(const SGFixedVector2Internal &p_other) const {
	return fixed(sg_sqrt_64((p_other.x.value - x.value) * (p_other.x.value - x.value) + (p_other.y.value - y.value) * (p_other.y.value - y.value)));
}

fixed SGFixedVector2Internal::distance_squared_to(const SGFixedVector2Internal &p_other) const {
	return fixed((p_other.x.value - x.value) * (p_other.x.value - x.value) + (p_other.y.value - y.value) * (p_other.y.value - y.value));
}

fixed SGFixedVector2Internal::angle_to(const SGFixedVector2Internal &p_other) const {
	return cross(p_other).atan2(dot(p_other));
}

fixed SGFixedVector2Internal::angle_to_point(const SGFixedVector2Internal &p_other) const {
	return (y - p_other.y).atan2(x - p_other.x);
}

fixed SGFixedVector2Internal::dot(const SGFixedVector2Internal &p_other) const {
	return x * p_other.x + y * p_other.y;
}

fixed SGFixedVector2Internal::cross(const SGFixedVector2Internal &p_other) const {
	return x * p_other.y - y * p_other.x;
}

/*
fixed SGFixedVector2Internal::posmod(const fixed p_mod) const {

}

fixed SGFixedVector2Internal::posmodv(const SGFixedVector2Internal &p_modv) const {

}

SGFixedVector2Internal SGFixedVector2Internal::project(const SGFixedVector2Internal &p_to) const {
	return p_to * (dot(p_to) / p_to.length_squared());
}
*/

SGFixedVector2Internal SGFixedVector2Internal::slide(const SGFixedVector2Internal &p_normal) const {
#ifdef MATH_CHECKS
	ERR_FAIL_COND_V_MSG(!p_normal.is_normalized(), SGFixedVector2Internal(), "The normal SGFixedVector2Internal must be normalized.");
#endif
	return *this - p_normal * this->dot(p_normal);
}

SGFixedVector2Internal SGFixedVector2Internal::bounce(const SGFixedVector2Internal &p_normal) const {
	return -reflect(p_normal);
}

SGFixedVector2Internal SGFixedVector2Internal::reflect(const SGFixedVector2Internal &p_normal) const {
#ifdef MATH_CHECKS
	ERR_FAIL_COND_V_MSG(!p_normal.is_normalized(), SGFixedVector2Internal(), "The normal SGFixedVector2Internal must be normalized.");
#endif
	return p_normal * fixed::TWO * this->dot(p_normal) - *this;
}

bool SGFixedVector2Internal::is_equal_approx(const SGFixedVector2Internal &p_v) const {
	return fixed::is_equal_approx(x, p_v.x) && fixed::is_equal_approx(y, p_v.y);
}

