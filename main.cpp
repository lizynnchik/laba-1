#include <iostream>
#include <cstring>
#include <bitset>
#include <limits>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const int BIT_SET = 1;
const int BIT_CLEAR = 2;
const int BIT_TOGGLE = 3;

// --- размеры типов ---

void printTypeSizes() {
    cout << "bool        — " << sizeof(bool)        << " байт — логический тип" << endl;
    cout << "char        — " << sizeof(char)        << " байт — один символ" << endl;
    cout << "short int   — " << sizeof(short int)   << " байт — короткое целое число" << endl;
    cout << "int         — " << sizeof(int)         << " байт — обычное целое число" << endl;
    cout << "long int    — " << sizeof(long int)    << " байт — длинное целое" << endl;
    cout << "float       — " << sizeof(float)       << " байт — IEEE 754 binary32" << endl;
    cout << "double      — " << sizeof(double)      << " байт — IEEE 754 binary64" << endl;
    cout << "long double — " << sizeof(long double) << " байт — расширенная точность" << endl;
}

// --- печать битов ---

void printRawBits(unsigned long long bits, int width) {
    for (int i = width - 1; i >= 0; i--) {
        cout << ((bits >> i) & 1ULL);
    }
}

void printBitsWithSign(unsigned long long bits, int width) {
    for (int i = width - 1; i >= 0; i--) {
        cout << ((bits >> i) & 1ULL);
        if (i == width - 1) cout << "  ";
    }
}

void printIntBits(const int& value) {
    unsigned int bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    cout << "int " << value << " = ";
    printBitsWithSign(bits, 32);
    cout << "   (S | значащие разряды)" << endl;
}

void printShortBits(const short& value) {
    unsigned short bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    cout << "short " << value << " = ";
    printBitsWithSign(bits, 16);
    cout << "   (S | значащие разряды)" << endl;
}

void printUIntBits(const unsigned int& value) {
    cout << "unsigned " << value << " = ";
    printRawBits(value, 32);
    cout << "   (беззнаковый, знакового разряда нет)" << endl;
}


void printFloatBits(const float& value) {
    unsigned int bits = 0;
    memcpy(&bits, &value, sizeof(bits));

    unsigned sign = (bits >> 31) & 1u;
    unsigned exp  = (bits >> 23) & 0xFFu;
    unsigned man  = bits & 0x7FFFFFu;

    cout << "float " << value << endl;
    cout << "  Знак | Порядок | Мантисса" << endl;
    cout << "  ";
    unsigned mask = 1u << 31;
    for (int i = 0; i < 32; i++) {
        cout << ((bits & (mask >> i)) ? '1' : '0');
        if (i == 0 || i == 8) cout << " | ";
    }
    cout << endl;
    cout << "  знак=" << sign << "  порядок=" << exp << " (bias 127)  мантисса=" << man;

    if (exp == 0 && man == 0) cout << "  [ноль]";
    else if (exp == 0) cout << "  [денормализованное]";
    else if (exp == 255 && man == 0) cout << "  [inf]";
    else cout << "  [нормализованное, E=" << (int)exp - 127 << "]";
    cout << endl;
}

void printDoubleBits(const double& value) {
    unsigned long long bits = 0;
    memcpy(&bits, &value, sizeof(bits));

    unsigned sign = (unsigned)((bits >> 63) & 1ULL);
    unsigned exp  = (unsigned)((bits >> 52) & 0x7FFULL);
    unsigned long long man = bits & 0xFFFFFFFFFFFFFULL;

    cout << "double " << value << endl;
    cout << "  Знак | Порядок | Мантисса" << endl;
    cout << "  ";
    unsigned long long mask = 1ULL << 63;
    for (int i = 0; i < 64; i++) {
        cout << ((bits & (mask >> i)) ? '1' : '0');
        if (i == 0 || i == 11) cout << " | ";
    }
    cout << endl;
    cout << "  знак=" << sign << "  порядок=" << exp << " (bias 1023)  мантисса=" << man;

    if (exp == 0 && man == 0) cout << "  [ноль]";
    else if (exp == 0) cout << "  [денормализованное]";
    else if (exp == 2047 && man == 0) cout << "  [inf]";
    else if (exp == 2047) cout << "  [NaN]";
    else cout << "  [нормализованное, E=" << (int)exp - 1023 << "]";
    cout << endl;
}

// --- смена бита ---

unsigned int applyOp32(unsigned int bits, int pos, int op) {
    unsigned int mask = 1u << pos;
    if (op == BIT_SET)         bits |= mask;
    else if (op == BIT_CLEAR)  bits &= ~mask;
    else if (op == BIT_TOGGLE) bits ^= mask;
    return bits;
}

