set -e # Exit early if any commands fail

(
  cd "$(dirname "$0")/.." # Ensure compile steps are run within the repository directory
  cmake -B build -S . 
  cmake --build ./build
)

exec ./build/interpreter "$@"