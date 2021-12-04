// The following integer conversions should be performed by the AwVector constructors.

template <class vec_t>
AwVector<int8_t> toInt8(const vec_t& in)
{
    AwVector<int8_t> out;
    for( size_t i=0; i < in.size(); i++ ) {
        out.push_back(in[i]);
    }
    return out;
}

template <class vec_t>
AwVector<int16_t> toInt16(const vec_t& in)
{
    AwVector<int16_t> out;
    for( size_t i=0; i < in.size(); i++ ) {
        out.push_back(in[i]);
    }
    return out;
}

template <class vec_t>
AwVector<int32_t> toInt32(const vec_t& in)
{
    AwVector<int32_t> out;
    for( size_t i=0; i < in.size(); i++ ) {
        out.push_back(in[i]);
    }
    return out;
}

template <typename sample_t>
AwVector<int32_t> int3(const AwVector<sample_t>& in, size_t rate)
{
    auto y= toInt32(linspace(0,1,in.size()*rate));

    const size_t len = in.size();
    const size_t block_size = 5;

    CICInt3<sample_t> cic(rate);

    size_t i;
    for(i=0; i<=len-block_size; i+= block_size)
    {
        cic((sample_t*)&in[i], &y[i*rate], block_size);
    }
    cic((sample_t*)&in[i], &y[i*rate], len-i);
    
    return y;
}

template <class vec_t>
AwVector<int32_t> intv(const vec_t& in, size_t rate, size_t order)
{
    // Convert the input and zero-initialise the output vector.
    AwVector<int32_t> x,y;
    for( size_t i=0; i < in.size(); i++ ) {
        x.push_back(in[i]);
        for( size_t j=0; j<rate; j++) y.push_back(0);
    }
    up(&x[0], &y[0], x.size(), rate, order);
    
    return y;
}
