#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>

#include "claves.h"
#include "clavesRPC.h"

#define MAX_STR 256
#define MAX_V2 32

static struct Paquete rpc_a_local_paquete(PaqueteRPC p) {
    struct Paquete out;
    out.x = p.x;
    out.y = p.y;
    out.z = p.z;
    return out;
}

static PaqueteRPC local_a_rpc_paquete(struct Paquete p) {
    PaqueteRPC out;
    out.x = p.x;
    out.y = p.y;
    out.z = p.z;
    return out;
}

bool_t destroy_1_svc(int *result, struct svc_req *rqstp) {
    (void)rqstp;

    if (result == NULL) {
        return FALSE;
    }

    *result = destroy();
    return TRUE;
}

bool_t set_value_1_svc(ArgsSetModify arg1, int *result, struct svc_req *rqstp) {
    struct Paquete p;

    (void)rqstp;

    if (result == NULL) {
        return FALSE;
    }

    p = rpc_a_local_paquete(arg1.value3);

    *result = set_value(
        arg1.key,
        arg1.value1,
        arg1.N_value2,
        arg1.V_value2.V_value2_val,
        p
    );

    return TRUE;
}

bool_t get_value_1_svc(char *arg1, GetValueResult *result, struct svc_req *rqstp) {
    char value1[MAX_STR];
    int n_value2 = 0;
    float v_value2[MAX_V2];
    struct Paquete value3;

    (void)rqstp;

    if (result == NULL) {
        return FALSE;
    }

    memset(result, 0, sizeof(*result));

    if (arg1 == NULL) {
        result->status = -1;
        return TRUE;
    }

    result->status = get_value(arg1, value1, &n_value2, v_value2, &value3);
    if (result->status != 0) {
        return TRUE;
    }

    result->value1 = strdup(value1);
    if (result->value1 == NULL) {
        result->status = -1;
        return TRUE;
    }

    result->N_value2 = n_value2;
    result->V_value2.V_value2_len = n_value2;
    result->V_value2.V_value2_val = (float *)malloc(sizeof(float) * n_value2);
    if (result->V_value2.V_value2_val == NULL) {
        free(result->value1);
        result->value1 = NULL;
        result->status = -1;
        return TRUE;
    }

    for (int i = 0; i < n_value2; i++) {
        result->V_value2.V_value2_val[i] = v_value2[i];
    }

    result->value3 = local_a_rpc_paquete(value3);

    return TRUE;
}

bool_t modify_value_1_svc(ArgsSetModify arg1, int *result, struct svc_req *rqstp) {
    struct Paquete p;

    (void)rqstp;

    if (result == NULL) {
        return FALSE;
    }

    p = rpc_a_local_paquete(arg1.value3);

    *result = modify_value(
        arg1.key,
        arg1.value1,
        arg1.N_value2,
        arg1.V_value2.V_value2_val,
        p
    );

    return TRUE;
}

bool_t delete_key_1_svc(char *arg1, int *result, struct svc_req *rqstp) {
    (void)rqstp;

    if (result == NULL) {
        return FALSE;
    }

    if (arg1 == NULL) {
        *result = -1;
        return TRUE;
    }

    *result = delete_key(arg1);
    return TRUE;
}

bool_t exist_1_svc(char *arg1, int *result, struct svc_req *rqstp) {
    (void)rqstp;

    if (result == NULL) {
        return FALSE;
    }

    if (arg1 == NULL) {
        *result = -1;
        return TRUE;
    }

    *result = exist(arg1);
    return TRUE;
}