#!/bin/bash
# Fern Graphics CLI - A tool for compiling and serving WebAssembly graphics applications

VERSION="1.0.1"
PORT=8000

# Define color codes for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Display help information
show_help() {
    echo -e "${BLUE}Fern Graphics CLI v${VERSION}${NC}"
    echo "A tool for compiling and serving WebAssembly graphics applications"
    echo ""
    echo "Usage:"
    echo "  fern [options] [file.c]"
    echo ""
    echo "Options:"
    echo "  --help              Show this help information"
    echo "  --version           Display version information"
    echo "  --serve             Compile and serve (default if no option specified)"
    echo "  --build             Only compile, don't start server"
    echo "  --port <number>     Use custom port for server (default: 8000)"
    echo ""
    echo "Examples:"
    echo "  fern example.c                 # Compile and serve example.c"
    echo "  fern --build main.c            # Just compile main.c without serving"
    echo "  fern --port 9000 example.c     # Use port 9000 for the server"
    echo ""
}

# Display version information
show_version() {
    echo -e "${BLUE}Fern Graphics CLI v${VERSION}${NC}"
}

# Rest of your existing functions like check_dependencies(), etc.
# ...

# Parse command line arguments
parse_args() {
    SERVE=true
    FILE=""
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --help)
                show_help
                exit 0
                ;;
            --version)
                show_version
                exit 0
                ;;
            --serve)
                SERVE=true
                shift
                ;;
            --build)
                SERVE=false
                shift
                ;;
            --port)
                if [[ -z "$2" || "$2" =~ ^- ]]; then
                    echo -e "${RED}Error: Port number is required for --port option${NC}"
                    exit 1
                fi
                PORT="$2"
                shift 2
                ;;
            --port=*)
                PORT="${1#*=}"
                shift
                ;;
            -*)
                echo -e "${RED}Error: Unknown option $1${NC}"
                show_help
                exit 1
                ;;
            *)
                if [[ -z "$FILE" ]]; then
                    FILE="$1"
                    shift
                else
                    echo -e "${RED}Error: Too many arguments. Only one source file can be specified.${NC}"
                    show_help
                    exit 1
                fi
                ;;
        esac
    done
    
    # If no file specified, check for defaults
    if [[ -z "$FILE" ]]; then
        if [[ -f "main.c" ]]; then
            FILE="main.c"
        elif [[ -f "example.c" ]]; then
            FILE="example.c"
        else
            echo -e "${RED}Error: No C source file specified and no default files (main.c or example.c) found.${NC}"
            exit 1
        fi
    fi
}

# Updated compile function
compile_app() {
    local source_file=$1
    local output_base=$(basename "$source_file" .c)
    
    # Check if source file exists
    if [[ ! -f "$source_file" ]]; then
        echo -e "${RED}Error: $source_file not found in current directory.${NC}"
        exit 1
    fi
    
    # Check if template.html exists
    if [[ ! -f "template.html" ]]; then
        echo -e "${YELLOW}Warning: template.html not found. Creating a default template...${NC}"
        create_default_template
    fi
    
    # Create dist directory if it doesn't exist
    if [[ ! -d "dist" ]]; then
        echo -e "${YELLOW}Creating dist directory...${NC}"
        mkdir -p dist
    fi
    
    # Compile with emscripten
    echo -e "${YELLOW}Compiling $source_file with Emscripten...${NC}"
    emcc "$source_file" -o "dist/${output_base}.html" --shell-file template.html -s WASM=1 \
         -s EXPORTED_RUNTIME_METHODS='["cwrap", "HEAPU8"]' -s ALLOW_MEMORY_GROWTH=1
    
    if [[ $? -eq 0 ]]; then
        echo -e "${GREEN}Compilation successful!${NC}"
        
        # Create a simple index.html if it doesn't exist to help navigation
        if [[ ! -f "dist/index.html" ]] || [[ "$output_base" != "index" ]]; then
            create_index_html "$output_base"
        fi
        
        echo -e "${GREEN}Output: dist/${output_base}.html${NC}"
        return 0
    else
        echo -e "${RED}Compilation failed.${NC}"
        exit 1
    fi
}

# Updated main function
main() {
    check_dependencies
    parse_args "$@"
    
    # Compile the application
    compile_app "$FILE"
    
    # Start server if --serve (default) was specified
    if [[ "$SERVE" == true ]]; then
        # Define the URL to open
        local output_base=$(basename "$FILE" .c)
        local url="http://localhost:${PORT}/${output_base}.html"
        
        # Start HTTP server in the dist directory
        echo -e "${YELLOW}Starting HTTP server on port ${PORT}...${NC}"
        echo -e "${GREEN}Access your application at: $url${NC}"
        echo -e "${YELLOW}Press Ctrl+C to stop the server${NC}"
        
        # Try to open the browser
        open_browser "$url"
        
        # Change to the dist directory and start the server
        (cd dist && python3 -m http.server ${PORT})
    fi
}

# Run the script
main "$@"