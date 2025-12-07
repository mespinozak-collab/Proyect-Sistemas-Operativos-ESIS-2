// simulador_memoria.c

#include "simulador.h"

// Función auxiliar crear un nuevo nodo de bloque de memoria
BloqueMemoria* crear_bloque(int inicio, int tamano, int pid) {
    BloqueMemoria* nuevo = (BloqueMemoria*)malloc(sizeof(BloqueMemoria));
    if (nuevo == NULL) {
        perror("Error al asignar memoria para BloqueMemoria");
        exit(EXIT_FAILURE);
    }
    nuevo->inicio = inicio;
    nuevo->tamano = tamano;
    nuevo->id_proceso = pid;
    nuevo->siguiente = NULL;
    return nuevo;
}

// Inicializa la memoria 
BloqueMemoria* inicializar_memoria(int tam_total) {
    // Prototipo de la función de asignación
    int asignar_memoria(BloqueMemoria** memoria_ptr, Proceso* p, const char* estrategia);
    
    return crear_bloque(0, tam_total, -1);
}

BloqueMemoria* encontrar_first_fit(BloqueMemoria* head, int tam_sol) {
    BloqueMemoria* actual = head;
    while (actual != NULL) {
        // Se Busca si el bloque que esta libre y con tamaño suficiente
        if (actual->id_proceso == -1 && actual->tamano >= tam_sol) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL; 
}

BloqueMemoria* encontrar_best_fit(BloqueMemoria* head, int tam_sol) {
    BloqueMemoria* mejor_bloque = NULL;
    int menor_sobrante = -1; // Encontrar el bloque más ajustado

    BloqueMemoria* actual = head;
    while (actual != NULL) {
        if (actual->id_proceso == -1 && actual->tamano >= tam_sol) {
            int sobrante = actual->tamano - tam_sol;
           
            if (mejor_bloque == NULL || sobrante < menor_sobrante) {
                menor_sobrante = sobrante;
                mejor_bloque = actual;
            }
        }
        actual = actual->siguiente;
    }
    return mejor_bloque;
}

int asignar_memoria(BloqueMemoria** memoria_ptr, Proceso* p, const char* estrategia) {
    int tam_sol = p->tam_memoria;
    BloqueMemoria* bloque_encontrado = NULL;
   
    // Buscar el bloque libre según la estrategia
    if (strcmp(estrategia, "first-fit") == 0) {
        bloque_encontrado = encontrar_first_fit(*memoria_ptr, tam_sol);
    } else if (strcmp(estrategia, "best-fit") == 0) {
        bloque_encontrado = encontrar_best_fit(*memoria_ptr, tam_sol);
    }

    if (bloque_encontrado == NULL) {
        printf("MEMORIA: PID %d, Tam %d -> NO ENCONTRADO\n", p->pid, tam_sol);
        return 0; // Falló la asignación
    }

    // Asignación y Manejo de Fragmentacion de memoria
    int restante = bloque_encontrado->tamano - tam_sol;
   
    p->dir_inicio_mem = bloque_encontrado->inicio; // Registrar la dirección en el PCB
    p->tam_bloque_mem = tam_sol;

    if (restante > 0) {
        // Fragmentación
       
        // Crea el nuevo bloque libre restante
        BloqueMemoria* nuevo_libre = crear_bloque(bloque_encontrado->inicio + tam_sol, restante, -1);
       
        // Insertar el nuevo libre después del bloque_encontrado
        nuevo_libre->siguiente = bloque_encontrado->siguiente;
        bloque_encontrado->siguiente = nuevo_libre;
       
        // El bloque_encontrado se convierte en el bloque asignado.
        bloque_encontrado->tamano = tam_sol;
        bloque_encontrado->id_proceso = p->pid;
       
    } else {
        // Ajuste Exacto o restante == 0, El bloque libre se convierte en asignado.
        bloque_encontrado->id_proceso = p->pid;
    }
   
    printf("MEMORIA: PID %d asignado en 0x%X (Tam: %d). Bloque elegido: %d\n",
           p->pid, p->dir_inicio_mem, tam_sol, bloque_encontrado->tamano);
   
    return 1; // Éxito
}

void desasignar_memoria(BloqueMemoria** memoria_ptr, Proceso* p) {
    if (p->dir_inicio_mem == -1) {
        // El proceso nunca tuvo memoria asignada o ya fue liberada.
        return;
    }

    BloqueMemoria* actual = *memoria_ptr;
    BloqueMemoria* previo = NULL;
   
    // Encontrar el bloque del proceso a ser liberado
    while (actual != NULL && actual->id_proceso != p->pid) {
        previo = actual;
        actual = actual->siguiente;
    }

    if (actual == NULL) {
        printf("MEMORIA: Bloque de PID %d no encontrado para liberar.\n", p->pid);
        return;
    }

    // Marcar el bloque como libre
    actual->id_proceso = -1;
    printf("MEMORIA: PID %d liberado en 0x%X.\n", p->pid, actual->inicio);

    // Reiniciar los campos del PCB 
    p->dir_inicio_mem = -1;
    p->tam_bloque_mem = 0;

    // COALESCING o FUSION hacia adelante
    if (actual->siguiente != NULL && actual->siguiente->id_proceso == -1) {
        BloqueMemoria* siguiente_libre = actual->siguiente;
       
        // Fusion Sumar el tamaño del siguiente al actual y saltarse el siguiente
        actual->tamano += siguiente_libre->tamano;
        actual->siguiente = siguiente_libre->siguiente;
       
        free(siguiente_libre);
    }

    // COALESCING o FUSION hacia atrás
    if (previo != NULL && previo->id_proceso == -1) {
        // Fusion Sumar el tamaño del actual al previo y saltarse el actual
        previo->tamano += actual->tamano;
        previo->siguiente = actual->siguiente;
       
        free(actual);
    }
}
