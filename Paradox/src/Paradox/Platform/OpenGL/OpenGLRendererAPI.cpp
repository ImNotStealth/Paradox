#include "pxpch.h"
#include "OpenGLRendererAPI.h"

#include "Paradox/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLShader.h"
#include "Paradox/Platform/OpenGL/OpenGLPipeline.h"
#include "Paradox/Platform/OpenGL/OpenGL.h"

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
		glFrontFace(GL_CCW);
	}

	void OpenGLRendererAPI::BeginRenderPass(const Shared<Pipeline>& pipeline)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Shared<OpenGLShader> shader = std::static_pointer_cast<OpenGLShader>(pipeline->GetProperties().shader);
		shader->Bind();

		Shared<OpenGLPipeline> glPipeline = std::static_pointer_cast<OpenGLPipeline>(pipeline);
		glPipeline->Bind();

		glLineWidth(glPipeline->GetProperties().wireframeWidth);
		glPolygonMode(GL_FRONT_AND_BACK, glPipeline->GetProperties().wireframe ? GL_LINE : GL_FILL);

		CullMode cullMode = pipeline->GetProperties().cullMode;
		if (cullMode != CullMode::None)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(cullMode == CullMode::FrontAndBack ? GL_FRONT_AND_BACK : cullMode == CullMode::Front ? GL_FRONT : GL_BACK);
		}
		else
			glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::EndRenderPass()
	{
	}

	void OpenGLRendererAPI::DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer)
	{
		Shared<OpenGLVertexBuffer> glVertexBuffer = std::static_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
		glVertexBuffer->Bind();

		Shared<OpenGLIndexBuffer> glIndexBuffer = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
		glIndexBuffer->Bind();

		glDrawElements(GL_TRIANGLES, glIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::MessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* message, const void* userParam)
	{
#ifndef PX_PLATFORM_PSVITA
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
#endif

		PX_CORE_ASSERT(false, "Unknown OpenGL severity level.");
	}
}