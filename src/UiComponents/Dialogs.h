#pragma once

#include <functional>

void showSaveDialog(const char title[], const std::function<void(const char[])> &callback);
