#pragma once

extern Paradox::Application* Paradox::CreateApplication(const Paradox::CommandLineParser& args);

int main(int argc, char** argv)
{
	Paradox::Log::Init();
	auto app = Paradox::CreateApplication({ argc, argv });
	app->Run();
	delete app;
	Paradox::Log::Shutdown();
}