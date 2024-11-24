rm non_existent_file.txt && go test -coverprofile=coverage.out && go tool cover -html=coverage.out -o coverage.html


