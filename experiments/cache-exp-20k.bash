config_files=$(find ./k-20/config-files-cache-graph/ -type f -name "*.txt")

rm -rf ./k-20/output-20k/*-cache.txt

# Loop through each configuration file and run the program 4 times
for config_file in $config_files; do

    config_name=$(basename "$config_file" .txt)
    config_name=${config_name:7}
    
    config_file_queries="./k-20/config-files-query-template/$(basename "$config_file")"


    for i in {1..4}; do
        
        rm ./data/*-graph.bin

        echo "Running with config file: $config_file (iteration $i)"
        ../bin/project -config "$config_file" >> "./k-20/output-20k/output-${config_name}-graph-cache.txt"

        echo "Running queries on the created graph: $config_file_queries"
        ../bin/project -config "$config_file_queries" >> "./k-20/output-20k/output-${config_name}-queries-cache.txt"


    done

    graph_file=$(ls ./data/*-graph.bin 2>/dev/null)
        
    # Ensure the graph file exists and rename it to avoid overwriting
    if [ -n "$graph_file" ]; then
        cp "$graph_file" "./graphs/${config_name}-${i}.bin"
    else
        echo "No graph file found for $config_file"
    fi

done