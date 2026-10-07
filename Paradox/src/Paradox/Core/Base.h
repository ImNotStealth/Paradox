#pragma once

#include <memory>

#include "Paradox/Core/PlatformDetection.h"

#ifdef PX_PLATFORM_WINDOWS
	#ifdef PX_BUILD_DLL
		#ifdef PX_DLL_EXPORT
			#define PARADOX_API __declspec(dllexport)
		#else
			#define PARADOX_API __declspec(dllimport)
		#endif
	#else
		#define PARADOX_API
	#endif
#elif defined(PX_PLATFORM_LINUX)
	#ifdef PX_BUILD_DLL
		#define PARADOX_API __attribute__((visibility("default")))
	#else
		#define PARADOX_API
	#endif
#elif defined(PX_PLATFORM_PSVITA)
	#define PARADOX_API
#else
	#error Platform not supported!
#endif

#ifdef PX_DEBUG
	#if defined(PX_PLATFORM_WINDOWS)
		#define PX_DEBUGBREAK() __debugbreak()
	#elif defined(PX_PLATFORM_LINUX)
		#include <signal.h>
		#define PX_DEBUGBREAK() raise(SIGTRAP)
	#elif defined(PX_PLATFORM_PSVITA)
		#include <psp2/kernel/processmgr.h>
		#define PX_DEBUGBREAK() sceKernelDelayThread(1*1000000); sceKernelExitProcess(0)
	#else
		#error "Platform doesn't support debugbreak."
	#endif
	#define PX_ENABLE_ASSERTS
#else
	#define PX_DEBUGBREAK()
#endif

#define BIT(x) (1 << x)

namespace Paradox
{
	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename... Args>
	constexpr Unique<T> CreateUnique(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Shared = std::shared_ptr<T>;
	template<typename T, typename... Args>
	constexpr Shared<T> CreateShared(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Weak = std::weak_ptr<T>;

	struct ReferenceControl
	{
		uint32_t refCount = 1;
		void* instance = nullptr;
		void (*destructor)(void*) = nullptr;

		/*
		Ok so this part was thanks to copilot.
		From my understanding this saves the destructor of T.
		In the event that Reference is holding a void (like in Shader.h)
		it can still correctly call T's destructor
		*/

		template<typename T>
		explicit ReferenceControl(T* instance)
			: instance(instance), destructor([](void* object) { delete static_cast<T*>(object); })
		{}
	};

	// A Ref counting class, references are non-owning
	template<typename T>
	class Reference
	{
	public:
		Reference() = default;

		Reference(std::nullptr_t) noexcept {}

		explicit Reference(T* instance)
			: m_Instance(instance)
		{
			// Can't use PX_CORE_ASSERT as it'll mess up include order
			static_assert(!std::is_void_v<T>, "Reference<void> must be created by converting an owning Reference.");

			if (m_Instance)
				m_Control = new ReferenceControl(instance);
		}

		Reference(const Reference& other)
			: m_Instance(other.m_Instance), m_Control(other.m_Control)
		{
			IncRef();
		}

		Reference(Reference&& other) noexcept
			: m_Instance(other.m_Instance), m_Control(other.m_Control)
		{
			other.Nullify();
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		Reference(const Reference<U>& other)
			: m_Instance(other.Get()), m_Control(other.m_Control)
		{
			IncRef();
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
		Reference(Reference<U>&& other) noexcept
			: m_Instance(other.Get()), m_Control(other.m_Control)
		{
			other.Nullify();
		}

		Reference& operator=(const Reference& other)
		{
			if (this != &other)
			{
				Release();
				m_Instance = other.m_Instance;
				m_Control = other.m_Control;
				IncRef();
			}
			return *this;
		}

		Reference& operator=(Reference&& other) noexcept
		{
			if (this != &other)
			{
				Release();
				m_Instance = other.m_Instance;
				m_Control = other.m_Control;
				other.Nullify();
			}
			return *this;
		}

		~Reference()
		{
			Release();
		}

		void Reset()
		{
			Release();
		}

		template<typename U>
		Reference<U> AsA() const
		{
			Reference<U> result;
			result.m_Instance = static_cast<U*>(m_Instance);
			result.m_Control = m_Control;
			result.IncRef();
			return result;
		}

		T* Get() const { return m_Instance; }
		T* operator->() const { return m_Instance; }
		std::add_lvalue_reference_t<T> operator*() const { return *m_Instance; }
		explicit operator bool() const { return m_Instance != nullptr; }

		uint32_t GetRefCount() const { return m_Control ? m_Control->refCount : 0; }

	private:
		template<typename U>
		friend class Reference;

		void IncRef()
		{
			if (m_Control)
				++m_Control->refCount;
		}

		void Release()
		{
			ReferenceControl* control = m_Control;
			m_Instance = nullptr;
			m_Control = nullptr;

			if (!control || --control->refCount != 0)
				return;

			control->destructor(control->instance);
			delete control;
		}

		void Nullify()
		{
			m_Instance = nullptr;
			m_Control = nullptr;
		}

	private:
		T* m_Instance = nullptr;
		ReferenceControl* m_Control = nullptr;
	};

	template<typename T, typename... Args>
	constexpr Reference<T> CreateRef(Args&&... args)
	{
		return Reference<T>(new T(std::forward<Args>(args)...));
	}
}

#include "Paradox/Core/Assert.h"
#include "Paradox/Core/Profiler.h"