# Use a modern Emscripten image (actively maintained)
FROM emscripten/emsdk:latest

# Set the working directory
WORKDIR /app

# Copy your project files into the container
COPY . .

# Build the project
RUN emcc main.c tetris.c -o tetris.html \
    -s USE_SDL=2 \
    -s USE_SDL_TTF=2 \
    --preload-file DejaVuSansMono.ttf

# Install nginx
RUN apt-get update && apt-get install -y nginx && rm -rf /var/lib/apt/lists/*

# Configure nginx to serve files from the /app directory
COPY nginx.conf /etc/nginx/nginx.conf

# Expose the default HTTP port
EXPOSE 80

# Start nginx
CMD ["nginx", "-g", "daemon off;"]
