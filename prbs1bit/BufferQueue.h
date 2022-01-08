/*
    Buffer Queue Class
    Copyright (C) 2022  Andrew Rogers

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

#ifndef BUFFER_QUEUE_H
#define BUFFER_QUEUE_H

class BufferQueue
{
public:
    typedef char* ptr_t;

    BufferQueue( size_t bytes_per_buffer, size_t num_buffers )
    {
        m_top = num_buffers;
        m_rd = 0;
        m_wr = 0;
        m_buffers = new ptr_t[num_buffers];
        for( size_t i=0; i<num_buffers; i++) m_buffers[i]=(ptr_t) new char[bytes_per_buffer];
    }

    ~BufferQueue()
    {
        for( size_t i=0; i<m_top; i++) delete[] m_buffers[i];
        delete[] m_buffers;
    }

    ptr_t acquireWrite()
    {
        return m_buffers[m_wr];
    }

    bool commit()
    {
        size_t next = m_wr+1;
        if (next == m_top) next=0;
        if (next == m_rd) return false; // Full
        m_wr = next;
    }

    ptr_t acquireRead()
    {
        if (m_rd == m_wr) return NULL; // Empty
        return m_buffers[m_rd];
    }

    bool release()
    {
        if (m_rd == m_wr) return false; // Empty
        m_rd++;
        if (m_rd = m_top) m_rd=0;
    }

private:
    size_t m_top;
    size_t m_rd;
    size_t m_wr;
    ptr_t* m_buffers;
};

#endif // BUFFER_QUEUE_H

