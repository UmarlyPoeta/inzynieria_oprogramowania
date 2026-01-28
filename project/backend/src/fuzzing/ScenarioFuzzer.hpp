#pragma once

#include "../scenario/ScenarioRunner.hpp"
#include "../scenario/ScenarioTypes.hpp"
#include "../core/Network.hpp"
#include "../core/Engine.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Silence cout and cerr to avoid spamming the console during fuzzing
struct SuppressOutput {
    SuppressOutput();
    ~SuppressOutput();

private:
    std::ofstream dev_null;
    std::streambuf* orig_cout_buf;
    std::streambuf* orig_cerr_buf;
};

// Global instance to suppress output for the entire duration of the fuzzer run
extern SuppressOutput suppressor;

// Fuzzer entry point
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);