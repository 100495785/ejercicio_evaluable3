#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>

#include "claves.h"
#include "clavesRPC.h"

#define MAX_STR 256
#define MAX_V2 32

static CLIENT *crear_cliente_rpc(void) {
    char *ip = getenv("IP_TUPLAS");

    if (ip == NULL || *ip == '\0') {
        fprintf(stderr, "Error: la variable de entorno IP_TUPLAS no esta definida.\n");
        return NULL;
    }

    CLIENT *clnt = clnt_create(ip, CLAVESRPC_PROG, CLAVESRPC_VERS, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(ip);
        return NULL;
    }

    return clnt;
}

static void copiar_paquete_rpc_a_local(struct Paquete *dst, PaqueteRPC src) {
    dst->x = src.x;
    dst->y = src.y;
    dst->z = src.z;
}

static PaqueteRPC copiar_paquete_local_a_rpc(struct Paquete src) {
    PaqueteRPC p;
    p.x = src.x;
    p.y = src.y;
    p.z = src.z;
    return p;
}

int destroy(void) {
    CLIENT *clnt = crear_cliente_rpc();
    int result = -1;
    enum clnt_stat estado;

    if (clnt == NULL) {
        return -1;
    }

    estado = destroy_1(&result, clnt);
    if (estado != RPC_SUCCESS) {
        clnt_perror(clnt, "RPC destroy fallo");
        clnt_destroy(clnt);
        return -1;
    }

    clnt_destroy(clnt);
    return result;
}

int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    CLIENT *clnt;
    ArgsSetModify args;
    int result = -1;
    enum clnt_stat estado;

    if (key == NULL || value1 == NULL || V_value2 == NULL) {
        return -1;
    }

    clnt = crear_cliente_rpc();
    if (clnt == NULL) {
        return -1;
    }

    memset(&args, 0, sizeof(args));
    args.key = key;
    args.value1 = value1;
    args.N_value2 = N_value2;
    args.V_value2.V_value2_len = N_value2;
    args.V_value2.V_value2_val = V_value2;
    args.value3 = copiar_paquete_local_a_rpc(value3);

    estado = set_value_1(args, &result, clnt);
    if (estado != RPC_SUCCESS) {
        clnt_perror(clnt, "RPC set_value fallo");
        clnt_destroy(clnt);
        return -1;
    }

    clnt_destroy(clnt);
    return result;
}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3) {
    CLIENT *clnt;
    GetValueResult result;
    enum clnt_stat estado;

    if (key == NULL || value1 == NULL || N_value2 == NULL || V_value2 == NULL || value3 == NULL) {
        return -1;
    }

    clnt = crear_cliente_rpc();
    if (clnt == NULL) {
        return -1;
    }

    memset(&result, 0, sizeof(result));

    estado = get_value_1(key, &result, clnt);
    if (estado != RPC_SUCCESS) {
        clnt_perror(clnt, "RPC get_value fallo");
        clnt_destroy(clnt);
        return -1;
    }

    if (result.status != 0) {
        xdr_free((xdrproc_t)xdr_GetValueResult, (char *)&result);
        clnt_destroy(clnt);
        return -1;
    }

    strncpy(value1, result.value1, MAX_STR - 1);
    value1[MAX_STR - 1] = '\0';

    *N_value2 = result.N_value2;
    if (*N_value2 < 1 || *N_value2 > MAX_V2) {
        xdr_free((xdrproc_t)xdr_GetValueResult, (char *)&result);
        clnt_destroy(clnt);
        return -1;
    }

    if ((int)result.V_value2.V_value2_len != *N_value2) {
        xdr_free((xdrproc_t)xdr_GetValueResult, (char *)&result);
        clnt_destroy(clnt);
        return -1;
    }

    for (int i = 0; i < *N_value2; i++) {
        V_value2[i] = result.V_value2.V_value2_val[i];
    }

    for (int i = *N_value2; i < MAX_V2; i++) {
        V_value2[i] = 0.0f;
    }

    copiar_paquete_rpc_a_local(value3, result.value3);

    xdr_free((xdrproc_t)xdr_GetValueResult, (char *)&result);
    clnt_destroy(clnt);
    return 0;
}

int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    CLIENT *clnt;
    ArgsSetModify args;
    int result = -1;
    enum clnt_stat estado;

    if (key == NULL || value1 == NULL || V_value2 == NULL) {
        return -1;
    }

    clnt = crear_cliente_rpc();
    if (clnt == NULL) {
        return -1;
    }

    memset(&args, 0, sizeof(args));
    args.key = key;
    args.value1 = value1;
    args.N_value2 = N_value2;
    args.V_value2.V_value2_len = N_value2;
    args.V_value2.V_value2_val = V_value2;
    args.value3 = copiar_paquete_local_a_rpc(value3);

    estado = modify_value_1(args, &result, clnt);
    if (estado != RPC_SUCCESS) {
        clnt_perror(clnt, "RPC modify_value fallo");
        clnt_destroy(clnt);
        return -1;
    }

    clnt_destroy(clnt);
    return result;
}

int delete_key(char *key) {
    CLIENT *clnt;
    int result = -1;
    enum clnt_stat estado;

    if (key == NULL) {
        return -1;
    }

    clnt = crear_cliente_rpc();
    if (clnt == NULL) {
        return -1;
    }

    estado = delete_key_1(key, &result, clnt);
    if (estado != RPC_SUCCESS) {
        clnt_perror(clnt, "RPC delete_key fallo");
        clnt_destroy(clnt);
        return -1;
    }

    clnt_destroy(clnt);
    return result;
}

int exist(char *key) {
    CLIENT *clnt;
    int result = -1;
    enum clnt_stat estado;

    if (key == NULL) {
        return -1;
    }

    clnt = crear_cliente_rpc();
    if (clnt == NULL) {
        return -1;
    }

    estado = exist_1(key, &result, clnt);
    if (estado != RPC_SUCCESS) {
        clnt_perror(clnt, "RPC exist fallo");
        clnt_destroy(clnt);
        return -1;
    }

    clnt_destroy(clnt);
    return result;
}