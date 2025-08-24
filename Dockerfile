# Use a base image with Node.js and a package manager
FROM node:18-alpine

# Install build dependencies and clone the emsdk repository
RUN apk add --no-cache git python3

# Use a single RUN command to install, activate, and add emcc to the PATH
RUN git clone https://github.com/emscripten-core/emsdk.git \
    && cd emsdk \
    && ./emsdk install latest \
    && ./emsdk activate latest \
    && . /emsdk/emsdk_env.sh

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