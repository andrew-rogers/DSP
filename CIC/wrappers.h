template <typename sample_t>
AwVector<int32_t> int3(const AwVector<sample_t>& in, size_t rate)
{
    AwVector<int32_t> y(in.size()*rate);

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
    AwVector<int32_t> x(in);
    AwVector<int32_t> y(in.size()*rate);
    up(&x[0], &y[0], x.size(), rate, order);

    return y;
}

template <class vec_t>
AwVector<int32_t> decv(const vec_t& in, size_t rate, size_t order)
{
    AwVector<int32_t> x(in);
    AwVector<int32_t> y(in.size()/rate);
    down(&x[0], &y[0], y.size(), rate, order);

    return y;
}
