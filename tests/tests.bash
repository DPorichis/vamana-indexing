SCRIPT_DIR="$(dirname "$(realpath "$0")")"

cd $SCRIPT_DIR
../bin/graph_test
../bin/io_test
../bin/gready_test
../bin/prune_test
../bin/vamana_test