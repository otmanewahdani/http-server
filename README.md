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
- CGI script execution through CGI/1.1 interface as defined by [RFC 3875](https://datatracker.ietf.org/doc/html/rfc3875)
- Cookies and session management
- Logs its operations in a log file (see [Demo](#demo) section)
- Hosts multiple websites
- Accepts direct uploads

## Configuration

## Setup
- It is a no brainer that if you want to use this server you will need a c++ compiler (preferrably gcc or clang).
- A Unix environment was used to develop and test this program, specifically Linux and Darwin for MacOs. So no guarantees that it will compile smoothly in any other environment. For Linux, Bash is needed. And for a Darwin-based Macos, Zsh works best with the building process.


## Usage
First in your terminal, clone and go to the base directory:
```bash
git clone https://github.com/otmanewahdani/http-server.git && cd http-server
```
- ### Compiling and running the demo programs
  The Makefile has rules for compiling 3 separate demo programs (which are the main.cpp's in each container's directory) that demonstrate the 3 containers: vector, stack and map. So for compiling and running the demo program, do the following :
  
  ```bash
  make map && cd map && ./map
  ```
  :bulb: The same procedure works for the other two containers, just substitute map with vector or stack.<br />
  
  :warning: The program output is long and unintelligible since it's only meant to be used by the testing script.<br />
  
- ### Running the tests
  Like it was explained in the features section, each container can be benchmarked and tested separately against the original STL containers. Thus make sure you are in the base directory (same location as the Makefile) and run the following to execute the tests:
  ```bash
    make test-map
  ```
  :bulb: if you want to test the other containers, just keep the same command and substitute map with the name of the container.<br />
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
