#!/bin/bash
# filepath: /home/rishi/graph_c/fern_cli.sh

set -e  # Exit on any error

# Define color codes for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check for dependencies
check_dependencies() {
    echo -e "${YELLOW}Checking dependencies...${NC}"
    
    if ! command -v emcc &> /dev/null; then
        echo -e "${RED}Error: emcc (Emscripten compiler) not found.${NC}"
        echo "Please install Emscripten: https://emscripten.org/docs/getting_started/downloads.html"
        exit 1
    fi
    
    if ! command -v python3 &> /dev/null; then
        echo -e "${RED}Error: python3 not found.${NC}"
        echo "Please install Python 3"
        exit 1
    fi
    
    echo -e "${GREEN}All dependencies found!${NC}"
}

# Try to open the browser automatically
open_browser() {
    local url=$1
    echo -e "${YELLOW}Attempting to open browser automatically...${NC}"
    
    if command -v xdg-open &> /dev/null; then
        xdg-open "$url" &> /dev/null &
    elif command -v open &> /dev/null; then
        open "$url" &> /dev/null &
    else
        echo -e "${YELLOW}Could not open browser automatically. Please open:${NC}"
        echo -e "${GREEN}$url${NC}"
    fi
}

# Compile and run the application
run_app() {
    local source_file=$1
    local output_base=$(basename "$source_file" .c)
    
    # Check if source file exists
    if [ ! -f "$source_file" ]; then
        echo -e "${RED}Error: $source_file not found in current directory.${NC}"
        exit 1
    fi
    
    # Check if template.html exists
    if [ ! -f "template.html" ]; then
        echo -e "${YELLOW}Warning: template.html not found. Creating a default template...${NC}"
        create_default_template
    fi
    
    # Create dist directory if it doesn't exist
    if [ ! -d "dist" ]; then
        echo -e "${YELLOW}Creating dist directory...${NC}"
        mkdir -p dist
    fi
    
    # Compile with emscripten - use the input filename for the output
    echo -e "${YELLOW}Compiling $source_file with Emscripten...${NC}"
    emcc "$source_file" -o "dist/${output_base}.html" --shell-file template.html -s WASM=1 \
         -s EXPORTED_RUNTIME_METHODS='["cwrap", "HEAPU8"]' -s ALLOW_MEMORY_GROWTH=1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Compilation successful!${NC}"
        
        # Create a simple index.html if it doesn't exist to help navigation
        if [ ! -f "dist/index.html" ] || [ "$output_base" != "index" ]; then
            create_index_html "$output_base"
        fi
        
        # Define the URL to open
        local url="http://localhost:8000/${output_base}.html"
        
        # Start HTTP server in the dist directory
        echo -e "${YELLOW}Starting HTTP server on port 8000...${NC}"
        echo -e "${GREEN}Access your application at: $url${NC}"
        echo -e "${YELLOW}Press Ctrl+C to stop the server${NC}"
        
        # Try to open the browser
        open_browser "$url"
        
        # Change to the dist directory and start the server
        (cd dist && python3 -m http.server)
    else
        echo -e "${RED}Compilation failed.${NC}"
        exit 1
    fi
}

# Create a simple index.html to list all compiled applications
create_index_html() {
    local current_app=$1
    
    cat > dist/index.html << EOF
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Fern Graphics Projects</title>
    <style>
        body {
            background-color: #222;
            color: #eee;
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            text-align: center;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            text-align: left;
            background: #333;
            padding: 20px;
            border-radius: 8px;
        }
        h1 { margin-bottom: 20px; }
        ul { list-style-type: none; padding: 0; }
        li { margin: 10px 0; padding: 10px; background: #444; border-radius: 4px; }
        li.current { background: #336633; }
        a { color: #8cf; text-decoration: none; display: block; }
        a:hover { text-decoration: underline; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Fern Graphics Projects</h1>
        <ul id="project-list">
            <li class="current"><a href="./${current_app}.html">▶️ ${current_app} (current)</a></li>
            <!-- Other projects will be listed here by JavaScript -->
        </ul>
    </div>
    <script>
        // List all HTML files in the directory
        fetch('./')
            .then(response => response.text())
            .then(text => {
                const parser = new DOMParser();
                const doc = parser.parseFromString(text, 'text/html');
                const links = Array.from(doc.querySelectorAll('a'));
                
                const htmlFiles = links
                    .map(link => link.getAttribute('href'))
                    .filter(href => href.endsWith('.html') && href !== 'index.html' && href !== '${current_app}.html');
                
                const projectList = document.getElementById('project-list');
                
                htmlFiles.forEach(file => {
                    const li = document.createElement('li');
                    const a = document.createElement('a');
                    a.href = './' + file;
                    a.textContent = file.replace('.html', '');
                    li.appendChild(a);
                    projectList.appendChild(li);
                });
            });
    </script>
</body>
</html>
EOF
}

# Create a default template file if none exists
create_default_template() {
    cat > template.html << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Fern Graphics Application</title>
    <style>
        body { 
            background-color: #222; 
            color: #eee;
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            text-align: center;
        }
        canvas.emscripten { 
            border: 1px solid #444;
            margin: 0 auto;
            display: block;
            background-color: black;
        }
        h1 { margin-bottom: 20px; }
        a.home-link {
            position: absolute;
            top: 10px;
            left: 10px;
            color: #8cf;
            text-decoration: none;
        }
    </style>
</head>
<body>
    <a href="./index.html" class="home-link">← All Projects</a>
    <h1>Fern Graphics Demo</h1>
    <canvas id="canvas" class="emscripten" width="1200" height="600"></canvas>
    <script>
      var Module = {
        canvas: document.getElementById('canvas')
      };
    </script>
    {{{ SCRIPT }}}
</body>
</html>
EOF
    echo -e "${GREEN}Created default template.html${NC}"
}

# Main function
main() {
    check_dependencies
    
    # If a specific file is provided, use that
    if [ -n "$1" ]; then
        # Check if it's a valid .c file
        if [[ $1 == *.c ]] && [ -f "$1" ]; then
            run_app "$1"
        else
            echo -e "${RED}Error: $1 is not a valid C source file.${NC}"
            exit 1
        fi
    else
        # No argument provided, look for default files
        if [ -f "main.c" ]; then
            run_app "main.c"
        elif [ -f "example.c" ]; then
            run_app "example.c"
        else
            echo -e "${RED}Error: No C source file specified and no default files (main.c or example.c) found.${NC}"
            exit 1
        fi
    fi
}

# Run the script
main "$@"