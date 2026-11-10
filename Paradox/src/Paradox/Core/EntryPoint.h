#pragma once

#include "Paradox/Core/Version.h"

extern Paradox::Application* Paradox::CreateApplication(const Paradox::CommandLineParser& args);

int EntryPoint(int argc, char** argv)
{
	Paradox::Log::Init();
	auto app = Paradox::CreateApplication({ argc, argv });
	app->Run();
	delete app;
	Paradox::Log::Shutdown();
	return 0;
}

#if defined(PX_PLATFORM_WINDOWS) && defined(PX_DIST)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	return EntryPoint(__argc, __argv);
}
#else
int main(int argc, char** argv)
{
	return EntryPoint(argc, argv);
}
#endif