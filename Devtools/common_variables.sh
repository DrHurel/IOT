# Find the root path of the git repository
UM_ROOT_PATH=$(git rev-parse --show-toplevel 2>/dev/null)

# If not in a git repo, fallback to script directory
if [ -z "$UM_ROOT_PATH" ]; then
    UM_ROOT_PATH="$(cd "$(dirname "$0")/.." && pwd)"
fi