config_files=$(find ./k-100/config-files-graph-template/ -type f -name "*.txt")

rm -rf ./k-100/output-100k/*-queries.txt
rm -rf ./k-100/output-100k/*-graph.txt

# Loop through each configuration file and run the program 4 times
for config_file in $config_files; do

    config_name=$(basename "$config_file" .txt)
    config_name=${config_name:7}
    
    config_file_queries="./k-100/config-files-query-template/$(basename "$config_file")"


    for i in {1..4}; do
        
        rm ./data/*-graph.bin

        echo "Running with config file: $config_file (iteration $i)"
        ../bin/project -config "$config_file" >> "./k-100/output-100k/output-${config_name}-graph.txt"

        echo "Running queries on the created graph: $config_file_queries"
        ../bin/project -config "$config_file_queries" >> "./k-100/output-100k/output-${config_name}-queries.txt"


    done

    graph_file=$(ls ./data/*-graph.bin 2>/dev/null)
        
    # Ensure the graph file exists and rename it to avoid overwriting
    if [ -n "$graph_file" ]; then
        cp "$graph_file" "./graphs/${config_name}-100k-${i}.bin"
    else
        echo "No graph file found for $config_file"
    fi

done