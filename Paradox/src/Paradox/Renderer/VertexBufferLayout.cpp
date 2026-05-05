#include "pxpch.h"
#include "VertexBufferLayout.h"

namespace Paradox
{
	void VertexBufferLayout::CalculateStrideAndOffset()
	{
		m_Stride = 0;
		uint32_t offset = 0;
		for (VertexBufferElement& element : m_Elements)
		{
			element.offset = offset;
			offset += element.size;
			m_Stride += element.size;
		}
	}
}
