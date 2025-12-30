# Real WiFi Testing Guide

This guide explains how to test with real WiFi credentials using the `.env` file configuration.

## Setup

1. **Run the setup script** to create your `.env` file with WiFi credentials:
   ```bash
   bash Devtools/setup.sh
   ```
   
   The script will prompt you for:
   - WiFi SSID (network name)
   - WiFi Password

2. **Manual configuration** (alternative):
   - Copy `.env.example` to `.env`
   - Edit `.env` and fill in your credentials:
     ```
     WIFI_SSID=your_network_name
     WIFI_PASSWORD=your_password
     ```

## Using WiFi Credentials in Tests

### With MockNetworkService

```cpp
#include "mocks/MockNetworkService.h"

// Create mock service
testing::mocks::MockNetworkService network_service;

// Load credentials from .env file
auto load_result = network_service.load_credentials_from_env("/home/hurel/repo/IOT/.env");

if (load_result.succeed()) {
    // Credentials are now stored in the mock
    auto ssid_result = network_service.get_stored_ssid();
    auto password_result = network_service.get_stored_password();
    
    if (ssid_result.succeed() && password_result.succeed()) {
        std::string ssid = ssid_result.value();
        std::string password = password_result.value();
        
        // Connect using loaded credentials
        auto result = network_service.connect();
        if (result.succeed()) {
            // Connection successful
        }
    }
} else {
    // Handle error - file not found, missing credentials, etc.
    std::cout << load_result.error().message << std::endl;
}
```
```

### Running Tests

```bash
# Run all tests
pio test

# Run specific test
pio test -f test_real_wifi
```

## Security Notes

- ⚠️ **Never commit `.env` file to version control** - it's already in `.gitignore`
- The `.env.example` file is safe to commit as a template
- Keep your WiFi credentials secure and don't share them

## MockNetworkService API

### New Methods (with Result Pattern)

- `load_credentials_from_env(path)` → `Result<void>` - Load WiFi credentials from .env file
  - Returns success if credentials loaded
  - Returns failure with error message if file not found or credentials missing
  
- `get_stored_ssid()` → `Result<std::string>` - Get the currently stored SSID
  - Returns success with SSID value if set
  - Returns failure if SSID not set
  
- `get_stored_password()` → `Result<std::string>` - Get the currently stored password
  - Returns success with password value if set
  - Returns failure if password not set

### Existing Methods

- `set_credentials(ssid, password)` - Manually set credentials
- `connect()` - Attempt to connect to WiFi
- `disconnect()` - Disconnect from WiFi
- `is_connected()` - Check connection status
- `get_ip_address()` - Get IP address (when connected)
- `get_rssi()` - Get signal strength (when connected)

### Test Configuration Methods

- `set_connect_result(bool)` - Control whether connection succeeds
- `set_download_result(bool)` - Control whether downloads succeed
- `set_mock_ip(string)` - Set mock IP address
- `set_mock_rssi(int)` - Set mock RSSI value
- `set_connected(bool)` - Manually set connection state

## Example Test

See `test/test_real_wifi/test_real_wifi.cpp` for a complete example of testing with .env credentials.
