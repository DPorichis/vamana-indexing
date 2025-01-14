graph_config_files=$(find ./k-50/config-files-graph-template/ -type f -name "*parallel.txt")

rm -rf ./k-50/output-50k/*-thread.txt
threadcounts=(1 2 4 8 16)

for graph_config_file in $graph_config_files; do
    
    graph_config_name=$(basename "$graph_config_file" .txt)
    graph_config_name=${graph_config_name:7}

    # Create graphs for each number of threads
    for threads in "${threadcounts[@]}"; do
        # Graph creation
        rm ./data/*-graph.bin
        echo "Running with config file: $graph_config_file(threads $threads)"   
        sed -i "s/^threadcount=.*/threadcount=${threads}/" "$graph_config_file"
        ../bin/project -config "$graph_config_file" >> "./k-50/output-50k/output-${graph_config_name}-thread.txt"

        # Queries for the graph
        query_config_file=$(basename config-"$graph_config_name".txt)

        # Update the threadcount parameter in the config file
        # If threadcount exists, replace its value; otherwise, add it at the end
        
        # Update the threadcount in the config file
        sed -i "s/^threadcount=.*/threadcount=${threads}/" "./k-50/config-files-query-template/$query_config_file"
        echo "Running with config file: $query_config_file(threads $threads)"
        ../bin/project -config "./k-50/config-files-query-template/$query_config_file" >> "./k-50/output-50k/output-${graph_config_name}-thread.txt"
    done
done
rm ./data/*-graph.bin