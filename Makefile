# Variables del compilador
CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
CPPFLAGS = -I/usr/include/tirpc
LDFLAGS = -L. -Wl,-rpath=.
RPCLIBS = -ltirpc

.PHONY: all clean rpcgen

# Regla por defecto que compila todo
all: libclaves.so libproxyclaves.so clavesRPC_server cliente

# 1. Generación automática de ficheros RPC a partir del .x
rpcgen: clavesRPC.h clavesRPC_clnt.c clavesRPC_svc.c clavesRPC_xdr.c

clavesRPC.h clavesRPC_clnt.c clavesRPC_svc.c clavesRPC_xdr.c: clavesRPC.x
	rpcgen -NM clavesRPC.x

# 2. Creación de las bibliotecas dinámicas (.so)

# Biblioteca local (usada por el servidor)
libclaves.so: claves.o
	$(CC) -shared -o $@ $^ -lpthread

claves.o: claves.c claves.h
	$(CC) $(CFLAGS) -c $<

# Biblioteca proxy (usada por el cliente)
libproxyclaves.so: proxy-rpc.o clavesRPC_clnt.o clavesRPC_xdr.o
	$(CC) -shared -o $@ $^ $(RPCLIBS)

proxy-rpc.o: proxy-rpc.c claves.h clavesRPC.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clavesRPC_clnt.o: clavesRPC_clnt.c clavesRPC.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clavesRPC_xdr.o: clavesRPC_xdr.c clavesRPC.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

# 3. Creación de los ejecutables

# Ejecutable del servidor RPC
clavesRPC_server: clavesRPC_svc.o rpc_service.o clavesRPC_xdr.o libclaves.so
	$(CC) -o $@ clavesRPC_svc.o rpc_service.o clavesRPC_xdr.o $(LDFLAGS) -lclaves $(RPCLIBS) -lpthread

clavesRPC_svc.o: clavesRPC_svc.c clavesRPC.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

rpc_service.o: rpc_service.c claves.h clavesRPC.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

# Ejecutable del cliente
cliente: app-cliente.o libproxyclaves.so
	$(CC) -o $@ app-cliente.o $(LDFLAGS) -lproxyclaves $(RPCLIBS)

app-cliente.o: app-cliente.c claves.h
	$(CC) $(CFLAGS) -c $<

# 4. Limpieza de archivos generados
clean:
	rm -f *.o *.so clavesRPC_server cliente