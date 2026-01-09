# Authentication

> **Note:** This page is under construction. See [API Overview](/api/overview) for basic authentication information.

PlantNanny uses JWT (JSON Web Token) based authentication for secure API access.

## Authentication Flow

### 1. Register New Account

```http
POST /api/v1/auth/register
Content-Type: application/json

{
  "email": "user@example.com",
  "password": "secure_password",
  "name": "John Doe"
}
```

Response:
```json
{
  "status": "success",
  "data": {
    "userId": "user123",
    "email": "user@example.com",
    "name": "John Doe",
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
  }
}
```

### 2. Login

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
  "status": "success",
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "expiresAt": "2025-12-06T12:00:00Z",
    "user": {
      "id": "user123",
      "email": "user@example.com",
      "name": "John Doe"
    }
  }
}
```

### 3. Use Token

Include the token in the Authorization header for all authenticated requests:

```http
GET /api/v1/plants
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

### 4. Refresh Token

```http
POST /api/v1/auth/refresh
Authorization: Bearer <expired_or_expiring_token>
```

Response:
```json
{
  "status": "success",
  "data": {
    "token": "new_token...",
    "expiresAt": "2025-12-07T12:00:00Z"
  }
}
```

### 5. Logout

```http
POST /api/v1/auth/logout
Authorization: Bearer <token>
```

## Token Details

### JWT Structure

The JWT token contains:
- **Header**: Algorithm and token type
- **Payload**: User ID, email, expiration
- **Signature**: Cryptographic signature

### Token Expiration

- **Default lifetime**: 24 hours
- **Refresh window**: Can be refreshed 1 hour before expiration
- **Maximum lifetime**: 7 days (requires re-authentication)

### Security

- Tokens are signed with a secret key
- HTTPS required in production
- Tokens should be stored securely (keychain, secure storage)
- Never include tokens in URLs or logs

## Error Responses

### Invalid Credentials
```json
{
  "status": "error",
  "error": {
    "code": "INVALID_CREDENTIALS",
    "message": "Email or password is incorrect"
  }
}
```

### Token Expired
```json
{
  "status": "error",
  "error": {
    "code": "TOKEN_EXPIRED",
    "message": "Authentication token has expired"
  }
}
```

### Token Invalid
```json
{
  "status": "error",
  "error": {
    "code": "INVALID_TOKEN",
    "message": "Authentication token is invalid"
  }
}
```

## Password Requirements

- Minimum 8 characters
- At least one uppercase letter
- At least one lowercase letter
- At least one number
- Special characters recommended

## Password Reset

```http
POST /api/v1/auth/reset-password
Content-Type: application/json

{
  "email": "user@example.com"
}
```

An email with a reset link will be sent.

## Best Practices

1. **Store tokens securely** - Use platform-specific secure storage
2. **Use HTTPS** - Always use encrypted connections
3. **Handle expiration** - Implement token refresh logic
4. **Logout on exit** - Clear tokens when user logs out
5. **Don't share tokens** - Each device should have its own token

## Next Steps

- [Controller API](/api/controller)
- [Server API](/api/server)
- [API Overview](/api/overview)

---

*See [OpenAPI Specification](https://github.com/DrHurel/IOT/blob/main/PlantNanny/api/plant_nanny_api.yaml) for complete details.*
