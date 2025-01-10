config_files=$(find ./config-files-graph-1m/ -type f -name "*-1m.txt")

rm -rf ./output-data/*-queries.txt

# Loop through each configuration file and run the program 4 times
for config_file in $config_files; do
    
    config_name=$(basename "$config_file" .txt)
    config_name=${config_name:7}
    
    rm ./data/*-graph.bin

    echo "Running with config file: $config_file (iteration $i)"
    ../bin/project -config "$config_file" >> "./output-data/output-${config_name}-graph.txt"

    graph_file=$(ls ./data/*-graph.bin 2>/dev/null)
        
    # Ensure the graph file exists and rename it to avoid overwriting
    if [ -n "$graph_file" ]; then
        cp "$graph_file" "./graphs/${config_name}.bin"
    else
        echo "No graph file found for $config_file"
    fi

done