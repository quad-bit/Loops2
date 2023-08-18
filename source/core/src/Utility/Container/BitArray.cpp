#include "Utility/Container/BitArray.h"
#include <assert.h>
#include <stdint.h>

Core::Containers::BitArray::BitArray(unsigned int size)
{
    assert(size > 0);
    totalBits = size;
    m_bits.reserve(size);
    totalBytes = BIT_TO_CHAR(size);
    for (uint32_t i = 0; i < size; i++)
        m_bits.push_back(0);
}


Core::Containers::BitArray::~BitArray()
{
}

void Core::Containers::BitArray::SetSize(const uint32_t & size)
{
    assert(size > 0);
    m_bits.clear();
    totalBits = size;
    m_bits.reserve(size);
    totalBytes = BIT_TO_CHAR(size);
    for (uint32_t i = 0; i < size; i++)
        m_bits.push_back(0);
}

void Core::Containers::BitArray::ClearAllBits()
{
    m_bits.assign(m_bits.size(), 0);
}

void Core::Containers::BitArray::SetAllBits()
{
    m_bits.assign(m_bits.size(), 0xff);
}

void Core::Containers::BitArray::SetByte(unsigned int byte, unsigned char val)
{
}

void Core::Containers::BitArray::SetBit(unsigned int bit)
{
    int index = bit / BIT_PER_BYTE;
    int offset = BIT_PER_BYTE - 1 - bit % BIT_PER_BYTE;

    m_bits[index] |= (1 << offset);
}

void Core::Containers::BitArray::ClearBit(unsigned int bit)
{
    assert(totalBits > bit);
    unsigned char mask = BIT_IN_CHAR(bit);
    mask = ~mask;

    m_bits[BIT_TO_CHAR(bit)] &= mask;
}

bool Core::Containers::BitArray::operator[](unsigned int bit) const
{
    assert(totalBits > bit);
    if ((m_bits[BIT_TO_CHAR(bit)] & BIT_IN_CHAR(bit)))
        return true;
    else
        return false;
}

bool Core::Containers::BitArray::operator==(const BitArray & obj) const
{
    ASSERT_MSG_DEBUG(this->totalBits != obj.totalBits, "Not comparable");
    for (unsigned int i = 0; i < totalBits; i++)
    {
        if (At(i) != obj.At(i))
            return false;
    }

    return true;
}

bool Core::Containers::BitArray::operator==(const Core::Containers::BitArray * obj) const
{
    ASSERT_MSG_DEBUG(this->totalBits != obj->totalBits, "Not comparable");
    for (unsigned int i = 0; i < totalBits; i++)
    {
        if (At(i) != obj->At(i))
            return false;
    }

    return true;
}

bool Core::Containers::BitArray::At(unsigned int bit) const
{
    assert(totalBits > bit);
    if ((m_bits[BIT_TO_CHAR(bit)] & BIT_IN_CHAR(bit)))
        return true;
    else
        return false;
}

bool operator<(const Core::Containers::BitArray & l, const Core::Containers::BitArray & r)
{
    bool result = true;
    for (uint32_t i = 0; i < l.totalBits; i++)
    {
        if (l[i] > r[i])
        {
            result = false;
            break;
        }
    }

    return result;
}
