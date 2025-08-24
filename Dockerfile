# Use a base image with Emscripten and Node.js installed
# The emscripten image doesn't include node, so we'll use a more flexible base.
FROM node:18-alpine

# Install Emscripten using the emsdk
RUN apk add --no-cache git python3
RUN git clone https://github.com/emscripten-core/emsdk.git
RUN emsdk/emsdk install latest
RUN emsdk/emsdk activate latest
ENV PATH="/emsdk:$PATH"

# Set the working directory
WORKDIR /app

# Copy your project files into the container
COPY . .

# Build the project
RUN emcc main.c tetris.c -o tetris.html -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file DejaVuSansMono.ttf

# Install http-server
RUN npm install -g http-server

# Expose the port the server will run on
EXPOSE 8080

# The command to run when the container starts
CMD ["http-server", "."]