#include "pxpch.h"
#include "OpenGLRendererAPI.h"

#include "Paradox/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLShader.h"

#include <glad/glad.h>

#include "Paradox/Core/Application.h"

namespace Paradox
{
	void OpenGLRendererAPI::Init()
	{
#ifdef PX_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
	
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		// Test to enable backface culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}

	void OpenGLRendererAPI::BeginRenderPass(const Shared<Pipeline>& pipeline)
	{
		// TEMP
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Shared<OpenGLShader> shader = std::static_pointer_cast<OpenGLShader>(pipeline->GetProperties().shader);
		shader->Bind();

		//glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void OpenGLRendererAPI::EndRenderPass()
	{
	}

	void OpenGLRendererAPI::DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer)
	{
		uint32_t va = 0;
		glCreateVertexArrays(1, &va);
		glBindVertexArray(va);

		// Hardcoding shader variables
		Shared<OpenGLVertexBuffer> glVertexBuffer = std::static_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
		glVertexBuffer->Bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (const void*)8);

		Shared<OpenGLIndexBuffer> glIndexBuffer = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
		glIndexBuffer->Bind();

		glBindVertexArray(va);
		glDrawElements(GL_TRIANGLES, glIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

		glDeleteVertexArrays(1, &va);
	}

	void OpenGLRendererAPI::MessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			PX_CORE_CRITICAL(message);
			return;

		case GL_DEBUG_SEVERITY_MEDIUM:
			PX_CORE_ERROR(message);
			return;

		case GL_DEBUG_SEVERITY_LOW:
			PX_CORE_WARN(message);
			return;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			PX_CORE_TRACE(message);
			return;
		}

		PX_CORE_ASSERT(false, "Unknown OpenGL severity level.");
	}
}