#!/bin/bash

replace_string() {
	input_string=$1
	replaced_string="${input_string//host_here/$CONFIG_HOST}"
	echo "${replaced_string//port_here/$CONFIG_PORT}"
}

# Se supone que esta comprobado que el archivo existe
if [ "$REQUEST_METHOD" == "GET" ]
then
	content=$(cat "$1")
	modified_content=$(replace_string "$content")
	size=${#modified_content}
	echo "HTTP/1.1 $REQUEST_STATUS OK"
    echo "Content-Length: $size"
	echo ""
    echo "$modified_content"
fi

# Ya esta comprobado que el archivo no existe
if [ "$REQUEST_METHOD" == "POST" ]
then
	content=$(cat "$1")
	size=${#content}
	echo "HTTP/1.1 $REQUEST_STATUS OK"
    echo "Content-Length: $size"
	echo ""
    echo "$content"
fi

if [ "$REQUEST_METHOD" == "DELETE" ]
then
	rm -rf "$REQUEST_ROUTE"
	echo "HTTP/1.1 $REQUEST_STATUS OK"
	echo ""
fi

