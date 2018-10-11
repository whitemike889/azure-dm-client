#### How to set up development environment

Be sure you have CMAKE configured:

* Install [CMake](https://cmake.org/download/). 
* Make sure it is in your PATH by typing cmake -version from a command prompt. CMake will be used to create Visual Studio projects to build libraries and samples. 
* Make sure your CMake version is at least 3.6.1.

Be sure you have PERL configured:

* Install [perl](https://www.perl.org/get.html). You can use either ActivePerl or Strawberry Pearl. Run the installer as administrator to avoid issues.
    
Be sure you are using Visual Studio 2017 with Visual C++ (this last bit is important!)

#### Build binaries for x86, ARM and X64

    Start a VS 2017 developer command prompt
    cd <repo>
    build.all.cmd
 
 * Building only azure-c-sdk, run build.azure-c-sdk.cmd
 * Building only limpet, run build.azure-dm.cmd

