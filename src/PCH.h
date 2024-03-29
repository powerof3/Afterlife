#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif

using namespace std::literals;
namespace logger = SKSE::log;

namespace stl
{
    using namespace SKSE::stl;
}

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
