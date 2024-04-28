#------------------------------------------------------
# build stage
#------------------------------------------------------

FROM ubuntu:24.04 AS BUILD

WORKDIR /rcssserver

# set environment variables
ENV config_file=server.conf \
    DATE_FORMAT="%Y%m%d%H%M%S" \
    VERSION=18.1.3


# install dependencies
RUN apt-get clean && apt-get update --allow-insecure-repositories && \
    DEBIAN_FRONTEND="noninteractive" apt-get -y install \
    tzdata \
    sudo \
    gcc \
    g++ \
    wget \
    flex \
    bison \
    libboost-all-dev \
    automake \
    make \
    cmake \
    iputils-ping

# copy rcssserver source code
#COPY [ "./src", "./rcss", "./m4", "config*","Make*" , "CMake*", "/rcssserver/"]
COPY . /rcssserver/

# make and install rcssserver
RUN cd /rcssserver/ \
    && ./bootstrap \
    && ./configure --prefix=`pwd`/server-bin \
    && make \
    && make install \
    && ldconfig




#------------------------------------------------------
# run stage
#------------------------------------------------------
FROM ubuntu:24.04 AS RUN

ENV LD_LIBRARY_PATH=/app/server-bin/lib:/usr/local/lib:/usr/lib:/lib \
    PATH=/app/server-bin/bin:$PATH 

WORKDIR /app

COPY --from=BUILD /rcssserver/server-bin /app/server-bin

COPY utils/docker/docker-entrypoint.sh /app/docker-entrypoint.sh

CMD [ "bash", "/app/docker-entrypoint.sh" ]