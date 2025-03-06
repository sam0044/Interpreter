set -e # Exit on failure

cd "$(dirname "$0")/.."
exec ./build/interpreter "$@"