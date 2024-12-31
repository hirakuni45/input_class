#include <iostream>
#include <cstdio>
#include <string>
#include <boost/format.hpp>

#include "input.hpp"

namespace {

	static const char* errmsg_[] = {
			"none",				///< エラー無し
			"cha_sets",			///< 文字セットの不一致
			"partition",		///< 分離キャラクターの不一致
			"not_integer",		///< 整数型の不一致
			"sign_type",		///< 符号無し整数にマイナス符号
			"not_float",		///< 浮動小数点型の不一致
			"terminate",		///< 終端文字の不一致
			"overflow"			///< オーバーフロー
	};
	static utils::input::error err_st_;

	void report_(int no, int subidx, int subnum, const std::string& title, bool err) noexcept
	{
		if(err) {
			std::cout << "\x1B[32;1m";
		} else {
			std::cout << "\x1B[31;1m";
		}

		std::cout << boost::format("(%2d) ") % no;
		std::cout << boost::format("(%2d/%2d) ") % subidx % subnum;
		std::cout << title;

		if(err) {
			std::cout << "  Pass." << std::endl;
		} else {
			auto i = static_cast<int>(err_st_);
			std::cout << ", " << errmsg_[i] << std::endl;
		}
		std::cout << "\x1B[37;m";
	}
}

