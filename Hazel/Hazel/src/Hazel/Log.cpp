#include "Log.h"
namespace Hazel
{
	std::shared_ptr < spdlog::logger > Log:: s_CoreLogger(nullptr);
	std::shared_ptr < spdlog::logger> Log:: s_Clientlogger(nullptr);
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("Hazel");
		s_CoreLogger -> set_level(spdlog::level::trace);


		s_Clientlogger = spdlog::stdout_color_mt("APP");
		s_Clientlogger->set_level(spdlog::level::trace);

	}

}