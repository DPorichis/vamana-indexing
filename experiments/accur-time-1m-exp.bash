config_files=$(find ./m-1/config-files-graph-template/ -type f -name "*.txt" | sort)

rm -rf ./m-1/output-1m/*-queries.txt
rm -rf ./m-1/output-1m/*-graph.txt

# Loop through each configuration file and run the program 4 times
for config_file in $config_files; do

    config_name=$(basename "$config_file" .txt)
    config_name=${config_name:9}
    
    config_file_queries="./m-1/config-files-query-template/$(basename "$config_file")"

    rm ./data/*-graph.bin

    echo "Running with config file: $config_file"
    ../bin/project -config "$config_file" >> "./m-1/output-1m/output-${config_name}-graph.txt"

    # echo "Running queries on the created graph: $config_file_queries"
    # ../bin/project -config "$config_file_queries" >> "./m-1/output-1m/output-${config_name}-queries.txt"

    graph_file=$(ls ./data/*-graph.bin 2>/dev/null)
        
    # Ensure the graph file exists and rename it to avoid overwriting
    if [ -n "$graph_file" ]; then
        cp "$graph_file" "./graphs/${config_name}-1m.bin"
    else
        echo "No graph file found for $config_file"
    fi

done