int main(int argc, char* argv[])
{
	using namespace utils;

	uint32_t exec = 0b1111'1111'1111'1111'1111'1111'1111'1111;
	int pass = 0;
	int no = 0;

// test 1 int 型整数の通常変換
	if((exec & 1) != 0) {
		int b[] = { 123456789, -123456789 };
		static const char* inp[] = { "123456789", "-123456789" };
		int sub = 0;
		for(int i = 0; i < 2; ++i) {
			int a = 99999999;
			err_st_ = (input("%d", inp[i]) % a).get_error();
			auto t = (boost::format("Integer input (type int): '%s' ---> %d") % inp[i] % a).str();
			bool f = false;
			if(a == b[i] && err_st_ == input::error::none) { f = true; }
			report_(no + 1, i + 1, 2, t, f);
			if(f) ++sub;
		}
		if(sub == 2) ++pass;
		++no;
	}
	exec >>= 1;

// test 2 浮動小数点（exponent）の正常変換
	if((exec & 1) != 0) {
		float b[] = { 2.5439e3, -2.5439e3 };
		static const char* inp[] = { "2.5439e3", "-2.5439e3" };
		int sub = 0;
		for(int i = 0; i < 2; ++i) {
			float a = 0.0f;
			err_st_ = (input("%f", inp[i]) % a).get_error();
			auto t = (boost::format("Floating point input (type float exponent): '%s' ---> %e") % inp[i] % a).str();
			bool f = false;
			if(a == b[i] && err_st_ == input::error::none) { f = true; }
			report_(no + 1, i + 1, 2, t, f);
			if(f) ++sub;
		}
		if(sub == 2) ++pass;
		++no;
	}
	exec >>= 1;

// test 3 符号無し整数の最大値変換 4294967295(u32)
	if((exec & 1) != 0) {
		uint32_t a;
		static const char* inp = { "4294967295" };
		err_st_ = (input("%d", inp) % a).get_error();
		auto t = (boost::format("Integer (uint32_t) max value conversion: '%s' ---> %u") % inp % a).str();
		bool f = false;
		if(a == std::numeric_limits<uint32_t>::max() && err_st_ == input::error::none) { f = true; }
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 4 整数の異常変換、ステータス確認、引数の初期値が変化しない事
	if((exec & 1) != 0) {
		static const char* inp = { "8o0173476" };
		int a = 123456789;
		auto b = a;
		err_st_ = (input("%d", inp) % a).get_error();
		auto t = (boost::format("Fail conversion state check, initial value hold: '%s' %d ---> %d") % inp % b % a).str();
		bool f = false;
		if(err_st_ != input::error::none && a == b) f = true;
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 5 複数の整数変換（１）
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		int a = 0;
		uint32_t b = 0;
		int c = 0;
		static const char* fom = { "%d[, ]%x[, ]%d" };
		static const char* inp = { "-98516 105aBd3,21369871" };
		auto n = (input(fom, inp) % a % b % c).num();
		auto t = (boost::format("Multi scan for integer (%s): '%s' (%d) %d, %x, %d") % fom % inp % n % a % b % c).str();
		bool f = false;
		if(n == 3 && a == -98516 && b == 0x105abd3 && c == 21369871) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 6 複数の整数変換（２）
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		int a = 0;
		uint32_t b = 0;
		uint32_t c = 0;
		static const char* fom = { "%d[, ]%x,%d" };
		static const char* inp = { "-9577463,fa4c57ab,23645687" };
		auto n = (input(fom, inp) % a % b % c).num();
		auto t = (boost::format("Multi scan for integer (%s): '%s' (%d) %d, %x, %u") % fom % inp % n % a % b % c).str();
		bool f = false;
		if(n == 3 && a == -9577463 && b == 0xfa4c57ab && c == 23645687) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 7 整数、浮動小数点、文字 の複数変換
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		int a = 0;
		float b = 0;
		char c = 0;
		static const char* fom = { "%d,%f,%c" };
		static const char* inp = { "-17743347,-1273.75941,Z" };
		auto n = (input(fom, inp) % a % b % c).num();
		auto t = (boost::format("Multi scan for (%s): '%s' (%d) %d, %f, %c") % fom % inp % n % a % b % c).str();
		bool f = false;
		if(n == 3 && a == -17743347 && b == -1273.75941f && c == 'Z') {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 8 浮動小数点、整数、１６進、８進の複数変換
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		float a;
		int b;
		uint32_t c;
		uint32_t d;
		static const char* fom = { "%f,%d,%x,%o" };
		static const char* inp = { "101.945678,-76437583,7fdcab5f,7453642341" };
		auto n = (input(fom, inp) % a % b % c % d).num();
		auto t = (boost::format("Multi scan for (%s): '%s' (%d) %f, %d, %x, %o") % fom % inp % n % a % b % c % d).str();
		bool f = false;
		if(n == 4 && a == 101.945678f && b == -76437583 && c == 0x7fdcab5f && d == 07453642341) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 9 セパレーター文字の一致検査
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		int a = 9999;
		int b = 9999;
		int c = 9999;
		static const char* fom = { " %d %d %d" };
		static const char* inp = { " 100 250 300" };
		auto n = (input(" %d %d %d", inp) % a % b % c).num();
		auto t = (boost::format("Multi scan for 'int' at spacing (%s): '%s' (%d) %d, %d, %d") % fom % inp % n % a % b % c).str();
		bool f = false;
		if(n == 3 && a == 100 && b == 250 && c == 300) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test10 auto 機能確認、0x, 0b, 0o
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		int a[4] = { -1 };
		static const char* fom = { "%a %a %a %a" };
		static const char* inp = { "100 0x9a 0b1101 0o775" };
		auto n = (input(fom, inp) % a[0] % a[1] % a[2] % a[3]).num();
		auto t = (boost::format("Multi scan for 'auto' (%s): '%s' (%d) %d, %d, %d, %d") % fom % inp % n % a[0] % a[1] % a[2] % a[3]).str();
		bool f = false;
		if(n == 4 && a[0] == 100 && a[1] == 0x9a && a[2] == 0b1101 && a[3] == 0775) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test11 (auto 機能確認、x, b, o)
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		int a[4] = { -1 };
		float b = 0.0f;
		static const char* fom = { "%a %a %a %a %a" };
		static const char* inp = { "100 x9a b1101 o775 -123.4567" };
		auto n = (input(fom, inp) % a[0] % a[1] % a[2] % a[3] % b).num();
		auto t = (boost::format("Multi scan for 'auto' (%s): '%s' (%d) %d, %d, %d, %d, %f") % fom % inp % n % a[0] % a[1] % a[2] % a[3] % b).str();
		bool f = false;
		if(n == 5 && a[0] == 100 && a[1] == 0x9a && a[2] == 0b1101 && a[3] == 0775 && b == -123.4567f) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test12 ２進、８進、１０進、１６進、浮動小数、文字、変換
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		uint32_t a = 99999;
		uint32_t b = 99998;
		int32_t c = 99997;
		uint32_t d = 99996;
		float e = 0.0f;
		char f = '_';
		static const char* fom = { "%b %o %d %x %f %c" };
		static const char* inp = { "11011 0625 -985 4a7Bc 1.025 Y" };
		auto n = (input(fom, inp) % a % b % c % d % e % f).num();
		auto t = (boost::format("Multi scan for (%s): '%s' (%d) %u, %u, %d, %x, %f, %c") % fom % inp % n % a % b % c % d % e % f).str();
		bool fl = false;
		if(n == 6 && a == 0b11011 && b == 0625 && c == -985 && d == 0x4a7bc && e == 1.025f && f == 'Y') {
			fl = true;
		}
		report_(no + 1, 1, 1, t, fl);
		if(fl) ++pass;
		++no;
	}
	exec >>= 1;

// test13 整数の符号無しエラー確認
	if((exec & 1) != 0) {
		uint32_t a = 0;
		static const char* fom = { "%d" };
		static const char* inp = { "-8964743" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Check signed integer error (%s): '%s' ---> %u") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::sign_type) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test14 符号付き整数に対する１６進変換
	if((exec & 1) != 0) {
		int32_t a = 0;
		static const char* fom = { "%x" };
		static const char* inp = { "-7ffe9283" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Check hexa-decimal conversion for signed integer (%s): '%s' ---> %x") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == -0x7ffe9283) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test15 符号無しにマイナス記号、エラー確認
	if((exec & 1) != 0) {
		uint32_t a = 0;
		static const char* fom = { "%d" };
		static const char* inp = { "-8967464" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Check signed type error (%s): '%s' ---> %u") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::sign_type) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test16 std::string 型の検査
	if((exec & 1) != 0) {
		int a;
		int b = 56789;
		std::string fom = "%d";
		std::string inp = "56789";
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Integer input 'std::string' as (%s): '%s' ---> %d") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == b) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test17 多桁の少数点以下入力
	if((exec & 1) != 0) {
		float a = 0.0f;
		float b = 0.555555555555f;
		std::string fom = "%f";
		std::string inp = "0.555555555555";
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Overflowed floting point input as (%s): '%s' ---> %f") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == b) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test18 int 型整数のオーバーフロー変換エラー確認
	if((exec & 1) != 0) {
		int a = 999999;
		static const char* fom ={ "%d" };
		static const char* inp = { "2147483648" };  // 2147483647(i32)
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Overflowed integer32 '2147483647' (%s) error check input as '%s' ---> %d") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::overflow) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test19 多桁の浮動小数点、実数オーバーフロー確認
	if((exec & 1) != 0) {
		float a = 0.0f;
		static const char* fom = { "%f" };
		static const char* inp = { "987643214321" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Overflowed floting point input as (%s): '%s' ---> %f") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::overflow) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test20 二進数のオーバーフロー変換エラー確認
	if((exec & 1) != 0) {
		uint32_t a = 999999;
		static const char* fom = { "%b" };
		static const char* inp = { "101010101010101010101010101010101" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Overflowed binary error check input (%s): '%s' ---> 0x%x") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::overflow) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test21 １６進数のオーバーフロー変換エラー確認
	if((exec & 1) != 0) {
		uint32_t a = 999999;
		static const char* fom = { "%x" };
		static const char* inp = { "a290fc4b1" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Overflowed hexa-decimal error check input (%s): '%s' ---> 0x%x") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::overflow) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// Test22: ８進数のオーバーフロー変換エラー確認
	if((exec & 1) != 0) {
		uint32_t a = 999999;
		static const char* fom = { "%o" };
		static const char* inp = { "65234223514" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Overflowed octal error check input (%s): '%s' ---> %x") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::overflow) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// Test23: 特殊文字のマッチ「%, [, ]」など
	if((exec & 1) != 0) {
		err_st_ = input::error::none;
		static const char* fom = { "%d\\%%d\\[%d\\]%d" };
		static const char* inp = { "123%456[789]5678" };
		int a[4] = { -1 };
		auto n = (input(fom, inp) % a[0] % a[1] % a[2] % a[3]).num();
		auto t = (boost::format("Special character separator (%s): '%s' (%d) %d, %d, %d, %d") % fom % inp % n % a[0] % a[1] % a[2] % a[3]).str();
		bool f = false;
		if(n == 4 && a[0] == 123 && a[1] == 456 && a[2] == 789 && a[3] == 5678) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// Test24: 多桁の実装、浮動小数点変換（実数部のみ）
	if((exec & 1) != 0) {
		float a = -1.0f;
		float b = 123456789.f;
		static const char* fom = { "%f" };
		static const char* inp = { "123456789" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Floating point conversion for (%s): '%s' ---> %f") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == b) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// Test25: 多桁の実小数点、浮動小数点変換（少数のみ）
	if((exec & 1) != 0) {
		float a = -1.0f;
		float b = .123456789f;
		static const char* fom = { "%f" };
		static const char* inp = { ".123456789" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Floating point conversion (%s): '%s) ---> %f") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == b) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// Test26: 浮動小数点変換マイナス符号
	if((exec & 1) != 0) {
		float a = -1.0f;
		float b = -.123456789f;
		static const char* fom = { "%f" };
		static const char* inp = { "-.123456789" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Floating point conversion (%s): '%s' ---> %f") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == b) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// Test27: 浮動小数点変換 exponent マイナス符号
	if((exec & 1) != 0) {
		float a = -1.0f;
		float b = .123456789e-3f;
		static const char* fom = { "%f" };
		static const char* inp = { ".123456789e-3" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Floating point conversion (%s): '%s' ---> %f") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == b) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// Test28: 浮動小数点変換 実数マイナス、exponent マイナス符号
	if((exec & 1) != 0) {
		float a = -1.0f;
		float b = -.123456789e-3f;
		static const char* fom = { "%f" };
		static const char* inp = { "-.123456789e-3" };
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Floating point conversion (%s): '%s' ---> %f") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::none && a == b) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 29 uint64_t 符号無し整数の最大値変換 18446744073709551615(u64)
	if((exec & 1) != 0) {
		uint64_t a;
		static const char* inp = { "18446744073709551615" };
		err_st_ = (input("%d", inp) % a).get_error();
		auto t = (boost::format("Integer (uint64_t) max value conversion: '%s' ---> %d") % inp % a).str();
		bool f = false;
		if(a == std::numeric_limits<uint64_t>::max() && err_st_ == input::error::none) { f = true; }
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test30 int64_t 型整数のオーバーフロー変換エラー確認
	if((exec & 1) != 0) {
		int64_t a = 999999;
		static const char* fom ={ "%d" };
		static const char* inp = { "9223372036854775808" };  // 9223372036854775807(i64)
		err_st_ = (input(fom, inp) % a).get_error();
		auto t = (boost::format("Overflowed int64_t at '9223372036854775807' (%s) error check input as '%s' ---> %d") % fom % inp % a).str();
		bool f = false;
		if(err_st_ == input::error::overflow) {
			f = true;
		}
		report_(no + 1, 1, 1, t, f);
		if(f) ++pass;
		++no;
	}
	exec >>= 1;

// test 31 short 型整数の通常変換
	if((exec & 1) != 0) {
		short b[] = { 23456, -23456 };
		static const char* inp[] = { "23456", "-23456" };
		int sub = 0;
		for(int i = 0; i < 2; ++i) {
			short a = 9999;
			err_st_ = (input("%d", inp[i]) % a).get_error();
			auto t = (boost::format("Integer input (type short): '%s' ---> %d") % inp[i] % a).str();
			bool f = false;
			if(a == b[i] && err_st_ == input::error::none) { f = true; }
			report_(no + 1, i + 1, 2, t, f);
			if(f) ++sub;
		}
		if(sub == 2) ++pass;
		++no;
	}
	exec >>= 1;

// test 32 uint16_t 型整数の通常変換
	if((exec & 1) != 0) {
		uint16_t b[] = { 45678, 56789 };
		static const char* inp[] = { "45678", "56789" };
		int sub = 0;
		for(int i = 0; i < 2; ++i) {
			uint16_t a = 9999;
			err_st_ = (input("%d", inp[i]) % a).get_error();
			auto t = (boost::format("Integer input (type uint16_t): '%s' ---> %u") % inp[i] % a).str();
			bool f = false;
			if(a == b[i] && err_st_ == input::error::none) { f = true; }
			report_(no + 1, i + 1, 2, t, f);
			if(f) ++sub;
		}
		if(sub == 2) ++pass;
		++no;
	}
	exec >>= 1;

	// auto l = std::numeric_limits<int64_t>::max();
	// std::cout << l << std::endl;

	std::cout << std::endl;
	std::cout << "input class Version: " << input::VERSION << std::endl;
	if(pass == no) {
		std::cout << boost::format("All Pass: %d/%d") % pass % no << std::endl;
	} else {
		std::cout << boost::format("Pass for: %d/%d") % pass % no << std::endl;
		return -1;
	}
}
