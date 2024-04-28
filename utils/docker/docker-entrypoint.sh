#!/bin/env bash

SERVER_CONFIG_FOLDER='/root/.rcssserver'
LOGS_FOLDER='/root/rcssserver-logs'

mkdir -p $LOGS_FOLDER
cd $LOGS_FOLDER

rcssserver 


