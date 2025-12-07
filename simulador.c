// simulador.c 

#include "simulador.h" 

// Función auxiliar ordena el array inicial de procesos por tiempo de llegada.
void ordenar_por_llegada(Proceso procesos[], int n) {
    int comparar_llegada(const void* a, const void* b) {
        Proceso* p1 = (Proceso*)a;
        Proceso* p2 = (Proceso*)b;
        return p1->llegada - p2->llegada;
    }
    qsort(procesos, n, sizeof(Proceso), comparar_llegada);
}

int comparar_por_pid(const void* a, const void* b) {
    Proceso* proc_a = (Proceso*)a;
    Proceso* proc_b = (Proceso*)b;
   
    // Restar los PIDs, a->pid < b->pid, a va primero
    return proc_a->pid - proc_b->pid;
}

void iniciar_simulacion(Proceso procesos[], int num_procesos, Configuracion conf, BloqueMemoria** memoria_ptr) {
    // Inicialización y ordenamiento
    ordenar_por_llegada(procesos, num_procesos);
   
    NodoProceso* cola_listos = NULL;
    NodoProceso* lista_terminados = NULL;
    Proceso* proceso_en_cpu = NULL;
   
    int tiempo_actual = 0;
    int procesos_terminados = 0;
    int indice_proceso_llegada = 0;
   
    int quantum_restante_actual = 0;

    printf("\nINICIANDO SIMULACIÓN (%s) \n", conf.algoritmo_cpu);

    while (procesos_terminados < num_procesos || proceso_en_cpu != NULL || indice_proceso_llegada < num_procesos) {
       
        // Zona de manejo de Llegadas
        while (indice_proceso_llegada < num_procesos &&
               procesos[indice_proceso_llegada].llegada == tiempo_actual) {
           
            Proceso* p_nuevo = &procesos[indice_proceso_llegada];
           
            if (strcmp(conf.algoritmo_cpu, "SPN") == 0) {
                insertar_spn(&cola_listos, p_nuevo);
            } else { 
                encolar(&cola_listos, p_nuevo);
            }
            indice_proceso_llegada++;
        }

        // Selección del Proceso 
        if (proceso_en_cpu == NULL && cola_listos != NULL) {
            proceso_en_cpu = desencolar(&cola_listos);
           
            // Registrar Inicio 
            if (proceso_en_cpu->inicio == -1) {
                proceso_en_cpu->inicio = tiempo_actual;
            }
           
            // INICIALIZAR QUANTUM para RR
            if (strcmp(conf.algoritmo_cpu, "RR") == 0) {
                quantum_restante_actual = conf.quantum;
            }
        }
        
        if (proceso_en_cpu->restante == 0) {
	    proceso_en_cpu->fin = tiempo_actual;
	   
	    // LIBERACION DE LA MEMORIA ASIGNADA
	    if (proceso_en_cpu->tam_memoria > 0) {
		desasignar_memoria(memoria_ptr, proceso_en_cpu);
	    }

	    encolar(&lista_terminados, proceso_en_cpu);
	    proceso_en_cpu = NULL;
	    procesos_terminados++;
	}
       
        // Ejecución
        if (proceso_en_cpu != NULL) {
           
            // Avanzar el tiempo y actualizar restante
            tiempo_actual++;
            proceso_en_cpu->restante--;
           
            // MANEJO DE QUANTUM
            if (strcmp(conf.algoritmo_cpu, "RR") == 0) {
                quantum_restante_actual--;
            }
           
            // Manejo de Terminación
            if (proceso_en_cpu->restante == 0) {
                proceso_en_cpu->fin = tiempo_actual;
                encolar(&lista_terminados, proceso_en_cpu);
                proceso_en_cpu = NULL;
                procesos_terminados++;
               
            }
            // Manejo de Expropiación solo para Round Robin
            else if (strcmp(conf.algoritmo_cpu, "RR") == 0 && quantum_restante_actual == 0) {
                // Quantum expiró, pero el proceso no ha terminado. ¡Expropiar!
                encolar(&cola_listos, proceso_en_cpu); // Mover al final de la cola
                proceso_en_cpu = NULL; 
            }
           
        } else {
            // Avanzar el tiempo solo si hay procesos por llegar, evitando un bucle infinito
            if (indice_proceso_llegada < num_procesos) {
                tiempo_actual++;
            } else if (procesos_terminados == num_procesos) {
                break;
            }
        }
    }

    // Cálculo e Impresión de Métricas
    calcular_y_mostrar_metricas(lista_terminados, num_procesos, tiempo_actual);
}

// Función de cálculo e impresión
void calcular_y_mostrar_metricas(NodoProceso* lista_terminados, int num_procesos, int tiempo_total) {
   
    // Copiar los datos a un array
    Proceso* procesos_ordenados = (Proceso*)malloc(num_procesos * sizeof(Proceso));
    NodoProceso* actual = lista_terminados;
    int k = 0;
    while (actual != NULL) {
        // Copiar la estructura del proceso
        procesos_ordenados[k] = *(actual->pcb);
        actual = actual->siguiente;
        k++;
    }

    // Ordenar el array por PID usando qsort
    // Se Necesita una función de comparación
    qsort(procesos_ordenados, num_procesos, sizeof(Proceso), comparar_por_pid);

    printf("\nNone\n");
    printf("%-3s | %-7s | %-8s | %-6s | %-3s | %-9s | %-7s | %-7s\n",
           "PID", "Llegada", "Servicio", "Inicio", "Fin", "Respuesta", "Espera", "Retorno");
    printf("--- | ------- | -------- | ------ | --- | --------- | ------- | -------\n");

    for (int i = 0; i < num_procesos; i++) {
        Proceso* p = &procesos_ordenados[i];
       
        p->respuesta = p->inicio - p->llegada;
        p->retorno = p->fin - p->llegada;
        p->espera = p->retorno - p->servicio;

        // ... Imprimir línea ...
        printf("%-3d | %-7d | %-8d | %-6d | %-3d | %-9d | %-7d | %-7d\n",
               p->pid, p->llegada, p->servicio, p->inicio, p->fin,
               p->respuesta, p->espera, p->retorno);
    }

    free(procesos_ordenados);
}
