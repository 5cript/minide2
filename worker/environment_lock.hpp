#pragma once

#include <functional>
#include <string>

/**
 *  You can safely change the environment of the worker here.
 *  But only here. Using this ensures that multiple ran processes can be started without conflict.
 */
void environmentLockedDo(std::function <void()> const& work);

/**
 *  Do something (like running a program) with a modified PATH.
 */
void doWithModifiedPath(std::function <void()> const& work, std::string const& path);