unsigned long long applyOp64(unsigned long long bits, int pos, int op) {
    unsigned long long mask = 1ULL << pos;
    if (op == BIT_SET)         bits |= mask;
    else if (op == BIT_CLEAR)  bits &= ~mask;
    else if (op == BIT_TOGGLE) bits ^= mask;
    return bits;
}

int changeBit(const int& value, int pos, int op) {
    unsigned int bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    bits = applyOp32(bits, pos, op);
    int result = 0;
    memcpy(&result, &bits, sizeof(result));
    return result;
}

unsigned int changeBit(const unsigned int& value, int pos, int op) {
    return applyOp32(value, pos, op);
}

float changeBit(const float& value, int pos, int op) {
    unsigned int bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    bits = applyOp32(bits, pos, op);
    float result = 0;
    memcpy(&result, &bits, sizeof(result));
    return result;
}

double changeBit(const double& value, int pos, int op) {
    unsigned long long bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    bits = applyOp64(bits, pos, op);
    double result = 0;
    memcpy(&result, &bits, sizeof(result));
    return result;
}

// --- ИДЗ ---

bool isOdd(const int& value) {
    return (value & 1) != 0;
}

int roundDownToEven(const int& value) {
    return value & ~(1 << 0);
}

void runIdz() {
    cout << "Проверка чётности и округление вниз до чётного." << endl;
    cout << "Идея: младший бит 1 — нечётное, 0 — чётное." << endl;
    cout << "Операция: n & ~(1 << 0) обнуляет младший бит." << endl << endl;

    const int tests[] = {15, 16, 0, 1, -1, -2, -3, -4};
    const int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        const int& x = tests[i];
        int y = roundDownToEven(x);
        cout << x << (isOdd(x) ? " нечётное" : " чётное")
             << "  " << bitset<32>((unsigned int)x)
             << "  ->  " << y
             << "  " << bitset<32>((unsigned int)y) << endl;
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    cout << "Этап 2. Размеры типов" << endl;
    printTypeSizes();
    cout << endl;



    cout << "=== Этап 3. Двоичное представление целых ===" << endl;
    printIntBits(42);
    printIntBits(-42);
    printIntBits(0);
    printShortBits((short)5);
    printShortBits((short)-5);
    printShortBits((short)0);
    printUIntBits(42u);
    printUIntBits(0u);
    printUIntBits(4294967295u);
    cout << endl;

    cout << "=== Этап 4. float (IEEE 754 binary32) ===" << endl;
    printFloatBits(3.14f);
    printFloatBits(-2.5f);
    printFloatBits(0.0f);
    printFloatBits(numeric_limits<float>::denorm_min());
    printFloatBits(numeric_limits<float>::infinity());
    printFloatBits(numeric_limits<float>::quiet_NaN());
    cout << endl;

    cout << "=== Этап 4. double (IEEE 754 binary64) ===" << endl;
    printDoubleBits(3.14);
    printDoubleBits(-2.5);
    printDoubleBits(0.0);
    printDoubleBits(numeric_limits<double>::denorm_min());
    printDoubleBits(numeric_limits<double>::infinity());
    cout << endl;

    cout << "=== Этап 5. Манипуляция битами ===" << endl;
    int x = 42;
    cout << "int 42, установить бит 0:  " << x << " -> " << changeBit(x, 0, BIT_SET) << endl;
    cout << "int 42, сбросить бит 1:    " << x << " -> " << changeBit(x, 1, BIT_CLEAR) << endl;
    cout << "int 42, инвертировать бит 1: " << x << " -> " << changeBit(x, 1, BIT_TOGGLE) << endl;

    unsigned int ux = 8u;
    cout << "unsigned 8, сбросить бит 3: " << ux << " -> " << changeBit(ux, 3, BIT_CLEAR) << endl;

    float f = 1.0f;
    float f2 = changeBit(f, 31, BIT_TOGGLE);
    cout << "float 1.0, инвертировать знаковый бит 31: " << f << " -> " << f2 << endl;
    cout << "до:  ";
    printFloatBits(f);
    cout << "после: ";
    printFloatBits(f2);

    double d = 3.14;
    double d2 = changeBit(d, 63, BIT_TOGGLE);
    cout << "double 3.14, инвертировать знаковый бит 63: " << d << " -> " << d2 << endl;
    cout << "до:  ";
    printDoubleBits(d);
    cout << "после: ";
    printDoubleBits(d2);
    cout << endl;

    cout << "=== Этап 6. Индивидуальное задание ===" << endl;
    runIdz();

    return 0;
}
