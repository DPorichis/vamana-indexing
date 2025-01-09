config_files=$(find ./config-files-query/ -type f -name "*.txt")

rm -rf ./output-data/*-queries.txt

# Loop through each configuration file and run the program 4 times
for config_file in $config_files; do
    
    config_name=$(basename "$config_file" .txt)
    config_name=${config_name:7}
    
    for i in {1..4}; do
        
        echo "Running with config file: $config_file (iteration $i)"
        ../bin/project -config "$config_file" >> "./output-data/output-${config_name}-queries.txt"

    done

done