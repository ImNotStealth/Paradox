#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	enum class VertexBufferDataType
	{
		Float = 0, Float2, Float3, Float4
	};

	struct VertexBufferElement
	{
		VertexBufferDataType type;
		uint32_t size = 0;
		uint32_t offset = 0;

		VertexBufferElement(VertexBufferDataType type)
			: type(type), size(GetSize(type)) {}

		static uint32_t GetSize(VertexBufferDataType type)
		{
			switch (type)
			{
				case VertexBufferDataType::Float: return 4;
				case VertexBufferDataType::Float2: return 4 * 2;
				case VertexBufferDataType::Float3: return 4 * 3;
				case VertexBufferDataType::Float4: return 4 * 4;
			}
			PX_CORE_ASSERT(false, "Invalid VertexBufferDataType.");
			return 0;
		}
	};

	class PARADOX_API VertexBufferLayout
	{
	public:
		VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateStrideAndOffset();
		}

		const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
		uint32_t GetStride() const { return m_Stride; }

	private:
		void CalculateStrideAndOffset();

	private:
		std::vector<VertexBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}