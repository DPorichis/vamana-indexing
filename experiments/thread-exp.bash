config_files=$(find ./config-files-graph/ -type f -name "*.txt")

rm -rf ./output-data/*-thread.txt
threadcounts=(1 2 4 8 16)

# Loop through each configuration file and run the program 4 times
for config_file in $config_files; do
    
    config_name=$(basename "$config_file" .txt)
    config_name=${config_name:7}

    config_file_queries="./config-files-query/$(basename "$config_file")"

    
    # Iterate through the array of threadcount values
    for threadcount in "${threadcounts[@]}"; do
        # Update the threadcount parameter in the config file
        # If threadcount exists, replace its value; otherwise, add it at the end
        if grep -q "^threadcount=" "$config_file"; then
            sed -i "s/^threadcount=.*/threadcount=$threadcount/" "$config_file"
        else
            echo "threadcount=$threadcount" >> "$config_file"
        fi

        # Print the updated configuration for this iteration
        echo "Updated 'threadcount' in '$config_file' to '$threadcount'."
        
        # Optionally, run a command using the updated configuration
        # Example:
        # ./run_program --config "$config_file"
    done
done