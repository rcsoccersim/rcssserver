# RoboCup Soccer Simulator Server
This image contains the RoboCup Soccer Simulator Server (rcssserver) installed and ready to use.

## Tags
- latest

## How to use this image

Run the following command to start the server:
```bash
docker run --name rcssserver --network host -it rcssserver:latest
```
Now you can connect to the server using the monitor or a client.

## How to use custom configuration files
1. You can mount a volume with your custom configuration files, like:
```bash
docker run --name rcssserver --network host -v /path/to/your/configs:/root/.rcssserver -it rcssserver:latest
```
2. You can run server with custom command line arguments, like:
```bash
docker run --name rcssserver --network host -it rcssserver:latest rcssserver server::synch_mode=true

```


**Note**: You can either use one of the above methods at a time. 
It means:
* if there is a server configuration file environment variable will be ignored. 
* if you use third method, the second method will be ignored.



## :incoming_envelope: Contributing

For bug reports, feature requests and latest updates, please goto
https://github.com/rcsoccersim/rcssserver and open an issue or a pull request.

> The RoboCup Soccer Server Maintainance Group
