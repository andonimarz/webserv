#!/bin/bash

replace_string() {
	input_string=$1
	replaced_string="${input_string//host_here/$CONFIG_HOST}"
	echo "${replaced_string//port_here/$CONFIG_PORT}"
}

# Se supone que está comprobado que el archivo no existe
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
if [ "$REQUEST_METHOD" == "POST" ]; then
    # Obtener la extensión del archivo
    filename=$(basename "$REQUEST_FILE")
    extension="${filename##*.}"

    # Establecer el tipo de contenido en función de la extensión
    case "$extension" in
        jpg|jpeg)
            content_type="image/jpeg"
            ;;
        png)
            content_type="image/png"
            ;;
        gif)
            content_type="image/gif"
            ;;
        # Agrega más extensiones y tipos de contenido según sea necesario

        *)
            content_type="application/octet-stream"
            ;;
    esac

    content_length=$(wc -c < "$REQUEST_FILE")
    echo "HTTP/1.1 $REQUEST_STATUS OK"
    echo "Content-Type: $content_type"
    echo "Content-Length: $content_length"
    echo ""
    cat "$REQUEST_FILE"
fi


if [ "$REQUEST_METHOD" == "DELETE" ]
then
	rm -rf "$REQUEST_ROUTE"
	echo "HTTP/1.1 $REQUEST_STATUS OK"
	echo ""
fi

