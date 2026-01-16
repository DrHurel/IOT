#!/bin/bash
# Launch PostgreSQL container for PlantNanny

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Starting PlantNanny PostgreSQL database..."
docker compose up -d

echo ""
echo "Waiting for PostgreSQL to be ready..."
sleep 3

# Check if healthy
if docker compose ps | grep -q "healthy"; then
    echo "✓ PostgreSQL is ready!"
    echo ""
    echo "Connection details:"
    echo "  Host: localhost"
    echo "  Port: 5432"
    echo "  Database: plantnanny"
    echo "  User: plantnanny"
    echo "  Password: plantnanny_secret"
    echo ""
    echo "Connection URL: postgresql://plantnanny:plantnanny_secret@localhost:5432/plantnanny"
else
    echo "PostgreSQL is starting... Check with: docker compose logs -f"
fi
