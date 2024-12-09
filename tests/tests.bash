SCRIPT_DIR="$(dirname "$(realpath "$0")")"

cd $SCRIPT_DIR
cd ..
echo Checking graph

./bin/graph_test

echo Checking io

./bin/io_test

echo Checking gready

./bin/gready_test

echo Checking prune

./bin/prune_test

echo Checking filtered vamana functions

./bin/filtered_test

echo Checking all vamana indexing methods

./bin/vamana_test
