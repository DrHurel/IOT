#pragma once

#include <unity.h>
#include "testing/libs/plant_nanny/services/network/MockINetworkService.h"
#include <memory>

using namespace plant_nanny::services;
using namespace testing::mocks;

// Global test state
extern std::unique_ptr<MockNetworkService> mock_network;

// Common test setup and teardown
void setUp();
void tearDown();
