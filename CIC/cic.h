/*
    CIC - Cascaded Integrator Comb interpolators and decimators.
    Copyright (C) 2021 Andrew Rogers

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <cstddef>

template <typename sample_t>
void interpolate(const sample_t* in, sample_t* out, size_t len, size_t rate)
{
    for( size_t i=0; i < len; i++ )
    {
        out[i*rate]=in[i];
        for( size_t j=1; j < rate; j++) out[i*rate+j]=0;
    }
}

template <typename sample_t>
void decimate(const sample_t& in, sample_t* out, size_t len, size_t rate)
{
    for( size_t i=0; i < len; i++ )
    {
        out[i]=in[i*rate];
    }
}

template <typename sample_t>
void diff(sample_t* v, size_t len)
{
    int32_t prev=0;
    int32_t x;
    for( size_t i=0; i < len; i++ )
    {
        x = v[i];
        v[i] = x - prev;
        prev = x;
    }
}

template <typename sample_t>
void integrate(sample_t* v, size_t len)
{
    int32_t sum=0;
    for( size_t i=0; i < len; i++ )
    {
        sum += v[i];
        v[i] = sum;
    }
}

template <typename sample_t>
void up(const sample_t* in, sample_t* out, size_t len_in, size_t rate, size_t order)
{
    size_t len_out = len_in * rate;
    sample_t x[len_in];
    for(size_t i=0; i<len_in; i++)
    {
        x[i] = in[i];
    }
    for( size_t i=0; i<order; i++) diff( x, len_in );
    interpolate(x, out, len_in, rate);
    for( size_t i=0; i<order; i++) integrate( out, len_out );
}

template <typename sample_t>
void down(const sample_t* in, sample_t* out, size_t len_out, size_t rate, size_t order)
{
    size_t len_in = len_out * rate;
    sample_t x[len_in];
    for(size_t i=0; i<len_in; i++)
    {
        x[i] = in[i];
    }
    for( size_t i=0; i<order; i++) integrate( x, len_in );
    decimate(x, out, len_out, rate);
    for( size_t i=0; i<order; i++) diff( out, len_out );
}

template <typename Tin>
class CICInt3
{
public:
    CICInt3(size_t rate) : m_rate(rate),
    m_x0(0), m_x1(0),m_x2(0),
    m_y0(0), m_y1(0),m_y2(0),
    m_in_shift(16-sizeof(Tin)*8), m_s1(0), m_s2(0)
    {
        // integrator |                  output description                       | gain based on x(n)
        // -----------|-----------------------------------------------------------|--------------------
        //   first    | stepped with same amplitude as x(n)                       |  1
        //   second   | ramped with amplitude rate times higher than x(n)         | rate
        //   third    | rounded with amplitude rate * rate times higher than x(n) | rate * rate

        // The second integrator overflows when rate is greater than 32768
        if( m_rate > 32768 )
        {
            size_t growth = log2(m_rate-1) + 1;
            m_s1 = growth - 15;
        }

        // The third integrator overflows when rate is greater than 256 
        if( m_rate > 256 )
        {
            size_t growth = log2((int64_t)m_rate * m_rate - 1) + 1;
            m_s2 = growth - m_s1 - 16;
        }
    }

    int32_t gain()
    {
        return ((int64_t)m_rate * m_rate) << m_in_shift >> m_s1 >> m_s2;
    }

    void operator()(Tin* in, int32_t* out, size_t len)
    {
        samples(in, out, len);
    }

    void samples(const Tin* in, int32_t* out, size_t len)
    {
        int32_t x;
        int32_t sum;
        for( size_t i=0; i<len; i++)
        {
            // Comb stages.
            x = (int32_t)in[i] << m_in_shift;
            sum = x - m_x0;
            m_x0 = x;

            x = sum;
            sum = x - m_x1;
            m_x1 = x;

            x = sum;
            sum = x - m_x2;
            m_x2 = x;

            // Run the integrator stages at the higher rate.
            for( size_t j=0; j<m_rate; j++)
            {
                sum = sum + m_y0;
                m_y0 = sum;

                sum = (sum>>m_s1) + m_y1;
                m_y1 = sum;

                sum = (sum>>m_s2) + m_y2;
                m_y2 = sum;

                out[i*m_rate+j]=sum;
                sum=0;
            }
        }
    }

    static size_t log2(int64_t x)
    {
        size_t y=0;
        for( ; x>1 ; x=x>>1 ) y++;
        return y;
    }

private:
    size_t m_rate;
    int32_t m_x0, m_x1, m_x2;
    int32_t m_y0, m_y1, m_y2;
    size_t m_in_shift, m_s1, m_s2;
};


