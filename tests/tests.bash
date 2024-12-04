SCRIPT_DIR="$(dirname "$(realpath "$0")")"

cd $SCRIPT_DIR
echo Checking graph

../bin/graph_test

echo Checking io

../bin/io_test

echo Checking gready

../bin/gready_test

echo Checking prune

../bin/prune_test

echo Checking vamana

../bin/vamana_test

echo Checking filtered vamana

../bin/filtered_test