# MBHS - Madi's Basic HTTP Server

MBHS is a lightweight, easily deployable HTTP server designed for simplicity and speed. It's perfect for serving static content or for use in development environments where a full-featured web server is not necessary. It requires almost zero setup.

## Features

- **Lightweight and Fast**: MBHS is designed to be minimal and efficient, making it ideal for quick deployments.
- **Configurable**: Easily configure the server through a simple configuration file to suit your needs.
- **MHTML**: (Madi's) HTML is my experiment on adding unnecessary bloat to HTML. You can safely ignore this, but I will keep adding new features to make my own life easier.

## Getting Started

### Prerequisites

- A modern C++ compiler (C++11 or later)
- CMake (version 3.10 or later)

### Building the Server

1. Clone the repository to your local machine:

```sh
git clone https://github.com/Madiwka4/MBHS.git
cd MBHS
```

2. Create a build directory and navigate into it:
```sh
mkdir build && cd build
```

3. Run CMake to configure and build the project.
```sh
cmake ..
```

4. Run make to compile MBHS
```sh
make 
```

### Configuration

MBHS can be configured using a simple text file. For now the following options are available:

- `port`: The port number on which the server will listen.
- `path`: The *local* path to the directory from which the files are served.
- `htmlmode (true|false)`: Whether to use HTML or MHTML by default.

Example `config.txt`:
```
port=8080
path=tmp
htmlmode=true
```

### Running the Server
After building, you can start the server with the following command:
```sh
./MBHS [config file path]
```
If no configuration file is specified, the program will look for a `mconfig.txt` in the current directory.

### MHTML
For now Madi's HTML is literally just html, but with one addded feature to test if it works: the `<hlink>` tag.
The `<hlink>` tag has the following syntax:
```html
<hlink> Google {{google.com}}</hlink>
```
and will effectively change into
```html
<a href="google.com" style="text-decoration:none;"> Google </a>
```
Any class assignments or other arguments will also be carried over.

### Contributing
You're welcome to contribute, but why would you? This project is nothing more than an experiment. However, if you are kind enough to fix my spaghetti code, you're welcome!