#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"

#include <limits.h>
#include <float.h>
#include <math.h>

#define PRINT_CHECK(_expected, format, ...)   \
    do {                                      \
        printf(_expected "\r\n");             \
        printf(format "\r\n", ##__VA_ARGS__); \
    } while (0)

typedef void (*testfunc_t)(void);

#define DEFCASE(_name) static void test_##_name(void)
#define TEST(_name)    test_##_name

DEFCASE(space_flag)
{
    PRINT_CHECK(" 42", "% d", 42);
    PRINT_CHECK("-42", "% d", -42);
    PRINT_CHECK("   42", "% 5d", 42);
    PRINT_CHECK("  -42", "% 5d", -42);
    PRINT_CHECK("             42", "% 15d", 42);
    PRINT_CHECK("            -42", "% 15d", -42);
    PRINT_CHECK("        -42.987", "% 15.3f", -42.987);
    PRINT_CHECK("         42.987", "% 15.3f", 42.987);
    PRINT_CHECK(" 1024", "% d", 1024);
    PRINT_CHECK("-1024", "% d", -1024);
    PRINT_CHECK(" 1024", "% i", 1024);
    PRINT_CHECK("-1024", "% i", -1024);
}

DEFCASE(plus_flag)
{
    PRINT_CHECK("+42", "%+d", 42);
    PRINT_CHECK("-42", "%+d", -42);
    PRINT_CHECK("  +42", "%+5d", 42);
    PRINT_CHECK("  -42", "%+5d", -42);
    PRINT_CHECK("            +42", "%+15d", 42);
    PRINT_CHECK("            -42", "%+15d", -42);
    PRINT_CHECK("+1024", "%+d", 1024);
    PRINT_CHECK("-1024", "%+d", -1024);
    PRINT_CHECK("+1024", "%+i", 1024);
    PRINT_CHECK("-1024", "%+i", -1024);
    PRINT_CHECK("+", "%+0.d", 0);
}

DEFCASE(zero_flag)
{
    PRINT_CHECK("42", "%0d", 42);
    PRINT_CHECK("42", "%0ld", 42L);
    PRINT_CHECK("-42", "%0d", -42);
    PRINT_CHECK("00042", "%05d", 42);
    PRINT_CHECK("-0042", "%05d", -42);
    PRINT_CHECK("000000000000042", "%015d", 42);
    PRINT_CHECK("-00000000000042", "%015d", -42);
    PRINT_CHECK("000000000042.12", "%015.2f", 42.1234);
    PRINT_CHECK("00000000042.988", "%015.3f", 42.9876);
    PRINT_CHECK("-00000042.98760", "%015.5f", -42.9876);
}

DEFCASE(minus_flag)
{
    PRINT_CHECK("42", "%-d", 42);
    PRINT_CHECK("-42", "%-d", -42);
    PRINT_CHECK("42   ", "%-5d", 42);
    PRINT_CHECK("-42  ", "%-5d", -42);
    PRINT_CHECK("42             ", "%-15d", 42);
    PRINT_CHECK("-42            ", "%-15d", -42);
}

DEFCASE(sharp_flag)
{
    PRINT_CHECK("0", "%#o", 0);
    PRINT_CHECK("0", "%#0o", 0);
    PRINT_CHECK("0", "%#.0o", 0);
    PRINT_CHECK("0", "%#.1o", 0);
    PRINT_CHECK("  0", "%#3o", 0);
    PRINT_CHECK("000", "%#.3o", 0);
    PRINT_CHECK("01", "%#o", 1);
    PRINT_CHECK("01", "%#0o", 1);
    PRINT_CHECK("01", "%#.0o", 1);
    PRINT_CHECK("01", "%#.1o", 1);
    PRINT_CHECK(" 01", "%#3o", 1);
    PRINT_CHECK("0001", "%#.4o", 1);
    PRINT_CHECK("0x1001", "%#04x", 0x1001);
    PRINT_CHECK("01001", "%#04o", 01001);
    PRINT_CHECK("", "%#.0x", 0);
    PRINT_CHECK("0x0000614e", "%#.8x", 0x614e);
}

DEFCASE(sharp_flag_with_long_long)
{
    PRINT_CHECK("0", "%#llo", (long long)0);
    PRINT_CHECK("0", "%#0llo", (long long)0);
    PRINT_CHECK("0", "%#.0llo", (long long)0);
    PRINT_CHECK("0", "%#.1llo", (long long)0);
    PRINT_CHECK("   0", "%#4llo", (long long)0);
    PRINT_CHECK("0000", "%#.4llo", (long long)0);
    PRINT_CHECK("01", "%#llo", (long long)1);
    PRINT_CHECK("01", "%#0llo", (long long)1);
    PRINT_CHECK("01", "%#.0llo", (long long)1);
    PRINT_CHECK("01", "%#.1llo", (long long)1);
    PRINT_CHECK("  01", "%#4llo", (long long)1);
    PRINT_CHECK("0001", "%#.4llo", (long long)1);
    PRINT_CHECK("0x1001", "%#04llx", (long long)0x1001);
    PRINT_CHECK("01001", "%#04llo", (long long)01001);
    PRINT_CHECK("", "%#.0llx", (long long)0);
    PRINT_CHECK("0x0000614e", "%#.8llx", (long long)0x614e);
}

DEFCASE(specifier)
{
    PRINT_CHECK("Hello World", "Hello World");
    PRINT_CHECK("Hello World", "%s", "Hello World");
    PRINT_CHECK("(null)", "%s", (const char *)NULL);
    PRINT_CHECK("1024", "%d", 1024);
#if INT_MAX >= 2147483647LL
    PRINT_CHECK("2147483647", "%d", 2147483647);
    PRINT_CHECK("4294966272", "%u", 4294966272U);
    PRINT_CHECK("37777777001", "%o", 4294966785U);
    PRINT_CHECK("1234abcd", "%x", 305441741);
    PRINT_CHECK("edcb5433", "%x", 3989525555U);
    PRINT_CHECK("1234ABCD", "%X", 305441741);
    PRINT_CHECK("EDCB5433", "%X", 3989525555U);
#endif
    PRINT_CHECK("-1024", "%d", -1024);
    PRINT_CHECK("1024", "%i", 1024);
    PRINT_CHECK("-1024", "%i", -1024);
    PRINT_CHECK("1024", "%u", 1024);
    PRINT_CHECK("777", "%o", 511);
    PRINT_CHECK("%%", "%%");
}

DEFCASE(width)
{
    PRINT_CHECK("Hello World", "%1s", "Hello World");
    PRINT_CHECK("1024", "%1d", 1024);
    PRINT_CHECK("-1024", "%1d", -1024);
    PRINT_CHECK("1024", "%1i", 1024);
    PRINT_CHECK("-1024", "%1i", -1024);
    PRINT_CHECK("1024", "%1u", 1024);
    PRINT_CHECK("4294966272", "%1u", 4294966272U);
    PRINT_CHECK("777", "%1o", 511);
    PRINT_CHECK("37777777001", "%1o", 4294966785U);
    PRINT_CHECK("1234abcd", "%1x", 305441741);
    PRINT_CHECK("edcb5433", "%1x", 3989525555U);
    PRINT_CHECK("1234ABCD", "%1X", 305441741);
    PRINT_CHECK("EDCB5433", "%1X", 3989525555U);
    PRINT_CHECK("x", "%1c", 'x');
}

DEFCASE(width_20)
{
    PRINT_CHECK("               Hello", "%20s", "Hello");
    PRINT_CHECK("                1024", "%20d", 1024);
    PRINT_CHECK("               -1024", "%20d", -1024);
    PRINT_CHECK("                1024", "%20i", 1024);
    PRINT_CHECK("               -1024", "%20i", -1024);
    PRINT_CHECK("                   0", "%20i", 0);
    PRINT_CHECK("                1024", "%20u", 1024);
    PRINT_CHECK("          4294966272", "%20u", 4294966272U);
    PRINT_CHECK("                 777", "%20o", 511);
    PRINT_CHECK("         37777777001", "%20o", 4294966785U);
    PRINT_CHECK("            1234abcd", "%20x", 305441741);
    PRINT_CHECK("            edcb5433", "%20x", 3989525555U);
    PRINT_CHECK("            1234ABCD", "%20X", 305441741);
    PRINT_CHECK("            EDCB5433", "%20X", 3989525555U);
    PRINT_CHECK("                   0", "%20X", 0);
    PRINT_CHECK("                   0", "%20X", 0U);
    PRINT_CHECK("                   0", "%20llX", 0ULL);
    PRINT_CHECK("                   x", "%20c", 'x');
}

DEFCASE(width_asterisk_20)
{
    PRINT_CHECK("               Hello", "%*s", 20, "Hello");
    PRINT_CHECK("                1024", "%*d", 20, 1024);
    PRINT_CHECK("               -1024", "%*d", 20, -1024);
    PRINT_CHECK("                1024", "%*i", 20, 1024);
    PRINT_CHECK("               -1024", "%*i", 20, -1024);
    PRINT_CHECK("                1024", "%*u", 20, 1024);
    PRINT_CHECK("          4294966272", "%*u", 20, 4294966272U);
    PRINT_CHECK("                 777", "%*o", 20, 511);
    PRINT_CHECK("         37777777001", "%*o", 20, 4294966785U);
    PRINT_CHECK("            1234abcd", "%*x", 20, 305441741);
    PRINT_CHECK("            edcb5433", "%*x", 20, 3989525555U);
    PRINT_CHECK("            1234ABCD", "%*X", 20, 305441741);
    PRINT_CHECK("            EDCB5433", "%*X", 20, 3989525555U);
    PRINT_CHECK("                   x", "%*c", 20, 'x');
}

DEFCASE(width_minus_20)
{
    PRINT_CHECK("Hello               ", "%-20s", "Hello");
    PRINT_CHECK("1024                ", "%-20d", 1024);
    PRINT_CHECK("-1024               ", "%-20d", -1024);
    PRINT_CHECK("1024                ", "%-20i", 1024);
    PRINT_CHECK("-1024               ", "%-20i", -1024);
    PRINT_CHECK("1024                ", "%-20u", 1024);
    PRINT_CHECK("1024.1234           ", "%-20.4f", 1024.1234);
    PRINT_CHECK("4294966272          ", "%-20u", 4294966272U);
    PRINT_CHECK("777                 ", "%-20o", 511);
    PRINT_CHECK("37777777001         ", "%-20o", 4294966785U);
    PRINT_CHECK("1234abcd            ", "%-20x", 305441741);
    PRINT_CHECK("edcb5433            ", "%-20x", 3989525555U);
    PRINT_CHECK("1234ABCD            ", "%-20X", 305441741);
    PRINT_CHECK("EDCB5433            ", "%-20X", 3989525555U);
    PRINT_CHECK("x                   ", "%-20c", 'x');
    PRINT_CHECK("|    9| |9 | |    9|", "|%5d| |%-2d| |%5d|", 9, 9, 9);
    PRINT_CHECK("|   10| |10| |   10|", "|%5d| |%-2d| |%5d|", 10, 10, 10);
    PRINT_CHECK("|    9| |9           | |    9|", "|%5d| |%-12d| |%5d|", 9, 9, 9);
    PRINT_CHECK("|   10| |10          | |   10|", "|%5d| |%-12d| |%5d|", 10, 10, 10);
}

DEFCASE(padding_20)
{
    PRINT_CHECK("00000000000000001024", "%020d", 1024);
    PRINT_CHECK("-0000000000000001024", "%020d", -1024);
    PRINT_CHECK("00000000000000001024", "%020i", 1024);
    PRINT_CHECK("-0000000000000001024", "%020i", -1024);
    PRINT_CHECK("00000000000000001024", "%020u", 1024);
    PRINT_CHECK("00000000004294966272", "%020u", 4294966272U);
    PRINT_CHECK("00000000000000000777", "%020o", 511);
    PRINT_CHECK("00000000037777777001", "%020o", 4294966785U);
    PRINT_CHECK("0000000000001234abcd", "%020x", 305441741);
    PRINT_CHECK("000000000000edcb5433", "%020x", 3989525555U);
    PRINT_CHECK("0000000000001234ABCD", "%020X", 305441741);
    PRINT_CHECK("000000000000EDCB5433", "%020X", 3989525555U);
}

DEFCASE(padding_dot_20)
{
    PRINT_CHECK("00000000000000001024", "%.20d", 1024);
    PRINT_CHECK("-00000000000000001024", "%.20d", -1024);
    PRINT_CHECK("00000000000000001024", "%.20i", 1024);
    PRINT_CHECK("-00000000000000001024", "%.20i", -1024);
    PRINT_CHECK("00000000000000001024", "%.20u", 1024);
    PRINT_CHECK("00000000004294966272", "%.20u", 4294966272U);
    PRINT_CHECK("00000000000000000777", "%.20o", 511);
    PRINT_CHECK("00000000037777777001", "%.20o", 4294966785U);
    PRINT_CHECK("0000000000001234abcd", "%.20x", 305441741);
    PRINT_CHECK("000000000000edcb5433", "%.20x", 3989525555U);
    PRINT_CHECK("0000000000001234ABCD", "%.20X", 305441741);
    PRINT_CHECK("000000000000EDCB5433", "%.20X", 3989525555U);
}

DEFCASE(padding_sharp_020)
{
    PRINT_CHECK("00000000000000000777", "%#020o", 511);
    PRINT_CHECK("00000000037777777001", "%#020o", 4294966785U);
    PRINT_CHECK("0x00000000001234abcd", "%#020x", 305441741);
    PRINT_CHECK("0x0000000000edcb5433", "%#020x", 3989525555U);
    PRINT_CHECK("0X00000000001234ABCD", "%#020X", 305441741);
    PRINT_CHECK("0X0000000000EDCB5433", "%#020X", 3989525555U);
}

DEFCASE(padding_sharp_20)
{
    PRINT_CHECK("                0777", "%#20o", 511);
    PRINT_CHECK("        037777777001", "%#20o", 4294966785U);
    PRINT_CHECK("          0x1234abcd", "%#20x", 305441741);
    PRINT_CHECK("          0xedcb5433", "%#20x", 3989525555U);
    PRINT_CHECK("          0X1234ABCD", "%#20X", 305441741);
    PRINT_CHECK("          0XEDCB5433", "%#20X", 3989525555U);
}

DEFCASE(padding_20_point_5)
{
    PRINT_CHECK("               01024", "%20.5d", 1024);
    PRINT_CHECK("              -01024", "%20.5d", -1024);
    PRINT_CHECK("               01024", "%20.5i", 1024);
    PRINT_CHECK("              -01024", "%20.5i", -1024);
    PRINT_CHECK("               01024", "%20.5u", 1024);
    PRINT_CHECK("          4294966272", "%20.5u", 4294966272U);
    PRINT_CHECK("               00777", "%20.5o", 511);
    PRINT_CHECK("         37777777001", "%20.5o", 4294966785U);
    PRINT_CHECK("            1234abcd", "%20.5x", 305441741);
    PRINT_CHECK("          00edcb5433", "%20.10x", 3989525555U);
    PRINT_CHECK("            1234ABCD", "%20.5X", 305441741);
    PRINT_CHECK("          00EDCB5433", "%20.10X", 3989525555U);
}

DEFCASE(padding_negative_numbers)
{
    // space padding
    PRINT_CHECK("-5", "% 1d", -5);
    PRINT_CHECK("-5", "% 2d", -5);
    PRINT_CHECK(" -5", "% 3d", -5);
    PRINT_CHECK("  -5", "% 4d", -5);

    // zero padding
    PRINT_CHECK("-5", "%01d", -5);
    PRINT_CHECK("-5", "%02d", -5);
    PRINT_CHECK("-05", "%03d", -5);
    PRINT_CHECK("-005", "%04d", -5);
}

DEFCASE(float_padding_negative_numbers)
{
    // space padding
    PRINT_CHECK("-5.0", "% 3.1f", -5.);
    PRINT_CHECK("-5.0", "% 4.1f", -5.);
    PRINT_CHECK(" -5.0", "% 5.1f", -5.);

    PRINT_CHECK("    -5", "% 6.1g", -5.);
    PRINT_CHECK("-5.0e+00", "% 6.1e", -5.);
    PRINT_CHECK("  -5.0e+00", "% 10.1e", -5.);

    // zero padding
    PRINT_CHECK("-5.0", "%03.1f", -5.);
    PRINT_CHECK("-5.0", "%04.1f", -5.);
    PRINT_CHECK("-05.0", "%05.1f", -5.);

    // zero padding no decimal point
    PRINT_CHECK("-5", "%01.0f", -5.);
    PRINT_CHECK("-5", "%02.0f", -5.);
    PRINT_CHECK("-05", "%03.0f", -5.);

    PRINT_CHECK("-005.0e+00", "%010.1e", -5.);
    PRINT_CHECK("-05E+00", "%07.0E", -5.);
    PRINT_CHECK("-05", "%03.0g", -5.);
}

DEFCASE(length)
{
    PRINT_CHECK("", "%.0s", "Hello World");
    PRINT_CHECK("                    ", "%20.0s", "Hello World");
    PRINT_CHECK("", "%.s", "Hello World");
    PRINT_CHECK("                    ", "%20.s", "Hello World");
    PRINT_CHECK("                1024", "%20.0d", 1024);
    PRINT_CHECK("               -1024", "%20.0d", -1024);
    PRINT_CHECK("                    ", "%20.d", 0);
    PRINT_CHECK("                1024", "%20.0i", 1024);
    PRINT_CHECK("               -1024", "%20.i", -1024);
    PRINT_CHECK("                    ", "%20.i", 0);
    PRINT_CHECK("                1024", "%20.u", 1024);
    PRINT_CHECK("          4294966272", "%20.0u", 4294966272U);
    PRINT_CHECK("                    ", "%20.u", 0U);
    PRINT_CHECK("                 777", "%20.o", 511);
    PRINT_CHECK("         37777777001", "%20.0o", 4294966785U);
    PRINT_CHECK("                    ", "%20.o", 0U);
    PRINT_CHECK("            1234abcd", "%20.x", 305441741);
    PRINT_CHECK("                                          1234abcd", "%50.x", 305441741);
    PRINT_CHECK("                                          1234abcd     12345", "%50.x%10.u", 305441741, 12345);
    PRINT_CHECK("            edcb5433", "%20.0x", 3989525555U);
    PRINT_CHECK("                    ", "%20.x", 0U);
    PRINT_CHECK("            1234ABCD", "%20.X", 305441741);
    PRINT_CHECK("            EDCB5433", "%20.0X", 3989525555U);
    PRINT_CHECK("                    ", "%20.X", 0U);
}

DEFCASE(infinity_and_not_a_number_values)
{
    // test special-case floats using math.h macros
    PRINT_CHECK("     nan", "%8f", (double)NAN);
    PRINT_CHECK("     inf", "%8f", (double)INFINITY);
    PRINT_CHECK("-inf    ", "%-8f", (double)-INFINITY);
    PRINT_CHECK("     nan", "%8e", (double)NAN);
    PRINT_CHECK("     inf", "%8e", (double)INFINITY);
    PRINT_CHECK("-inf    ", "%-8e", (double)-INFINITY);
}

DEFCASE(floating_point_specifiers_with_31_to_32_bit_integer_values)
{
    PRINT_CHECK("2147483647", "%.10f", 2147483647.0); // 2^31 - 1
    PRINT_CHECK("2147483648", "%.10f", 2147483648.0); // 2^31
    PRINT_CHECK("4294967295", "%.10f", 4294967295.0); // 2^32 - 1
    PRINT_CHECK("4294967296", "%.10f", 4294967296.0); // 2^32

    // PRINT_CHECK("2.1474836470e+09", "%.10f", 2147483647.0); // 2^31 - 1
    // PRINT_CHECK("2.1474836480e+09", "%.10f", 2147483648.0); // 2^31
    // PRINT_CHECK("4.2949672950e+09", "%.10f", 4294967295.0); // 2^32 - 1
    // PRINT_CHECK("4.2949672960e+09", "%.10f", 4294967296.0); // 2^32

    PRINT_CHECK("2147483647", "%.10g", 2147483647.0); // 2^31 - 1
    PRINT_CHECK("2147483648", "%.10g", 2147483648.0); // 2^31
    PRINT_CHECK("4294967295", "%.10g", 4294967295.0); // 2^32 - 1
    PRINT_CHECK("4294967296", "%.10g", 4294967296.0); // 2^32
}

DEFCASE(tiny_floating_point_values)
{
    PRINT_CHECK("1e-23", "%.0e", 1.380651569e-23);
    PRINT_CHECK("1.4e-23", "%.1e", 1.380651569e-23);
    PRINT_CHECK("1.38e-23", "%.2e", 1.380651569e-23);
    PRINT_CHECK("1.381e-23", "%.3e", 1.380651569e-23);
    PRINT_CHECK("1.3807e-23", "%.4e", 1.380651569e-23);
    PRINT_CHECK("1.38065e-23", "%.5e", 1.380651569e-23);
    PRINT_CHECK("1.380652e-23", "%.6e", 1.380651569e-23);
    PRINT_CHECK("1.3806516e-23", "%.7e", 1.380651569e-23);
    PRINT_CHECK("1.38065157e-23", "%.8e", 1.380651569e-23);
    PRINT_CHECK("1.380651569e-23", "%.9e", 1.380651569e-23);
    PRINT_CHECK("1.3806515690e-23", "%.10e", 1.380651569e-23);
    PRINT_CHECK("1.38065156900e-23", "%.11e", 1.380651569e-23);
    // These go beyond our precision abilities; enable them, and they will likely fail.
    PRINT_CHECK("1.380651569000e-23", "%.12e", 1.380651569e-23);
    PRINT_CHECK("1.3806515690000e-23", "%.13e", 1.380651569e-23);
    PRINT_CHECK("1.38065156900000e-23", "%.14e", 1.380651569e-23);
    PRINT_CHECK("1.380651569000000e-23", "%.15e", 1.380651569e-23);
    PRINT_CHECK("1.3806515690000000e-23", "%.16e", 1.380651569e-23);
}

DEFCASE(floating_point_specifiers_precision_and_flags)
{
    PRINT_CHECK("3.1415", "%.4f", 3.1415354);
    PRINT_CHECK("30343.142", "%.3f", 30343.1415354);
    PRINT_CHECK("34", "%.0f", 34.1415354);
    PRINT_CHECK("1", "%.0f", 1.3);
    PRINT_CHECK("2", "%.0f", 1.55);
    PRINT_CHECK("1.6", "%.1f", 1.64);
    PRINT_CHECK("42.90", "%.2f", 42.8952);
    PRINT_CHECK("42.895200000", "%.9f", 42.8952);
    PRINT_CHECK("42.8952230000", "%.10f", 42.895223);
    PRINT_CHECK("42.895223123457", "%.12f", 42.89522312345678);
    PRINT_CHECK("42477.371093750000000", "%020.15f", 42477.37109375);
    PRINT_CHECK("42.895223876543", "%.12f", 42.89522387654321);
    PRINT_CHECK(" 42.90", "%6.2f", 42.8952);
    PRINT_CHECK("+42.90", "%+6.2f", 42.8952);
    PRINT_CHECK("+42.9", "%+5.1f", 42.9252);
    PRINT_CHECK("42.500000", "%f", 42.5);
    PRINT_CHECK("42.5", "%.1f", 42.5);
    PRINT_CHECK("42167.000000", "%f", 42167.0);
    PRINT_CHECK("-12345.987654321", "%.9f", -12345.987654321);
    PRINT_CHECK("4.0", "%.1f", 3.999);
    PRINT_CHECK("4", "%.0f", 3.5);
    PRINT_CHECK("4", "%.0f", 4.5);
    PRINT_CHECK("3", "%.0f", 3.49);
    PRINT_CHECK("3.5", "%.1f", 3.49);
    PRINT_CHECK("a0.5  ", "a%-5.1f", 0.5);
    PRINT_CHECK("a0.5  end", "a%-5.1fend", 0.5);

    PRINT_CHECK("0.5", "%.4g", 0.5);
    PRINT_CHECK("1", "%.4g", 1.0);
    PRINT_CHECK("12345.7", "%G", 12345.678);
    PRINT_CHECK("12345.68", "%.7G", 12345.678);
    PRINT_CHECK("1.2346E+08", "%.5G", 123456789.);
    PRINT_CHECK("12345", "%.6G", 12345.);
    PRINT_CHECK("  +1.235e+08", "%+12.4g", 123456789.);
    PRINT_CHECK("0.0012", "%.2G", 0.001234);
    PRINT_CHECK(" +0.001234", "%+10.4G", 0.001234);
    PRINT_CHECK("+001.234e-05", "%+012.4g", 0.00001234);
    // Note: The following two values are _barely_ normal; make their mantissa 1.1 and they lose their normality.
    PRINT_CHECK("-1.23e-308", "%.3g", -1.2345e-308);
    PRINT_CHECK("+1.230E+308", "%+.3E", 1.23e+308);
    PRINT_CHECK("1.000e+01", "%.3e", 9.9996);
    PRINT_CHECK("0", "%g", 0.);
    PRINT_CHECK("-0", "%g", -0.);
    PRINT_CHECK("+0", "%+g", 0.);
    PRINT_CHECK("-0", "%+g", -0.);
    PRINT_CHECK("-4e+04", "%.1g", -40661.5);
    PRINT_CHECK("-4.e+04", "%#.1g", -40661.5);
    PRINT_CHECK("100.", "%#.3g", 99.998580932617187500);
    // Note: The following value is _barely_ normal; make the mantissa 1.1 and it loses its normality.
    PRINT_CHECK("1.2345678901e-308", "%.10e", 1.2345678901e-308);
    // Rounding-focused checks
    PRINT_CHECK("4.895512e+04", "%e", 48955.125);
    PRINT_CHECK("9.2524e+04", "%.4e", 92523.5);
    PRINT_CHECK("-8.380923438e+04", "%.9e", -83809.234375);
}

DEFCASE(integer_types)
{
    PRINT_CHECK("0", "%i", 0);
    PRINT_CHECK("1234", "%i", 1234);
    PRINT_CHECK("32767", "%i", 32767);
    PRINT_CHECK("-32767", "%i", -32767);
    PRINT_CHECK("30", "%li", 30L);
    PRINT_CHECK("-2147483647", "%li", -2147483647L);
    PRINT_CHECK("2147483647", "%li", 2147483647L);

    PRINT_CHECK("30", "%lli", 30LL);
    PRINT_CHECK("-9223372036854775807", "%lli", -9223372036854775807LL);
    PRINT_CHECK("9223372036854775807", "%lli", 9223372036854775807LL);

    PRINT_CHECK("100000", "%lu", 100000L);
    PRINT_CHECK("4294967295", "%lu", 0xFFFFFFFFL);

    PRINT_CHECK("281474976710656", "%llu", 281474976710656LLU);
    PRINT_CHECK("18446744073709551615", "%llu", 18446744073709551615LLU);

    PRINT_CHECK("2147483647", "%zu", (size_t)2147483647UL);
    PRINT_CHECK("2147483647", "%zd", (size_t)2147483647UL);
    PRINT_CHECK("-2147483647", "%zi", (ssize_t)-2147483647L);
    PRINT_CHECK("165140", "%o", 60000);
    PRINT_CHECK("57060516", "%lo", 12345678L);
    PRINT_CHECK("12345678", "%lx", 0x12345678L);

    PRINT_CHECK("1234567891234567", "%llx", 0x1234567891234567LLU);

    PRINT_CHECK("abcdefab", "%lx", 0xabcdefabL);
    PRINT_CHECK("ABCDEFAB", "%lX", 0xabcdefabL);
    PRINT_CHECK("v", "%c", 'v');
    PRINT_CHECK("wv", "%cv", 'w');
    PRINT_CHECK("A Test", "%s", "A Test");
    PRINT_CHECK("255", "%hhu", (unsigned char)0xFFU);
    PRINT_CHECK("4660", "%hu", (unsigned short)0x1234u);
    PRINT_CHECK("Test100 65535", "%s%hhi %hu", "Test", (char)100, (unsigned short)0xFFFF);
    PRINT_CHECK("a", "%tx", 10 - 0);
    PRINT_CHECK("-2147483647", "%ji", (intmax_t)-2147483647L);
}

DEFCASE(pointer)
{
    if (sizeof(void *) == 4U) {
        PRINT_CHECK("0x00001234", "%p", (void *)0x1234U);
        PRINT_CHECK("0x12345678", "%p", (void *)0x12345678U);
        PRINT_CHECK("0x12345678-0x7edcba98", "%p-%p", (void *)0x12345678U, (void *)0x7EDCBA98U);
    } else {
        PRINT_CHECK("0x0000000000001234", "%p", (void *)0x1234U);
        PRINT_CHECK("0x0000000012345678", "%p", (void *)0x12345678U);
        PRINT_CHECK("0x0000000012345678-0x000000007edcba98", "%p-%p", (void *)0x12345678U, (void *)0x7EDCBA98U);
    }

    if (sizeof(uintptr_t) == sizeof(uint64_t)) {
        PRINT_CHECK("0x00000000ffffffff", "%p", (void *)(uintptr_t)0xFFFFFFFFU);
    } else {
        PRINT_CHECK("0xffffffff", "%p", (void *)(uintptr_t)0xFFFFFFFFU);
    }

    PRINT_CHECK("(nil)", "%p", (const void *)NULL);
}

DEFCASE(string_length)
{
    PRINT_CHECK("This", "%.4s", "This is a test");
    PRINT_CHECK("test", "%.4s", "test");
    PRINT_CHECK("123", "%.7s", "123");
    PRINT_CHECK("", "%.7s", "");
    PRINT_CHECK("1234ab", "%.4s%.2s", "123456", "abcdef");
    PRINT_CHECK("123", "%.*s", 3, "123456");
    PRINT_CHECK("(null)", "%.*s", 3, (const char *)NULL);
}

DEFCASE(misc)
{
    PRINT_CHECK("53000atest-20 bit", "%u%u%ctest%d %s", 5, 3000, 'a', -20, "bit");

    PRINT_CHECK("0.33", "%.*f", 2, 0.33333333);
    PRINT_CHECK("1", "%.*d", -1, 1);
    PRINT_CHECK("foo", "%.3s", "foobar");
    PRINT_CHECK(" ", "% .0d", 0);
    PRINT_CHECK("     00004", "%10.5d", 4);
    PRINT_CHECK("hi x", "%*sx", -3, "hi");
    PRINT_CHECK("00123               ", "%-20.5i", 123);
    PRINT_CHECK("-67224.546875000000000000", "%.18f", -67224.546875);

    PRINT_CHECK("0.33", "%.*g", 2, 0.33333333);
    PRINT_CHECK("3.33e-01", "%.*e", 2, 0.33333333);
    PRINT_CHECK("0.000000e+00", "%e", 0.0);
    PRINT_CHECK("-0.000000e+00", "%e", -0.0);
}

DEFCASE(long_long_4)
{
    PRINT_CHECK("30 30 30 30", "%lli %lli %lli %lli", 30LL, 30LL, 30LL, 30LL);
    PRINT_CHECK("30 30 30 30", "%lld %lld %lld %lld", 30LL, 30LL, 30LL, 30LL);
    PRINT_CHECK("-9223372036854775807 -9223372036854775807 -9223372036854775807 -9223372036854775807", "%lld %lld %lld %lld", -9223372036854775807LL, -9223372036854775807LL, -9223372036854775807LL, -9223372036854775807LL);
    PRINT_CHECK("9223372036854775807 9223372036854775807 9223372036854775807 9223372036854775807", "%lld %lld %lld %lld", 9223372036854775807LL, 9223372036854775807LL, 9223372036854775807LL, 9223372036854775807LL);
    PRINT_CHECK("-9223372036854775807 -9223372036854775807 -9223372036854775807 -9223372036854775807", "%lli %lli %lli %lli", -9223372036854775807LL, -9223372036854775807LL, -9223372036854775807LL, -9223372036854775807LL);
    PRINT_CHECK("9223372036854775807 9223372036854775807 9223372036854775807 9223372036854775807", "%lli %lli %lli %lli", 9223372036854775807LL, 9223372036854775807LL, 9223372036854775807LL, 9223372036854775807LL);
    PRINT_CHECK("281474976710656 281474976710656 281474976710656 281474976710656", "%llu %llu %llu %llu", 281474976710656LLU, 281474976710656LLU, 281474976710656LLU, 281474976710656LLU);
    PRINT_CHECK("18446744073709551615 18446744073709551615 18446744073709551615 18446744073709551615", "%llu %llu %llu %llu", 18446744073709551615LLU, 18446744073709551615LLU, 18446744073709551615LLU, 18446744073709551615LLU);
    PRINT_CHECK("1234567891234567 1234567891234567 1234567891234567 1234567891234567", "%llx %llx %llx %llx", 0x1234567891234567LLU, 0x1234567891234567LLU, 0x1234567891234567LLU, 0x1234567891234567LLU);
}

DEFCASE(buffer_long_long_4)
{
    long long buffer[4] = { 30LL, 30LL, 30LL, 30LL };
    long long lbuffer[4] = { 9223372036854775807LL, 9223372036854775807LL, 9223372036854775807LL, 9223372036854775807LL };
    unsigned long long ubuffer[4] = { 281474976710656LLU, 281474976710656LLU, 281474976710656LLU, 281474976710656LLU };
    unsigned long long u2buffer[4] = { 18446744073709551615LLU, 18446744073709551615LLU, 18446744073709551615LLU, 18446744073709551615LLU };
    unsigned long long xbuffer[4] = { 0x1234567891234567LLU, 0x1234567891234567LLU, 0x1234567891234567LLU, 0x1234567891234567LLU };

    PRINT_CHECK("30 30 30 30", "%lli %lli %lli %lli", buffer[0], buffer[1], buffer[2], buffer[3]);
    PRINT_CHECK("30 30 30 30", "%lld %lld %lld %lld", buffer[0], buffer[1], buffer[2], buffer[3]);
    PRINT_CHECK("-9223372036854775807 -9223372036854775807 -9223372036854775807 -9223372036854775807", "%lld %lld %lld %lld", -lbuffer[0], -lbuffer[1], -lbuffer[2], -lbuffer[3]);
    PRINT_CHECK("9223372036854775807 9223372036854775807 9223372036854775807 9223372036854775807", "%lld %lld %lld %lld", lbuffer[0], lbuffer[1], lbuffer[2], lbuffer[3]);
    PRINT_CHECK("-9223372036854775807 -9223372036854775807 -9223372036854775807 -9223372036854775807", "%lli %lli %lli %lli", -lbuffer[0], -lbuffer[1], -lbuffer[2], -lbuffer[3]);
    PRINT_CHECK("9223372036854775807 9223372036854775807 9223372036854775807 9223372036854775807", "%lli %lli %lli %lli", lbuffer[1], lbuffer[2], lbuffer[3]);
    PRINT_CHECK("281474976710656 281474976710656 281474976710656 281474976710656", "%llu %llu %llu %llu", ubuffer[0], ubuffer[1], ubuffer[2], ubuffer[3]);
    PRINT_CHECK("18446744073709551615 18446744073709551615 18446744073709551615 18446744073709551615", "%llu %llu %llu %llu", u2buffer[0], u2buffer[1], u2buffer[2], u2buffer[3]);
    PRINT_CHECK("1234567891234567 1234567891234567 1234567891234567 1234567891234567", "%llx %llx %llx %llx", xbuffer[0], xbuffer[1], xbuffer[2], xbuffer[3]);
}

static testfunc_t tests[] = {
    test_space_flag,
    test_plus_flag,
    test_zero_flag,
    test_minus_flag,
    test_sharp_flag,
    test_sharp_flag_with_long_long,
    test_specifier,
    test_width,
    test_width_20,
    test_width_asterisk_20,
    test_width_minus_20,
    test_padding_20,
    test_padding_dot_20,
    test_padding_sharp_020,
    test_padding_sharp_20,
    test_padding_20_point_5,
    test_padding_negative_numbers,
    test_float_padding_negative_numbers,
    test_length,
    test_infinity_and_not_a_number_values,
    test_floating_point_specifiers_with_31_to_32_bit_integer_values,
    test_tiny_floating_point_values,
    test_floating_point_specifiers_precision_and_flags,
    test_integer_types,
    test_pointer,
    test_string_length,
    test_misc,
    test_long_long_4,
    test_buffer_long_long_4
};

int main(void)
{
    board_init();

    for (int i = 0; i < sizeof(tests) / sizeof(testfunc_t); i++) {
        printf("\r\ntest %d begin >>>>>>>>>>>\r\n", i);
        tests[i]();
        printf("test %d end >>>>>>>>>>>>>\r\n", i);
    }

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
