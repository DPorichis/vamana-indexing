config_files=$(find ./k-20/config-files-graph-template/ -type f -name "*.txt")

rm -rf ./output-data/*-memory.txt
# Loop through each configuration file and run the program 4 times
for config_file in $config_files; do
    
    config_name=$(basename "$config_file" .txt)
    config_name=${config_name:7}

    config_file_queries="./k-20/config-files-query-template/$(basename "$config_file")"

    for i in {1..2}; do
        
        rm ./data/*-graph.bin

        echo "Running with config file: $config_file (iteration $i)"
        time_output=$(/usr/bin/time -v ../bin/project -config "$config_file" 2>&1)
        echo "$time_output" | grep "Maximum resident set size" >> "./k-20/output-20k/output-${config_name}-graph-memory.txt"

        echo "Running queries on the created graph: $config_file_queries"
        time_output=$(/usr/bin/time -v ../bin/project -config "$config_file" 2>&1)
        echo "$time_output" | grep "Maximum resident set size" >> "./k-20/output-20k/output-${config_name}-queries-memory.txt"


    done
done
