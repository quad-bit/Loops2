#include "Utility/Container/BitArray.h"
#include <assert.h>
#include <stdint.h>

BitArray::BitArray(unsigned int size)
{
	assert(size > 0);
	totalBits = size;
	m_bits.reserve(size);
	totalBytes = BIT_TO_CHAR(size);
	for (uint32_t i = 0; i < size; i++)
		m_bits.push_back(0);
}


BitArray::~BitArray()
{
}

void BitArray::SetSize(const uint32_t & size)
{
    assert(size > 0);
    m_bits.clear();
    totalBits = size;
    m_bits.reserve(size);
    totalBytes = BIT_TO_CHAR(size);
    for (uint32_t i = 0; i < size; i++)
        m_bits.push_back(0);
}

void BitArray::ClearAllBits()
{
	m_bits.assign(m_bits.size(), 0);
}

void BitArray::SetAllBits()
{
	m_bits.assign(m_bits.size(), 0xff);
}

void BitArray::SetByte(unsigned int byte, unsigned char val)
{
}

void BitArray::SetBit(unsigned int bit)
{
	int index = bit / BIT_PER_BYTE;
	int offset = BIT_PER_BYTE - 1 - bit % BIT_PER_BYTE;

	m_bits[index] |= (1 << offset);
}

void BitArray::ClearBit(unsigned int bit)
{
	assert(totalBits > bit);
	unsigned char mask = BIT_IN_CHAR(bit);
	mask = ~mask;

	m_bits[BIT_TO_CHAR(bit)] &= mask;
}

bool BitArray::operator[](unsigned int bit) const
{
	assert(totalBits > bit);
    if ((m_bits[BIT_TO_CHAR(bit)] & BIT_IN_CHAR(bit)))
        return true;
    else
        return false;
}

bool BitArray::operator==(const BitArray & obj) const
{
    ASSERT_MSG_DEBUG(this->totalBits != obj.totalBits, "Not comparable");
    for (unsigned int i = 0; i < totalBits; i++)
    {
        if (At(i) != obj.At(i))
            return false;
    }

    return true;
}

bool BitArray::operator==(const BitArray * obj) const
{
    ASSERT_MSG_DEBUG(this->totalBits != obj->totalBits, "Not comparable");
    for (unsigned int i = 0; i < totalBits; i++)
    {
        if (At(i) != obj->At(i))
            return false;
    }

    return true;
}

bool BitArray::At(unsigned int bit) const
{
    assert(totalBits > bit);
    if ((m_bits[BIT_TO_CHAR(bit)] & BIT_IN_CHAR(bit)))
        return true;
    else
        return false;
}

bool operator<(const BitArray & l, const BitArray & r)
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
