graph_config_files=$(find ./config-files-thread/ -type f -name "*parallel.txt")

rm -rf ./output-data/*-thread.txt
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
        ../bin/project -config "$graph_config_file" >> "./output-data/output-${graph_config_name}-thread.txt"

        # Queries for the graph
        query_config_file=$(basename config-"$graph_config_name"-query.txt)

        # Update the threadcount parameter in the config file
        # If threadcount exists, replace its value; otherwise, add it at the end
        
        # Update the threadcount in the config file
        sed -i "s/^threadcount=.*/threadcount=${threads}/" "./config-files-thread/$query_config_file"

        ../bin/project -config "./config-files-thread/$query_config_file" >> "./output-data/output-${graph_config_name}-thread.txt"


    done
done
rm ./data/*-graph.bin