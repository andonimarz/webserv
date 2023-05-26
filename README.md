# Funciones principales

**execve:** Ejecuta un programa con argumentos dados.
```
char *args[] = {"ls", "-la", NULL};
execve("/bin/ls", args, NULL);
```

**dup:** Duplica un descriptor de archivo.
```
int fd1 = open("file.txt", O_RDONLY);
int fd2 = dup(fd1);
```

**dup2:** Duplica un descriptor de archivo en otro número de archivo.
```
int fd1 = open("file.txt", O_RDONLY);
int fd2 = open("copy.txt", O_WRONLY | O_CREAT);
dup2(fd1, fd2);
```

**pipe:** Crea un par de descriptores de archivo para la comunicación de procesos.
```
int fd[2];
pipe(fd);
```

**strerror:** Devuelve una cadena que describe el error especificado por el número de error.
```
int errnum = ENOENT;
printf("Error: %s\n", strerror(errnum));
```

**gai_strerror:** Devuelve una cadena que describe el error especificado por el código de error de getaddrinfo.
```
int errcode = EAI_NONAME;
printf("Error: %s\n", gai_strerror(errcode));
```

**errno:** Variable global que contiene el número de error de la última llamada al sistema que falló.
```
int fd = open("nonexistent.txt", O_RDONLY);
if (fd == -1) {
    printf("Error: %s\n", strerror(errno));
}
```

**fork:** Crea un nuevo proceso clonando el proceso padre.
```
pid_t pid = fork();
if (pid == 0) {
    printf("Este es el proceso hijo\n");
} else {
    printf("Este es el proceso padre\n");
}
```

**htons:** Convierte un número de 16 bits al orden de bytes de la red.
```
uint16_t host_num = 0x1234;
uint16_t net_num = htons(host_num);
```

**htonl:** Convierte un número de 32 bits al orden de bytes de la red.
```
uint32_t host_num = 0x12345678;
uint32_t net_num = htonl(host_num);
```

**ntohs:** Convierte un número de 16 bits del orden de bytes de la red al orden de bytes del host.
```
uint16_t net_num = 0x3412;
uint16_t host_num = ntohs(net_num);
```

**ntohl:** Convierte un número de 32 bits del orden de bytes de la red al orden de bytes del host.
```
uint32_t net_num = 0x78563412;
uint32_t host_num = ntohl(net_num);
```

**select:** Espera hasta que uno o más descriptores de archivo estén listos para lectura, escritura o excepción.
```
fd_set read_fds;
FD_ZERO(&read_fds);
FD_SET(fd, &read_fds);
select(fd + 1, &read_fds, NULL, NULL, NULL);
```

**poll:** Espera hasta que uno o más descriptores de archivo estén listos para lectura, escritura o excepción.
```
struct pollfd fds[1];
fds[0].fd = fd;
fds[0].events = POLLIN;
poll(fds, 1, -1);
```

**epoll_create:** Crea una instancia de epoll.
```
int epfd = epoll_create(1);
```

**epoll_ctl:** Controla la operación de un descriptor de archivo en una instancia de epoll.
```
struct epoll_event event;
event.data.fd = fd;
event.events = EPOLLIN | EPOLLET;
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
```

**epoll_wait:** Espera hasta que uno o más descriptores de archivo estén listos para lectura, escritura o excepción en una instancia de epoll.
```
struct epoll_event events[10];
int n = epoll_wait(epfd, events, 10, -1);
for (int i = 0; i < n; i++) {
    // procesar el evento
}
```

**kqueue:** Crea una instancia de kqueue.
```
int kq = kqueue();
```

**kevent:** Controla la operación de un descriptor de archivo en una instancia de kqueue.
```
struct kevent event;
EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
kevent(kq, &event, 1, NULL, 0, NULL);
```

**socket:** Crea un socket.
```
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

**accept:** Acepta una conexión entrante en un socket.
```
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
```

**listen:** Escucha las conexiones entrantes en un socket.
```
int backlog = 10;
listen(sockfd, backlog);
```

**send:** Envía datos a través de un socket.
```
char *msg = "Hola, mundo!";
int len = strlen(msg);
int n = send(sockfd, msg, len, 0);
```

**recv:** Recibe datos a través de un socket.
```
char buffer[1024];
int n = recv(sockfd, buffer, sizeof(buffer), 0);
```

**bind:** Asocia un socket con una dirección.
```
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(port);
server_addr.sin_addr.s_addr = INADDR_ANY;
bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

**connect:** Establece una conexión con un socket.
```
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(port);
inet_pton(AF_INET, ip_addr, &server_addr.sin_addr);
connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

**getaddrinfo:** Obtiene información de direcciones para un nombre de host y un servicio.
```
struct addrinfo hints, *result;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
int rc = getaddrinfo("www.example.com", "http", &hints, &result);
```

**freeaddrinfo:** Libera la memoria reservada por getaddrinfo.
```
freeaddrinfo(result);
```

**setsockopt:** Configura las opciones de un socket.
```
int optval = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
```
**getsockname:** Obtiene la dirección local asociada con un socket.
```
struct sockaddr_in local_addr;
socklen_t addr_len = sizeof(local_addr);
getsockname(sockfd, (struct sockaddr *)&local_addr, &addr_len);
```

**getprotobyname:** Obtiene una estructura protoent para un protocolo dado su nombre.
```
struct protoent *proto = getprotobyname("tcp");
```

**fcntl:** Obtiene o establece las propiedades del descriptor de archivo, como modo de bloqueo y banderas de acceso.
```
int flags = fcntl(sockfd, F_GETFL, 0);
flags |= O_NONBLOCK;
fcntl(sockfd, F_SETFL, flags);
```
