# Use a base image with Emscripten installed
FROM trzeci/emscripten

# Set the working directory
WORKDIR /app

# Copy your project files into the container
COPY . .

# Build the project
RUN emcc main.c tetris.c -o tetris.html -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file DejaVuSansMono.ttf

# Install a web server (nginx is a great choice)
# The base image is based on Alpine Linux, so we use `apk`
RUN apk add --no-cache nginx

# Configure nginx to serve files from the /app directory
COPY nginx.conf /etc/nginx/nginx.conf

# Expose the default HTTP port
EXPOSE 80

# The command to run when the container starts
CMD ["nginx", "-g", "daemon off;"]