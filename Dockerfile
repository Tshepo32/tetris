# Use a base image with Emscripten installed
FROM trzeci/emscripten

# Set the working directory
WORKDIR /app

# Copy your project files into the container
COPY . .

# Build the project
RUN emcc main.c tetris.c -o tetris.html -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file DejaVuSansMono.ttf

# The publish directory will be /app