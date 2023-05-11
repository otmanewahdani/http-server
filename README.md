# HTTP-Server
> An HTTP server built from scratch using C++. This is a group project that was completed by @otmanewahdani and @flouta.

## Table of Contents
* [Overview](#overview)
* [Features](#features)
* [Setup](#setup)
* [Configuration](#configuration)
* [Usage](#usage)
* [Demo](#demo)
* [Project Status](#project-status)
* [Acknowledgements](#acknowledgements)

## Overview
- This web server implements HTTP/1.1 as described by [RFC 2616](https://www.rfc-editor.org/rfc/rfc2616). All features were developed using C++98 and the STL library. No boost or any external library was used.
- This server can manage multiple connections and requests at once without using any concurrent programming whatoever. Everything is managed by a single thread that makes use of I/O multiplexing.
- For networking, it uses socket APIs provided by UNIX systems.

## Features
- GET, POST, DELETE methods
- Can be configured with a configuration file (see [Configuration](#configuration) section)
- CGI script execution through CGI/1.1 interface as defined by [RFC 3875](https://datatracker.ietf.org/doc/html/rfc3875) without query string support
- CGI timeout (it is a useful in that some faulty scripts can waste the server's resources by running for too long)
- Cookies and session management
- Logs its operations in a log file (see [Demo](#demo) section)
- Hosts multiple websites
- Autoindex (directory listing)
- Accepts direct uploads

## Configuration
- You can create a configuration file and pass it as the first argument to the program or change [defaultConfigFileName in Config.cpp in line 9](srcs/Config/Config.cpp) so that the program can directly pick your default config file without passing it as the first argument.
- There are 2 contexts in the configuration file grammar:
  1. #### Server Context
  Since this http server supports hosting multiple websites, multiple servers can be setup to create a customized configuration for each website. Even the same website or service can be handled with different servers using different parameters. These parameters can be tuned using these following keywords:
  
    - server: this is how you open a server's context. All the keywords that will be listed next can only to be included within this context and need to be surrounded by an opening curly brace and a closing one. Anything outside this braced scoping doesn't belong to that specified context.
    - server_name: specifies the host name of the server. It's the same as the Host header fieald in an http request.
    - listen: directs the server to listen on a specific ip address and port using this notation (address:port). If the address or/and port are ommitted, it listens by default respectively on 0.0.0.0 or/and 8080.
    - client_body_size_max: limits the client's body to a max number of bytes that can be sent with a POST request. a 0 value or if ommitted, no limit is applied. A 413 status code is retuned in case, a client's request body surpasses this limit.
    - error_pages: web pages or files along with status codes can be set to be returned when an error happens. For example, when a 404 error is detected, the server will search for a 404 page that is set to be returned in case of a 404 error, if it finds one it will include it in the response.
   
  2. #### Location Context
  This is a nested context within the server context. Like the server context's keywords, the location context cannot exists outside of the server's context. The location scope specifies parameters for URL routes. URLs for the same server can have different configurations depending on which location context they fall under. Here is its grammar:
  
  - location: This is the keyword the declares the location context. It must be followed by the configured route and an open curly brace and just as the server context, all locations parameters will be terminated by a closing brace. if the route argument is a directory, the configuration will be applied to all URLs that are subpaths of this route. If the route is a specific file, then the parameters will apply to URL only. The keywords that come next can only be used within this context.
  - allow_methods: HTTP request methods that are allowed. if not specified, GET is enabled by default.
  - redirect: redirects all requests to the specified URL along with the specified status code. Only redirection status codes are allowed.
  - root: if defined, the root path is the actual path of the location's route on the server's filesystem. if root is set as /path/to/website, location path is set to / and the URL is /index.html, Then the server replaces '/' by '/path/to/website' and the URL becomes '/path/to/website/index.html'. If no root is set, then the current working directory is the root.
  - default: specifies a file to be served if the URL requests a directory.
  - autoindex: enables directory listing if the requested URL is a directory and autoindex is set to 'on'. if set to 'off' or not specified, then it is not enabled.
  - cgi: specifies a cgi script's extension (like .php) and an executable path (e.g /bin/php) to be used for running scripts with that extension.
  - upload: defines an absolute path of a directory where uploads will be saved. Uploads will be automatically triggered if the requested method is POST, the URL is a directory and upload is enabled for that URL's location.
 
  ##### configuration example (this example is not realistic. It is only meant to showcase the syntax)
```
  server {   
      server_name example.com;
      listen localhost:8080;
      error_pages 404 /404.html;
      error_pages 500 /404.html;    
      location / {
          allow_methods GET POST DELETE;
          root /path/to/real/directory;
          redirect 301 http://example.com;
          default /index.html;
          autoindex on;
          cgi .pl /bin/perl;
          cgi .py /bin/python3;      
    }    
```

## Setup
- It is a no brainer that if you want to use this server you will need a c++ compiler (preferrably gcc or clang).
- A Unix environment was used to develop and test this program, specifically Linux and Darwin for MacOs. So no guarantees that it will compile smoothly in any other environment. For Linux, Bash is needed. And for a Darwin-based Macos, Zsh works best with the building process.


## Usage
First in your terminal, clone and go to the base directory:
```bash
git clone https://github.com/otmanewahdani/http-server.git && cd http-server
```
Write your configuration file according to the [configuration section](#configuration) and run:
```bash
make
```
after that run the web server like this:
```bash
./http-server [config-file]
```
if you wish to run it without passing the config file as the first argument, see the introduction to the [configuration](#configuration)

Now go to a browser and type in the ip address and port you set up in your configuration file, followed by a valid URL that belongs to your configured [location](#location-context)
 
 ```
 ip-address:port/URL
 ```
  #### Illustration
  <img src="./.img/container testing illustration.gif" alt="Illustration of how container tests are run" width="600" height="100" />
- ### Using the Library in your own program
  1. #### Including the library's header files
      First, in your preprocessor include directive use only the name of the header file like this:
      ```c++
      #include <vector.hpp>
      #include <map.hpp>
      #include <stack.hpp>
      #include <type_traits.hpp>
      #include <utility.hpp>
      #include <iterator.hpp>
      #include <algorithm.hpp>
      ```
      :bulb: It's not necessary to include all of the header files, just include the ones you need.<br />
      
      Then, specify the path to ft_containers in any way you want (I included an example on how you might wanna do it, just replace parent-directories with the file path leading to ft_containers) and supplement the following flags to your compiler:
      ```bash
      export FT_CONTAINERS_PATH="parent-directories/ft_containers"
      export FT_CONTAINERS_INCS="-I$FT_CONTAINERS_PATH/vector/incs -I$FT_CONTAINERS_PATH/map/incs -I$FT_CONTAINERS_PATH/stack/incs -I$FT_CONTAINERS_PATH/algorithm -I$FT_CONTAINERS_PATH/iterator -I$FT_CONTAINERS_PATH/utility -I$FT_CONTAINERS_PATH/type_traits"
      
      <your-compiler> <your-program-source-files> $FT_CONTAINERS_INCS
      ```
      :bulb: Again you don't need to use all the flags. Only use what you need<br />
  2. #### Using the containers in your code
      They will be used the same way as the STL containers with only a slight difference. Since the standard containers are defined in the std namespace, std is prepended along with the scope resolution operator to the name of the container like this:
      ```c++
      std::vector<int> emptyVector;
      ```
      But for this library ft needs to be prepended to all type names like the following:
      ```c++
      ft::map<int, char*> emptyMap;
      ```

## Project Status
Project is: _complete_


## Acknowledgements
This project is part of our 1337 school (42 network) curriculum. So We would like to thank our school for giving us the opportunity to work on a project that taught us a lot of new things.
