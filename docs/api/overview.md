# API Overview

PlantNanny provides a comprehensive RESTful API for communication between controllers, servers, and mobile applications.

## API Architecture

The system consists of two primary APIs:

1. **Controller API** - Endpoints exposed by the ESP32 controller
2. **Server API** - Cloud-based backend services

## Base URLs

### Production
```
Controller: https://<controller-ip>/api/v1
Server:     https://api.plantnanny.io/api/v1
```

### Development
```
Controller: http://<controller-ip>/api/v1
Server:     http://localhost:3000/api/v1
```

## Authentication

### JWT Token Authentication

Most endpoints require authentication using JWT tokens:

```http
Authorization: Bearer <token>
```

### Obtaining Tokens

```http
POST /api/v1/auth/login
Content-Type: application/json

{
  "email": "user@example.com",
  "password": "secure_password"
}
```

Response:
```json
{
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "expires_at": "2025-12-06T12:00:00Z",
  "user": {
    "id": "user123",
    "email": "user@example.com",
    "name": "John Doe"
  }
}
```

## API Specifications

Full API specifications are available in OpenAPI 3.0 format:

- **Controller API**: [`RestApi/controller.yaml`](https://github.com/DrHurel/IOT/blob/main/RestApi/controller.yaml)
- **Server API**: [`RestApi/server.yaml`](https://github.com/DrHurel/IOT/blob/main/RestApi/server.yaml)

## Common Endpoints

### Server API

#### Authentication
- `POST /auth/register` - Create new account
- `POST /auth/login` - Authenticate user
- `POST /auth/logout` - End session
- `POST /auth/refresh` - Refresh token

#### Plants
- `GET /plants` - List user's plants
- `POST /plants` - Add new plant
- `GET /plants/{id}` - Get plant details
- `PUT /plants/{id}` - Update plant configuration
- `DELETE /plants/{id}` - Remove plant
- `POST /plants/{id}/water` - Trigger watering

#### Controllers
- `GET /controllers` - List user's controllers
- `GET /controllers/{id}` - Get controller info
- `POST /controllers/{id}/pair` - Pair controller
- `GET /controllers/{id}/sensors` - Get sensor list

#### Measurements
- `GET /measurements` - Query measurements
- `GET /measurements/plants/{plantId}` - Get plant history
- `POST /measurements` - Upload measurement (controller)

### Controller API

#### Status
- `GET /status` - Get controller status
- `GET /sensors` - List available sensors
- `GET /config` - Get current configuration

#### Data
- `GET /measurements` - Get recent measurements
- `GET /measurements/latest` - Get latest reading

#### Control
- `POST /water` - Trigger watering
- `POST /calibrate` - Run sensor calibration

#### Configuration
- `PUT /config` - Update configuration
- `POST /wifi` - Configure WiFi (via Bluetooth)

## Request/Response Format

### Content Types

All requests and responses use JSON:
```http
Content-Type: application/json
Accept: application/json
```

### Standard Response Structure

#### Success Response
```json
{
  "status": "success",
  "data": {
    // Response data
  },
  "message": "Operation completed successfully"
}
```

#### Error Response
```json
{
  "status": "error",
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Invalid input parameters",
    "details": [
      {
        "field": "temperature",
        "issue": "Value must be between -40 and 80"
      }
    ]
  }
}
```

## Status Codes

### Success Codes
- `200 OK` - Request successful
- `201 Created` - Resource created
- `204 No Content` - Success with no response body

### Client Error Codes
- `400 Bad Request` - Invalid input
- `401 Unauthorized` - Missing or invalid authentication
- `403 Forbidden` - Insufficient permissions
- `404 Not Found` - Resource not found
- `409 Conflict` - Resource conflict (e.g., duplicate)
- `422 Unprocessable Entity` - Validation error

### Server Error Codes
- `500 Internal Server Error` - Server error
- `503 Service Unavailable` - Service temporarily unavailable

## Rate Limiting

API requests are rate-limited to prevent abuse:

- **Authenticated requests**: 1000 requests/hour
- **Unauthenticated requests**: 100 requests/hour

Rate limit headers:
```http
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 999
X-RateLimit-Reset: 1638360000
```

## Pagination

List endpoints support pagination:

```http
GET /plants?page=1&limit=20
```

Response includes pagination metadata:
```json
{
  "status": "success",
  "data": [...],
  "pagination": {
    "page": 1,
    "limit": 20,
    "total": 45,
    "pages": 3
  }
}
```

## Filtering and Sorting

### Filtering
```http
GET /measurements?plantId=plant123&startDate=2025-12-01
```

### Sorting
```http
GET /plants?sort=name&order=asc
```

## Webhook Support

Server can send webhooks for events:

- Plant watering completed
- Sensor reading threshold exceeded
- Controller offline/online

Configure webhooks:
```http
POST /webhooks
{
  "url": "https://your-app.com/webhook",
  "events": ["plant.watered", "sensor.threshold"],
  "secret": "webhook_secret"
}
```

## WebSocket Support (Planned)

Real-time updates via WebSocket:

```javascript
ws://api.plantnanny.io/ws?token=<jwt_token>

// Subscribe to plant updates
{
  "action": "subscribe",
  "resource": "plant",
  "id": "plant123"
}
```

## API Versioning

The API uses URL versioning:
- Current version: `v1`
- URL format: `/api/v1/...`

Breaking changes will result in a new version (`v2`, etc.).

## SDKs and Clients

### Official SDKs (Planned)
- JavaScript/TypeScript
- Python
- React Native (for mobile app)

### Example Client Code

#### JavaScript
```javascript
const client = new PlantNannyClient({
  baseURL: 'https://api.plantnanny.io/api/v1',
  token: 'your-jwt-token'
});

// Get plants
const plants = await client.plants.list();

// Trigger watering
await client.plants.water('plant123');
```

#### Python
```python
from plantnanny import Client

client = Client(
    base_url='https://api.plantnanny.io/api/v1',
    token='your-jwt-token'
)

# Get plants
plants = client.plants.list()

# Trigger watering
client.plants.water('plant123')
```

## Testing the API

### Using cURL

```bash
# Login
curl -X POST https://api.plantnanny.io/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"user@example.com","password":"pass"}'

# Get plants (with token)
curl https://api.plantnanny.io/api/v1/plants \
  -H "Authorization: Bearer <token>"
```

### Using Postman

Import the OpenAPI specifications:
1. Open Postman
2. Import → Link
3. Enter: `https://raw.githubusercontent.com/DrHurel/IOT/main/RestApi/server.yaml`

### Interactive Documentation (Planned)

Swagger UI for interactive API testing:
```
https://api.plantnanny.io/docs
```

## Next Steps

- [Authentication Details](/api/authentication)
- [Controller Endpoints](/api/controller)
- [Server Endpoints](/api/server)
- [OpenAPI Specifications](https://github.com/DrHurel/IOT/tree/main/RestApi)
