#include "ScenarioFuzzer.hpp"

// SuppressOutput implementation
SuppressOutput::SuppressOutput() {
  // Redirect cout and cerr to /dev/null
  orig_cout_buf = std::cout.rdbuf(nullptr);
  orig_cerr_buf = std::cerr.rdbuf(nullptr);

  dev_null.open("/dev/null");
  if (dev_null.is_open()) {
    std::cout.rdbuf(dev_null.rdbuf());
    std::cerr.rdbuf(dev_null.rdbuf());
  }
}

SuppressOutput::~SuppressOutput() {
  // Restore original buffers
  std::cout.rdbuf(orig_cout_buf);
  std::cerr.rdbuf(orig_cerr_buf);
  if (dev_null.is_open()) {
    dev_null.close();
  }
}

// Global instance
SuppressOutput suppressor;

// Fuzzer entry point
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size == 0) {
    return 0;
  }

  std::string yaml_content(reinterpret_cast<const char *>(data), size);

  try {
    // Attempt to parse the YAML content
    auto scenario = netsim::scenario::Scenario::fromYAML(yaml_content);

    // If parsing succeeds, run the scenario
    Network network;
    Engine engine(network);
    netsim::scenario::ScenarioRunner runner(network, engine);

    runner.runScenario(scenario);

  } catch (const std::runtime_error &e) {
    // Catch parsing errors and other runtime errors.
    // This is expected for malformed input, so we just return.
    return 0;
  } catch (const std::exception &e) {
    // Catch other standard exceptions
    return 0;
  }

  return 0;
}
