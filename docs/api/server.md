# Server API

> **Note:** This page is under construction. See [API Overview](/api/overview) for general API information.

The Server API provides cloud-based services for plant management, user authentication, and historical data storage.

## Base URL

```
Production: https://api.plantnanny.io/api/v1
Development: http://localhost:3000/api/v1
```

## Authentication Required

Most endpoints require JWT authentication. See [Authentication](/api/authentication) for details.

```http
Authorization: Bearer <token>
```

## Endpoints

### Plants

#### List User's Plants

```http
GET /plants
Authorization: Bearer <token>
```

Response:
```json
{
  "status": "success",
  "data": {
    "plants": [
      {
        "id": "plant123",
        "name": "Spider Plant",
        "controllerId": "esp32-abc123",
        "temperature": 22.5,
        "humidity": 55.2,
        "lastWatering": "2025-12-05T08:00:00Z",
        "location": {
          "latitude": 48.8566,
          "longitude": 2.3522,
          "name": "Living Room"
        }
      }
    ]
  }
}
```

#### Get Plant Details

```http
GET /plants/{plantId}
Authorization: Bearer <token>
```

#### Create New Plant

```http
POST /plants
Authorization: Bearer <token>
Content-Type: application/json

{
  "name": "Spider Plant",
  "controllerId": "esp32-abc123",
  "temperatureOptimal": 21,
  "humidityOptimal": 50,
  "wateringCycle": 172800,
  "waterQuantity": 200,
  "location": {
    "latitude": 48.8566,
    "longitude": 2.3522,
    "name": "Living Room"
  }
}
```

#### Update Plant

```http
PUT /plants/{plantId}
Authorization: Bearer <token>
Content-Type: application/json

{
  "name": "Updated Name",
  "temperatureOptimal": 22,
  "humidityOptimal": 55
}
```

#### Delete Plant

```http
DELETE /plants/{plantId}
Authorization: Bearer <token>
```

#### Trigger Watering

```http
POST /plants/{plantId}/water
Authorization: Bearer <token>
Content-Type: application/json

{
  "quantity": 200,
  "manual": true
}
```

Response:
```json
{
  "status": "success",
  "data": {
    "watered": true,
    "timestamp": "2025-12-05T10:30:00Z",
    "quantity": 200
  }
}
```

### Controllers

#### List User's Controllers

```http
GET /controllers
Authorization: Bearer <token>
```

Response:
```json
{
  "status": "success",
  "data": {
    "controllers": [
      {
        "id": "esp32-abc123",
        "name": "Living Room Controller",
        "online": true,
        "lastSeen": "2025-12-05T10:30:00Z",
        "version": "1.0.0",
        "plantCount": 1
      }
    ]
  }
}
```

#### Get Controller Details

```http
GET /controllers/{controllerId}
Authorization: Bearer <token>
```

#### Pair Controller

```http
POST /controllers/{controllerId}/pair
Authorization: Bearer <token>
Content-Type: application/json

{
  "pairingCode": "ABC123"
}
```

#### Get Controller Sensors

```http
GET /controllers/{controllerId}/sensors
Authorization: Bearer <token>
```

### Measurements

#### Get Measurements

```http
GET /measurements?plantId=plant123&startDate=2025-12-01&endDate=2025-12-05
Authorization: Bearer <token>
```

Query Parameters:
- `plantId` - Filter by plant (required)
- `startDate` - Start date (ISO 8601)
- `endDate` - End date (ISO 8601)
- `type` - Measurement type (temperature, humidity, light)
- `page` - Page number
- `limit` - Results per page (max 1000)

Response:
```json
{
  "status": "success",
  "data": {
    "measurements": [
      {
        "timestamp": "2025-12-05T10:30:00Z",
        "temperature": 22.5,
        "humidity": 55.2,
        "light": 7500
      }
    ]
  },
  "pagination": {
    "page": 1,
    "limit": 100,
    "total": 1250,
    "pages": 13
  }
}
```

#### Get Plant History

```http
GET /measurements/plants/{plantId}
Authorization: Bearer <token>
```

#### Upload Measurements (Controller)

```http
POST /measurements
Authorization: Bearer <controller_token>
Content-Type: application/json

{
  "controllerId": "esp32-abc123",
  "plantId": "plant123",
  "measurements": [
    {
      "timestamp": "2025-12-05T10:30:00Z",
      "temperature": 22.5,
      "humidity": 55.2,
      "light": 7500
    }
  ]
}
```

### Users

#### Get User Profile

```http
GET /users/me
Authorization: Bearer <token>
```

Response:
```json
{
  "status": "success",
  "data": {
    "id": "user123",
    "email": "user@example.com",
    "name": "John Doe",
    "createdAt": "2025-01-01T00:00:00Z",
    "plantCount": 3,
    "controllerCount": 1
  }
}
```

#### Update Profile

```http
PUT /users/me
Authorization: Bearer <token>
Content-Type: application/json

{
  "name": "John Smith",
  "email": "newmail@example.com"
}
```

### Webhooks (Planned)

#### List Webhooks

```http
GET /webhooks
Authorization: Bearer <token>
```

#### Create Webhook

```http
POST /webhooks
Authorization: Bearer <token>
Content-Type: application/json

{
  "url": "https://your-app.com/webhook",
  "events": ["plant.watered", "sensor.threshold"],
  "secret": "webhook_secret"
}
```

## Pagination

List endpoints support pagination:

```http
GET /plants?page=2&limit=20
```

Response includes pagination metadata:
```json
{
  "pagination": {
    "page": 2,
    "limit": 20,
    "total": 45,
    "pages": 3
  }
}
```

## Filtering

Support for query parameters:

```http
GET /measurements?plantId=plant123&type=temperature&startDate=2025-12-01
```

## Sorting

```http
GET /plants?sort=name&order=asc
```

## Rate Limiting

- Authenticated: 1000 requests/hour
- Unauthenticated: 100 requests/hour

Headers:
```http
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 999
X-RateLimit-Reset: 1638360000
```

## Error Responses

### Not Found
```json
{
  "status": "error",
  "error": {
    "code": "NOT_FOUND",
    "message": "Plant not found"
  }
}
```

### Validation Error
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

### Authorization Error
```json
{
  "status": "error",
  "error": {
    "code": "FORBIDDEN",
    "message": "You don't have permission to access this resource"
  }
}
```

## WebSocket (Planned)

Real-time updates:

```
wss://api.plantnanny.io/ws?token=<jwt_token>
```

Subscribe to plant updates:
```json
{
  "action": "subscribe",
  "resource": "plant",
  "id": "plant123"
}
```

## Next Steps

- [Authentication](/api/authentication)
- [Controller API](/api/controller)
- [API Overview](/api/overview)

---

*Full documentation coming soon. See [OpenAPI Specification](https://github.com/DrHurel/IOT/blob/main/RestApi/server.yaml) for complete details.*
