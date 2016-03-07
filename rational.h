
#ifndef __RATIONAL_H__
#define __RATIONAL_H__

#include <assert.h>
#define _vd_assert(EXPR) assert(EXPR)


class Math
{
public:
    template<typename int_type>
    static int_type gcd(int_type a, int_type b)
    {
        int_type const ZERO = static_cast<int_type>(0);
        while (true) {
            if (a == ZERO)
                return b;
            b %= a;

            if (b == ZERO)
                return a;
            a %= b;
        }
    }
};


class rational
{
public:
    typedef long long int_type;
    enum : int_type {
        ZERO,
        ONE,
    };

    rational(): m_numer(ZERO), m_denom(ONE) {}
    rational(int_type a_numer): m_numer(a_numer), m_denom(ONE) {}
    rational(int_type a_numer, int_type a_denom)
        : m_numer(a_numer), m_denom(a_denom) { normalize(); }

    rational& operator = (int_type a_numer) {
        m_numer = a_numer;
        m_denom = ONE;
    }

    void assign(int_type a_numer, int_type a_denom) {
        m_numer = a_numer;
        m_denom = a_denom;
        normalize();
    }

    void normalize() {
        _vd_assert(m_denom != ZERO);
        if (m_numer == ZERO) {
            m_denom = ONE;
            return;
        }
        int_type gcd = Math::gcd(m_numer, m_denom);
        m_numer /= gcd;
        m_denom /= gcd;

        if (m_denom < ZERO) {
            m_numer = -m_numer;
            m_denom = -m_denom;
        }
    }

    int_type numer() const { return m_numer; }
    int_type denom() const { return m_denom; }

    // Unary plus
    rational operator + () const {
        return *this;
    }

    // Unary minus
    rational operator - () const {
        return rational(-m_numer, m_denom);
    }

    rational& operator += (const rational& r) {
        // This calculation avoids overflow, and minimises the number of expensive
        // calculations. Thanks to Nickolay Mladenov for this algorithm.
        //
        // Proof:
        // We have to compute a/b + c/d, where gcd(a,b)=1 and gcd(b,c)=1.
        // Let g = gcd(b,d), and b = b1*g, d=d1*g. Then gcd(b1,d1)=1
        //
        // The result is (a*d1 + c*b1) / (b1*d1*g).
        // Now we have to normalize this ratio.
        // Let's assume h | gcd((a*d1 + c*b1), (b1*d1*g)), and h > 1
        // If h | b1 then gcd(h,d1)=1 and hence h|(a*d1+c*b1) => h|a.
        // But since gcd(a,b1)=1 we have h=1.
        // Similarly h|d1 leads to h=1.
        // So we have that h | gcd((a*d1 + c*b1) , (b1*d1*g)) => h|g
        // Finally we have gcd((a*d1 + c*b1), (b1*d1*g)) = gcd((a*d1 + c*b1), g)
        // Which proves that instead of normalizing the result, it is better to
        // divide num and den by gcd((a*d1 + c*b1), g)

        // Protect against self-modification
        int_type r_numer = r.m_numer;
        int_type r_denom = r.m_denom;

        int_type g = Math::gcd(m_denom, r_denom);
        m_denom /= g;  // = b1 from the calculations above
        m_numer = m_numer * (r_denom / g) + r_numer * m_denom;
        g = Math::gcd(m_numer, g);
        m_numer /= g;
        m_denom *= r_denom/g;

        return *this;
    }

    rational& operator -= (const rational& r);
    rational& operator *= (const rational& r);
    rational& operator /= (const rational& r);

    rational& operator += (int_type i);
    rational& operator -= (int_type i);
    rational& operator *= (int_type i);
    rational& operator /= (int_type i);

    // Comparison operators
    bool operator < (const rational& r) const;
    bool operator == (const rational& r) const;

    bool operator < (int_type i) const;
    bool operator > (int_type i) const;
    bool operator == (int_type i) const;

private:
    int_type m_numer;
    int_type m_denom;
};


#include <iostream>

std::ostream& operator << (std::ostream& output, const rational& r) {
    output << r.numer();
    if (r.denom() > 1)
        output << '/' << r.denom();
    return output;
}

#endif //__RATIONAL_H__
