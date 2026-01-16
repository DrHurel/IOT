#!/bin/bash
# Reset PostgreSQL container and data

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "⚠️  This will delete all PostgreSQL data!"
read -p "Are you sure? (y/N) " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Stopping and removing containers..."
    docker compose down -v
    
    echo "✓ PostgreSQL data reset"
    echo ""
    echo "Run ./launch.sh to start fresh"
else
    echo "Cancelled"
fi
