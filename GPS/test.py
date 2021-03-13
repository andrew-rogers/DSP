#!/usr/bin/env python3

import ca_search
import file_reader

def test() :
    MHz = 1e6
    fs = 4 * MHz
    block_size = 4 * int(fs / 1000) # 4 millisecond blocks

    fr = file_reader.FileReader()
    ca = ca_search.CASearch( fs )

    for b in range(10) :
        bb_IQ = fr.read(block_size)
        ca.processBlock(bb_IQ)

test()
