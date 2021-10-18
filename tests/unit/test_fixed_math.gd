extends "res://addons/gut/test.gd"

func test_float_conversion():
	var a: float = 2.4
	var b: int = SGFixed.from_float(a)
	var c: float = SGFixed.to_float(b)
	
	assert_eq(b, 157286)
	assert_eq("%s" % c, "2.399994")

func test_int_conversion():
	var a: int = 2
	var b: int = SGFixed.from_int(a)
	var c: float = SGFixed.to_int(b)
	
	assert_eq(b, 65536*2)
	assert_eq(c, 2.0)

func test_multiplication():
	var a: int = SGFixed.from_float(2.5)
	var b: int = SGFixed.from_float(2.5)
	var c: int = SGFixed.mul(a, b)
	
	assert_eq(c, 409600)

func test_division():
	var a: int = SGFixed.from_int(15)
	var b: int = SGFixed.from_int(2)
	var c: int = SGFixed.div(a, b)
	
	assert_eq(c, 491520)

func test_trig():
	var sin_10: int = SGFixed.sin(10*65536)
	assert_eq(sin_10, -35653)
	
	var sin_50000: int = SGFixed.sin(50000*65536)
	assert_eq(sin_50000, -65309)

	var cos_10: int = SGFixed.cos(10*65536)
	assert_eq(cos_10, -54990)

#	var cos_50000: int = SGFixed.cos(50000*65536)
#	assert_eq(cos_50000, 1000)
#
#	var tan_10: int = SGFixed.tan(10*65536)
#	assert_eq(tan_10, 1000)
#
#	var tan_50000: int = SGFixed.tan(50000*65536)
#	assert_eq(tan_50000, 1000)
#
#	var asin_10: int = SGFixed.asin(10*65536)
#	assert_eq(asin_10, -35653)
#
#	var asin_50000: int = SGFixed.asin(50000*65536)
#	assert_eq(asin_50000, -65309)
#
#	var acos_10: int = SGFixed.acos(10*65536)
#	assert_eq(acos_10, 1000)
#
#	var acos_50000: int = SGFixed.acos(50000*65536)
#	assert_eq(acos_50000, 1000)
#
#	var atan_10: int = SGFixed.atan(10*65536)
#	assert_eq(atan_10, 1000)
#
#	var atan_50000: int = SGFixed.atan(50000*65536)
#	assert_eq(atan_50000, 1000)
#
#	var atan2_10: int = SGFixed.atan2(10*65536, 2*65536)
#	assert_eq(atan2_10, 1000)
#
#	var atan2_50000: int = SGFixed.atan2(50000*65536, 2*65536)
#	assert_eq(atan2_50000, 1000)

