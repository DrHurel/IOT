#!/bin/bash
# Stop PostgreSQL container

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Stopping PlantNanny PostgreSQL database..."
docker compose down

echo "✓ PostgreSQL stopped"
