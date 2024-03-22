You can use the following command to run the TCP related code.
gcc tcp_client.c -o tcp_client -lws2_32
gcc tcp_server.c -o tcp_server -lws2_32
./tcp_server 127.0.0.1 8080
./tcp_client 127.0.0.1 8080
