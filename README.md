# RoboCup Soccer Server Simulator

[![CircleCI](https://circleci.com/gh/rcsoccersim/rcssserver/tree/master.svg?style=svg)](https://circleci.com/gh/rcsoccersim/rcssserver/tree/master)
![License](https://img.shields.io/github/license/rcsoccersim/rcssserver.svg)

![image](https://user-images.githubusercontent.com/1832537/49242985-f69a3c00-f3ea-11e8-97f5-9b0bfdfc4e1c.png)

The RoboCup Soccer Simulator Server (rcssserver) is a research and educational tool for multi-agent systems and artificial intelligence. It allows 11 simulated autonomous robotic players to play soccer (football).

For further reading, please check [the user's manual](https://rcsoccersim.readthedocs.io/).

## :soccer: Quick Start

rcssserver is implemented by C++14 and depends some libraries.
Make sure you have the required dependencies installed on your system:

- g++ (which supports C++14)
- autoconf
- automake
- libtool
- flex
- bison
- boost >= 1.44

In the case of Ubuntu 18.04 or 20.04, the following commands will resolve all dependencies:

```
sudo apt update
sudo apt install build-essential automake autoconf libtool flex bison libboost-all-dev
```

Then, download the latest rcssserver tarball from the [releases section](https://github.com/rcsoccersim/rcssserver/releases).
Extract it and from the rcssserver directory execute:

```bash
tar xzvfp rcssserver-x.x.x.tar.gz
cd rcssserver-x.x.x
./configure
make
```

This will build the necessary binaries to get you up and running.
`rcssserver/src/rcssserver` is the binary for the simulator server.

The simulator server manages the actual simulation and comunicates with client programs that
control the simulated robots.
To be able to run, the binary needs to find shared libraries which are created when you build rcssserver.
This means you must either install the server (make install) or run it from `rcssserver/src`.

A sample client can be found at `rcssserver/src/rcssclient`.

To see what is actually happening in the simulator, you will need to start a
simulator monitor, which needs to be installed separately ([rcssmonitor](https://github.com/rcsoccersim/rcssmonitor), or any other third party monitor).

To playback games that you have recorded or downloaded, you will need to start the log player.
[rcssmonitor](https://github.com/rcsoccersim/rcssmonitor) can be used for this purpose.

The version 17.0.0 or later support [CMake](https://cmake.org/).
If CMake is prefered or problems with the above procedure, try the following commands at the top of the project directory:

```bash
cd rcssserver-x.x.x
mkdir build
cd build
cmake ..
make
```

## :gear: Configuring

Before building rcssserver, you will need to run the `configure` script located in the root of the distribution directory.
The default configuration will set up to install the server components in the
following location:

`/usr/local/bin`        for the executables

You may need administrator privileges to install the server into the default
location.  This locations can be modified by using configure's `--prefix=DIR`
and related options.  See `configure --help` for more details.
```bash
./configure --prefix=YOUR_INSTALLATION_DIR
```

The server has several features that can be enabled or disabled at configure time
by using the `--enable-FEATURE[=ARG]` or `--disable-FEATURE` parameters to
`configure`.  `--disable-FEATURE` is equivalent to `--enable-FEATURE=no` and
`--enable-FEATURE` is equivalent to `--enable-FEATURE=yes`.  The only valid values
for `ARG` are `yes` and `no`.

`--enable-fast_scanner=yes` will enable a fast building but (very) large scanner for the coach language.
You will need to have `flex` installed and you will need to manually remove the `coach_lang_tok.cpp` file in the `rcssserver/rcss/clang` directory.
This is disabled by default.
I found the actual speed of the parser show only minimal improvement when using this option on my system, but this may not be true on your system.
All I can suggest is to test it on your system and decide for yourself if the speed increase justifies the increase in size of the executable.

`--enable-rcssclient=yes` will enable the building of rcssclient, a sample
client program.  This is enabled by default.

`--enable-debug=yes` will enable the building of the modules with debugging
information.  This is disabled by default.

Once you have successfully configured the monitor, simply run `make` to build the sources.

If CMake is chosen, `ccmake` command is available for the configuration:
```bash
cd build
ccmake ..
```

## :hammer_and_wrench: Building

Once you have successfully configured the server, simply run `make` to build
the sources.

## :package: Installing

When you have completed building the server, its components can be installed
into their default locations or the locations specified during configuring by
running
```bash
make install
```
Depending on where you are installing the server, you may need special permissions.

## :wastebasket: Uninstalling

The server can also be easily removed by entering the distribution directory and
running
```bash
make uninstall
```

This will remove all the files that where installed,
but not any directories that were created during the installation process.

In the case of CMake, find `install_manifest.txt` under the build directory, then execute:
```bash
xargs rm < install_manifest.txt
```

## :arrow_forward: Using the Server

To start only the server either type `./rcssserver` from the directory
containing the executable or `rcssserver` if you installed the executables
in your PATH.
```bash
rcssserver
```
rcssserver will look in your home directory for the configuration files:

- ~/.rcssserver/server.conf
- ~/.rcssserver/player.conf
- ~/.rcssserver/CSVSaver.conf
- ~/.rcssserver-landmark.xml  (optional)

If these files do not exist they will be created and populated with default values.

To start the sample client, type `./rcssclient` or `rcssclient` as above.  Then type
`(init sample)`.  This will connect the sample client to the server.  You can then
type in client command to move the client around the field. You will also need a
monitor to be able to see whats happening on the field.

If you installed the server and the monitor successfully, you can use the
`rcsoccersim` script. To start the simulator (server and monitor) either type:

```bash
rcsoccersim
```

## :incoming_envelope: Contributing

For bug reports, feature requests and latest updates, please goto
https://github.com/rcsoccersim/rcssserver and open an issue or a pull request.

> The RoboCup Soccer Server Maintainance Group
