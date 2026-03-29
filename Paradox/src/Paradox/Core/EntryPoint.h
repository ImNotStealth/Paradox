#pragma once

extern Paradox::Application* Paradox::CreateApplication();

int main(int argc, char** argv)
{
	Paradox::Log::Init();
	auto app = Paradox::CreateApplication();
	app->Run();
	delete app;
}