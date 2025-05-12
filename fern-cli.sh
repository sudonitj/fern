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

# Function to open URL in browser
open_browser() {
    local url=$1
    
    # Determine the OS and use appropriate command
    case "$(uname -s)" in
        Darwin*)    # macOS
            open "$url"
            ;;
        Linux*)     # Linux
            # Try different browsers
            if command -v xdg-open &> /dev/null; then
                xdg-open "$url" &> /dev/null &
            elif command -v gnome-open &> /dev/null; then
                gnome-open "$url" &> /dev/null &
            elif command -v firefox &> /dev/null; then
                firefox "$url" &> /dev/null &
            elif command -v google-chrome &> /dev/null; then
                google-chrome "$url" &> /dev/null &
            elif command -v chromium-browser &> /dev/null; then
                chromium-browser "$url" &> /dev/null &
            else
                echo -e "${YELLOW}Unable to open browser automatically. Please open the URL manually: ${url}${NC}"
                return 1
            fi
            ;;
        CYGWIN*|MINGW*|MSYS*)  # Windows
            start "$url"
            ;;
        *)
            echo -e "${YELLOW}Unable to open browser automatically. Please open the URL manually: ${url}${NC}"
            return 1
            ;;
    esac
    
    return 0
}

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

# Check for required dependencies
check_dependencies() {
    # Check for emcc (Emscripten)
    if ! command -v emcc &> /dev/null; then
        echo -e "${RED}Error: Emscripten compiler (emcc) not found.${NC}"
        echo "Please install Emscripten: https://emscripten.org/docs/getting_started/downloads.html"
        exit 1
    fi
    
    # Check for Python (for HTTP server)
    if ! command -v python3 &> /dev/null; then
        echo -e "${RED}Error: Python 3 not found.${NC}"
        echo "Please install Python 3: https://www.python.org/downloads/"
        exit 1
    fi
}

# Create a default template.html if none exists
create_default_template() {
    cat > template.html << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fern Application</title>
    <style>
        body { margin: 0; background-color: #f0f0f0; font-family: Arial, sans-serif; }
        canvas { display: block; margin: 20px auto; border: 1px solid #ccc; }
        .container { max-width: 800px; margin: 0 auto; padding: 20px; }
        h1 { color: #333; text-align: center; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Fern Graphics</h1>
        {{{ CANVAS }}}
    </div>
</body>
</html>
EOF
    echo -e "${GREEN}Created default template.html${NC}"
}

# Create index.html in the dist directory for easier navigation
create_index_html() {
    local app_name=$1
    
    cat > dist/index.html << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fern Applications</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        h1 { color: #333; }
        a { color: #0066cc; text-decoration: none; }
        a:hover { text-decoration: underline; }
    </style>
</head>
<body>
    <h1>Fern Applications</h1>
    <p><a href="${app_name}.html">${app_name}</a></p>
</body>
</html>
EOF
    echo -e "${GREEN}Created index.html in dist directory${NC}"
}

run_interactive_server() {
    local source_file=$1
    local port=$2
    local output_base=$(basename "$source_file" .c)
    local url="http://localhost:${port}/${output_base}.html"
    local server_pid
    local original_dir=$(pwd) 

    # Navigate to dist directory
    cd dist
    
    # Start server in background
    python3 -m http.server ${port} &
    server_pid=$!
    
    # Open browser
    cd "$original_dir"
    open_browser "$url"
    
    echo -e "${GREEN}Server running at ${url}${NC}"
    echo -e "${YELLOW}Interactive mode enabled. Available commands:${NC}"
    echo "  r - Recompile and reload"
    echo "  c - Clear console"
    echo "  q - Quit server"
    echo "  h - Show this help"
    
    # Read input in a loop
    while true; do
        # Read a single character without requiring Enter
        read -n 1 -s cmd
        
        case "$cmd" in
            r|R)
                echo -e "${YELLOW}Recompiling...${NC}"
                cd "$original_dir"
                compile_app "$source_file"
                echo -e "${GREEN}Recompiled! Refresh your browser to see changes.${NC}"
                ;;
            c|C)
                clear
                echo -e "${GREEN}Server running at ${url}${NC}"
                echo -e "${YELLOW}Interactive mode. Press 'h' for help.${NC}"
                ;;
            q|Q)
                echo -e "${YELLOW}Shutting down server...${NC}"
                kill $server_pid
                exit 0
                ;;
            h|H)
                echo -e "${YELLOW}Available commands:${NC}"
                echo "  r - Recompile and reload"
                echo "  c - Clear console"
                echo "  q - Quit server"
                echo "  h - Show this help"
                ;;
            *)
                # Ignore other keystrokes
                ;;
        esac
    done
}

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
        echo -e "${YELLOW}Starting interactive development server on port ${PORT}...${NC}"
        run_interactive_server "$FILE" "$PORT"
    fi
}

# Run the script
main "$@"