#!/bin/bash

# Se supone que esta comprobado que el archivo existe
if [ "$REQUEST_METHOD" == "GET" ]
then
	content=$(cat "$1")
	size=${#content}
	echo "HTTP/1.1 200 OK"
    echo "Content-Length: $size"
	echo ""
    echo "$content"
fi

# Ya esta comprobado que el archivo no existe
if [ "$REQUEST_METHOD" == "POST" ]
then
	echo "HTTP/1.1 200 OK"
	echo ""
fi

if [ "$REQUEST_METHOD" == "DELETE" ]
then
	echo "HTTP/1.1 200 OK"
	echo ""
fi
