# RoboCup Soccer Server Simulator

[![CircleCI](https://circleci.com/gh/rcsoccersim/rcssserver/tree/master.svg?style=svg)](https://circleci.com/gh/rcsoccersim/rcssserver/tree/master)
![License](https://img.shields.io/github/license/rcsoccersim/rcssserver.svg)

![image](https://user-images.githubusercontent.com/1832537/49242985-f69a3c00-f3ea-11e8-97f5-9b0bfdfc4e1c.png)

The RoboCup Soccer Simulator Server (rcssserver) is a research and educational tool for multi-agent systems and artificial intelligence. It allows 11 simulated autonomous robotic players to play soccer (football).

## Quick Start

Make sure you have the required dependencies installed on your system:

- g++
- make
- boost
- bison
- flex

Download the latest rcssserver release in the [releases section](https://github.com/rcsoccersim/rcssserver/releases). Extract it and from the rcssserver directory execute:

```bash
./configure
make
```

This will build the necessary binaries to get you up and running.

`rcssserver/src/rcssserver` is the binary for the simulator server. The simulator
server manages the actual simulation and comunicates with client programs that
control the simulated robots.  To be able to run, the binary needs to find shared
libraries which are created when you build rcssserver.  This means you must either
install the server (make install) or run it from `rcssserver/src`.

A sample client can be found at `rcssserver/src/rcssclient`.

To see what is actually happening in the simulator, you will need to start a
simulator monitor, which needs to be installed separately ([rcssmonitor](https://github.com/rcsoccersim/rcssmonitor),
  rcssmonitor_classic, [soccerwindow2](https://osdn.net/projects/rctools/releases/p4886)
  or any other third party monitor).

To playback games that that you have recorded or downloaded, you will need to
start the log player such as [rcsslogplayer](https://github.com/rcsoccersim/rcsslogplayer),
which must also be downloaded separately.

## Configuring

Before you can build The RoboCup Soccer Simulator Server you will need to run
the `configure` script located in the root of the distribution directory.

The default configuration will set up to install the server components in the
following location:

`/usr/local/bin`        for the executables

You may need administrator privileges to install the server into the default
location.  This locations can be modified by using configure's `--prefix=DIR`
and related options.  See `configure --help` for more details.

The server has several features that can be enabled or disabled at configure time
by using the `--enable-FEATURE[=ARG]` or `--disable-FEATURE` parameters to
`configure`.  `--disable-FEATURE` is equivalent to `--enable-FEATURE=no` and
`--enable-FEATURE` is equivalent to `--enable-FEATURE=yes`.  The only valid values
for `ARG` are `yes` and `no`.

`--enable-fast_scanner=yes` will enable a fast building but (very) large
scanner for the coach language.  You will need to have `lex` or `flex` installed
and you will need to manually remove the `coach_lang_tok.cc` file in the
`rcssserver/src` directory. This is disabled by default. I found the actual
speed of the parser show only minimal improvement when using this option on my
system, but this may not be true on your system.  All I can suggest is to test it
on your system and decide for yourself if the speed increase justifies the
increase in size of the executable.

`--enable-rcssclient=yes` will enable the building of rcssclient, a sample
client program.  This is enabled by default.

`--enable-debug=yes` will enable the building of the modules with debugging
information.  This is disabled by default.


## Building

Once you have successfully configured the server, simply run `make` to build
the sources.

## Installing

When you have completed building the server, its components can be installed
into their default locations or the locations specified during configuring by
running `make install`.  Depending on where you are installing the
server, you may need special permissions.

## Uninstalling

The server can also be easily removed by entering the distribution directory and
running `make uninstall`.  This will remove all the files that where installed,
but not any directories that were created during the installation process.

## Using the Server

To start only the server either type `./rcssserver` from the directory 
containing the executable or `rcssserver` if you installed the executables 
in your PATH. rcssserver will look in your home directory for the configuration files:

```bash
~/.rcssserver/server.conf
~/.rcssserver/player.conf
~/.rcssserver-landmark.xml # (optional)
```

If these files do not exist they will be created and populated with default values.

To start the sample client, type `./rcssclient` or `rcssclient` as above.  Then type 
`(init sample)`.  This will connect the sample client to the server.  You can then 
type in client command to move the client around the field. You will also need a 
monitor to be able to see whats happening on the field.

If you installed the server and the monitor successfully, you can use the
`rcsoccersim` script. To start the simulator (server and monitor) either type:

`rcsoccersim`


## Contributing

For bug reports, feature requests and latest updates, please open an issue or a pull request.

> The RoboCup Soccer Server Maintainance Group
