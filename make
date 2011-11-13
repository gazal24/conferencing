rm server
rm client
g++ server.cpp -o server -lpthread
g++ client.cpp -o client -lpthread
