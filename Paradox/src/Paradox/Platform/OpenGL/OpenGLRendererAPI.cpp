#include "pxpch.h"
#include "OpenGLRendererAPI.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Renderer/Renderer2D.h"
#include "Paradox/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLShader.h"
#include "Paradox/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Paradox/Platform/OpenGL/OpenGL.h"

namespace Paradox
{
	void OpenGLRendererAPI::Init()
	{
#if defined(PX_DEBUG) && !defined(PX_PLATFORM_PSVITA)
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CW);
	}

	void OpenGLRendererAPI::BeginFrame()
	{
		Application::Get().GetWindow().GetSwapChain()->Begin();
		Renderer2D::InitFrame();
	}

	void OpenGLRendererAPI::EndFrame()
	{
		Application::Get().GetWindow().GetSwapChain()->End();
	}


	void OpenGLRendererAPI::BeginRenderPass(const Shared<Pipeline>& pipeline)
	{
		PX_PROFILE_FUNCTION();
		Shared<OpenGLFramebuffer> framebuffer = std::static_pointer_cast<OpenGLFramebuffer>(pipeline->GetProperties().framebuffer);
		framebuffer->Bind();

		bool hasDepth = framebuffer->HasDepth();
		if (hasDepth)
			glEnable(GL_DEPTH_TEST);

		// By default, we use multiple framebuffers and assemble them when we're done
		// Renderer2D uses a depth buffer and those can't be read as textures so we can't use multiple framebuffers on Vita
		// Best "hotfix" is to not clear the SceneFramebuffer that is used before it and use that one for Renderer2D.
		//TODO: Find a better fix
#ifdef PX_PLATFORM_PSVITA
		bool clear = framebuffer->GetProperties().clear && pipeline->GetProperties().debugName != "Renderer2D";
#else
		bool clear = framebuffer->GetProperties().clear;
#endif
		if (clear)
		{
			glm::vec4 clearColor = framebuffer->GetProperties().clearColor;
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			GLbitfield clearBits = GL_COLOR_BUFFER_BIT;
			if (hasDepth)
				clearBits |= GL_DEPTH_BUFFER_BIT;
			glClear(clearBits);
		}

		Shared<OpenGLShader> shader = std::static_pointer_cast<OpenGLShader>(pipeline->GetProperties().shader);
		shader->Bind();

		Shared<OpenGLPipeline> glPipeline = std::static_pointer_cast<OpenGLPipeline>(pipeline);
		m_CurrentPipeline = glPipeline;

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
		PX_PROFILE_FUNCTION();
		Shared<OpenGLShader> shader = std::static_pointer_cast<OpenGLShader>(m_CurrentPipeline->GetProperties().shader);
		shader->Unbind();
		if (m_CurrentPipeline)
		{
			m_CurrentPipeline->Unbind();
			m_CurrentPipeline = nullptr;
		}
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLRendererAPI::DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer, const uint32_t indexCount)
	{
		PX_PROFILE_FUNCTION();
		Shared<OpenGLVertexBuffer> glVertexBuffer = std::static_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
		glVertexBuffer->Bind();

		m_CurrentPipeline->Bind();

		Shared<OpenGLIndexBuffer> glIndexBuffer = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
		glIndexBuffer->Bind();

		glDrawElements(GL_TRIANGLES, indexCount == 0 ? glIndexBuffer->GetCount() : indexCount, glIndexBuffer->GetIndexType(), nullptr);
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