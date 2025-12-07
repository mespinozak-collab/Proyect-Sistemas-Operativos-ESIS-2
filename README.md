# Proyect-Sistemas-Operativos-ESIS
# Descripción

El proyecto consiste en un Simulador de Planificación de CPU y Gestión de Memoria (SISO) desarrollado en C, capaz de modelar el comportamiento de un sistema operativo simple.
El simulador analiza distintos algoritmos de planificación (FCFS, SPN, RR) y estrategias de asignación de memoria (First-Fit, Best-Fit), procesando toda la configuración desde un archivo JSON usando la librería Jansson.

El objetivo es aproximar el funcionamiento interno de un sistema operativo real, manejando procesos, colas, memoria principal y métricas de rendimiento como:

Tiempo de Espera

Tiempo de Retorno

Tiempo de Respuesta

Requerimientos

Sistema operativo:

Linux o cualquier distribución UNIX (Ubuntu, Debian, Arch, etc.)

Dependencias:

Compilador GCC compatible con C99

Librería Jansson para lectura de JSON

Cabeceras estándar del sistema (stdlib.h, stdio.h, unistd.h, etc.)

Instalación de dependencias (Ubuntu/Debian):

sudo apt update
sudo apt install libjansson-dev

# Instrucciones

Compila el proyecto dentro del directorio donde están los archivos .c

gcc main.c simulador.c simulador_colas.c simulador_memoria.c -o sim_so -std=c99 -ljansson

Para ejecutar el simulador:

./sim_so config.json

# Configuración de los Archivos JSON

| Objeto              | Parámetro  | Tipo                              | Descripción                        | Valores             |
| ------------------- | ---------- | --------------------------------- | ---------------------------------- | ------------------- |
| **cpu**             | algoritmo  | string                            | Algoritmo de planificación         | FCFS, SPN, RR       |
|                     | quantum    | int                               | Quantum para Round Robin           | > 0                 |
| **memoria**         | tam        | int                               | Tamaño total de memoria (bytes)    | Entero              |
|                     | estrategia | string                            | Método de asignación               | first-fit, best-fit |
| **procesos**        | Array      | Lista de procesos                 | Objetos con PID, llegada, servicio |                     |
| **solicitudes_mem** | Array      | Asignaciones iniciales de memoria | Objetos con PID y tamaño           |                     |

# Estructura de procesos

| Campo    | Tipo | Descripción               |
| -------- | ---- | ------------------------- |
| pid      | int  | Identificador del proceso |
| llegada  | int  | Tiempo de arribo          |
| servicio | int  | CPU total requerida       |

# Estructura de solicitudes de memoria

| Campo | Tipo | Descripción               |
| ----- | ---- | ------------------------- |
| pid   | int  | Proceso solicitante       |
| tam   | int  | Tamaño solicitado (bytes) |

# Ejemplo de config.json

{
    "cpu": { 
        "algoritmo": "RR", 
        "quantum": 4 
    }, 
    "procesos": [
        {"pid": 1, "llegada": 0, "servicio": 12}, 
        {"pid": 2, "llegada": 1, "servicio": 5},
        {"pid": 3, "llegada": 2, "servicio": 8}
    ],
    "memoria": { 
        "tam": 1048576, 
        "estrategia": "best-fit" 
    },
    "solicitudes_mem": [ {"pid": 1, "tam": 120000},  {"pid": 2, "tam": 64000} ]
}

# Resultados del simulador

El programa genera una tabla final con todas las métricas de rendimiento por cada proceso.

| Métrica   | Descripción                                 | Fórmula            |
| --------- | ------------------------------------------- | ------------------ |
| PID       | Identificador del proceso                   |                    |
| Llegada   | Tiempo de arribo                            |                    |
| Servicio  | CPU requerida                               |                    |
| Inicio    | Primer uso de CPU                           |                    |
| Fin       | Momento en que termina                      |                    |
| Respuesta | Tiempo hasta que se ejecuta por primera vez | Inicio − Llegada   |
| Espera    | Tiempo en cola                              | Retorno − Servicio |
| Retorno   | Tiempo total en el sistema                  | Fin − Llegada      |

# Salida

PID | Llegada | Servicio | Inicio | Fin | Respuesta | Espera | Retorno
--- | ------- | -------- | ------ | --- | --------- | ------- | -------
1   | 0       | 12       | 0      | 22  | 0         | 10      | 22
2   | 1       | 5        | 4      | 9   | 3         | 3       | 8
3   | 2       | 8        | 12     | 21  | 10        | 11      | 19

# Características del simulador

| Funcionalidad             | Descripción                        |
| ------------------------- | ---------------------------------- |
| Planificación de CPU      | FCFS, SPN y RR con quantum         |
| Administración de Memoria | First-Fit y Best-Fit               |
| Lectura desde JSON        | Toda la configuración es externa   |
| Métricas completas        | Respuesta, Espera, Retorno         |
| Procesamiento de colas    | Organización y manejo interno      |
| Trazas por consola        | Seguimiento paso a paso (opcional) |

# Mejoras

| Funcionalidad            | Descripción                                      |
| ------------------------ | ------------------------------------------------ |
| Procesamiento por ciclos | Ejecuta tick por tick simulando un CPU real      |
| Asignación dinámica      | Administración de memoria durante la simulación  |
| Representación tabular   | Resultados claros y ordenados                    |
| Modularidad              | Código dividido en múltiples módulos `.c` y `.h` |
| JSON flexible            | Permite aumentar o reducir procesos fácilmente   |

# Estructura del proyecto

/proyecto
  ├─ src/
  │   ├─ main.c
  │   ├─ simulador.c
  │   ├─ simulador.h
  │   ├─ simulador_colas.c
  │   ├─ simulador_colas.h
  │   ├─ simulador_memoria.c
  │   └─ simulador_memoria.h
  ├─ config/
  │   ├─  config.json
  └─ README.md
