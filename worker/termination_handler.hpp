#pragma once

#include <exception>

[[noreturn]] void onTerminate() noexcept;
[[noreturn]] void onBadSignal(int signal) noexcept;
void parseAndLogPreviousDump();
