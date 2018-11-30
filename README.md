# Introduction

The RoboCup Soccer Simulator Server (**rcssserver**) is a research and educational
tool for mutli-agent systems and artificial intelligence. It allows 11
simulated autonomous robotic players to play soccer (**football**).

## Quick Start

From the **rcssserver** directory execute:

	./configure
	make

This will built the necessary binaries to get you up and running.

	rcssserver/src/rcssserver
	
 Is the binary for the simulator server. The simulator server manages the actual simulation and communicates with client programs that control the simulated robots.  To be able to run, the binary needs to find shared libraries which created when you build **rcssserver**.  This means you must either install the server (make install) or run it from **rcssserver/src**.

A sample client can be found at **rcssserver/src/rcssclient**.

To see what is actually happening in the simulator, you will need to
start a simulator monitor, which needs to be installed separately
(**rcssmonitor**, **rcssmonitor_classic** or any other **third party monitor**).

To playback games that that you have recorded or downloaded, you will need to
start the log player such as **rcsslogplayer**, which must also be
downloaded separately.

## Configuring

Before you can build **The RoboCup Soccer Simulator Server** you will need to run
the **configure** script located in the root of the distribution directory.

The default configuration will setup to install the server components in
the following location:

	/usr/local/bin		# for the executables

You **may need administrator privilages** to install the server into the default
location.  This locations can be modified by using configure's `--prefix=DIR`
and related options.  See `configure --help` for more details.

The server has several features that can be enabled or disabled at
configure time by using the `--enable-FEATURE[=ARG]` or `--disable-FEATURE`
parameters to `configure`.  `--disable-FEATURE` is equivalent to
`--enable-FEATURE=no` and `--enable-FEATURE` is equivalent to
`--enable-FEATURE=yes`.  The only valid values for `ARG` are `yes` and `no`.

`--enable-fast_scanner=yes` will enable the building a fast but (very) large
scanner for the coach language.  You will need to have `lex` or `flex`
installed and you will need to manually remove the `coach_lang_tok.cc` file
in the `rcssserver/src` directory. This is disabled by default. I found the actual speed of the parser show only minimal improvement when using this option on my system, but this may not be so on your system.  All I can suggest is to test it on your system and decide for yourself if the speed increase justifies the increase in size of the executable.

`--enable-rcssclient=yes` will enable the building of **rcssclient**, a sample
client program.  This is enabled by **default**.

`--enable-debug=yes` will enable the building of the modules with debugging
information.  This is disabled by **default**.

## Building

Once you have successfully configured the server, simply run `make` to build
the sources.


## Installing

When you have completed building the server, it's components can be installed
into their default locations or the locations specified during configuring by
running `make install`.  Depending on where you are installing the
server, **you may need special permissions**.

## Uninstalling

The server can also be easily removed by entering the distribution
directory and running `make uninstall`.  This **will remove all the files** that
where installed, **but not any directories** that were created during the
installation process.

## Using the server

To **start only the server** either type

`./rcssserver`

From the directory containing the executable or

`rcssserver`

if you installed the executables in your **PATH**.  rcssserver will look in your
home directory for the configuration files:

	~/.rcssserver/server.conf
	~/.rcssserver/palyer.conf
	~/.rcssserver-landmark.xml 	# optional

If these files do not exist they will be created and populated with default
values.

To start the sample client, type:

`./rcssclient`
or
`rcssclient`

As above.  Then type `(init sample)`.  This will connect the sample client to the server.  You can then type in client command to move the client around the field.  You will also need a monitor to be able to see whats happening on the field.

If you installed the server and the monitor successfully, you can use the `rcsoccersim` script. To **start the simulator** (server and monitor) either type:

  `rcsoccersim`

## Run  Rcssserver w/ Docker Container (Ubuntu 12.04)

First, create a network to connect all containers (We use this version of Ubuntu, because its default for simulation 2d competition):

```
docker network create mynet
```
  
You need to make the Display exportable   
 
	xhost +local:root

Inside this **GitHub** repository, build the **Dockerfile** in terminal:

	docker build . -t "rcssserver"

Run the **rcssserver** client:

	docker run -it --rm --net mynet --name master rcssserver

Inside **docker image**, type to run client:
	
	rcssserver

You need to run this docker first, and after [rcssmonitor](https://github.com/rcsoccersim/rcssmonitor), [rcsslogplayer](https://github.com/rcsoccersim/rcsslogplayer) and **create each team container** to run separately.

## Making Contact

For **bug reports**, feature requests and latest updates, please go to:
	http://sourceforge.net/projects/sserver/
	
Or **email bugs** to:
	sserver-bugs@lists.sourceforge.net:

The RoboCup Soccer Server Maintenance Group:
	sserver-admin@lists.sourceforge.net
