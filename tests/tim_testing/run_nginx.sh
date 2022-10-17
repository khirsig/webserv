#!/usr/bin/env bash

CONTAINER_NAME="tester_nginx"
LOG_FOLDER="log_nginx"
PORT="9001"

if ! docker info >/dev/null 2>&1;
then
    echo "This script uses docker, and it isn't running - please start docker and try again!"
    exit 1
fi

if [[ $1 == "clean" ]];
then
    echo "cleaning..."
    docker stop $CONTAINER_NAME
    docker rm -f $CONTAINER_NAME
    rm -rf $LOG_FOLDER
    exit 0
fi

docker stop $CONTAINER_NAME >/dev/null 2>&1
docker rm -f $CONTAINER_NAME >/dev/null 2>&1

docker stop "php" >/dev/null 2>&1
docker rm -f "php" >/dev/null 2>&1

docker run \
    --name "php" \
    -p 9000:9000 \
    -v $(pwd)/data:/var/www/html \
    -d \
    php:7-fpm

docker run \
    --name $CONTAINER_NAME \
    -v $(pwd)/conf/nginx.conf:/etc/nginx/nginx.conf \
    -v $(pwd)/data:/var/www/html \
    -v $(pwd)/$LOG_FOLDER:/var/log/nginx \
    -p $PORT:80 \
    -p 4141:4141 \
    -p 4242:4242 \
    --link php:php \
    -d \
    nginx:1.22-alpine nginx-debug -g 'daemon off;'
