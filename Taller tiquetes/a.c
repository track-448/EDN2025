#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_APELLIDOS 110

typedef enum { FEMENINO = 1, MASCULINO = 2, NO_BINARIO = 3 } Gender;

typedef struct passenger {
    char last_name[MAX_APELLIDOS];
    Gender genero;
    struct passenger *next;
} Passenger;

typedef struct {
    Passenger *head;
    Passenger *tail;
    int size;
} List;

void init_list(List *l) {
    l->head = l->tail = NULL;
    l->size = 0;
}

Passenger* create_passenger(const char *apellido, Gender g) {
    Passenger *p = malloc(sizeof(Passenger));
    if (!p) {
        fprintf(stderr, "Error: memoria insuficiente\n");
        exit(EXIT_FAILURE);
    }
    strncpy(p->last_name, apellido, MAX_APELLIDOS - 1);
    p->last_name[MAX_APELLIDOS - 1] = '\0';
    p->genero = g;
    p->next = NULL;
    return p;
}

void append_list(List *l, Passenger *p) {
    p->next = NULL;
    if (l->tail == NULL) {
        l->head = l->tail = p;
    } else {
        l->tail->next = p;
        l->tail = p;
    }
    l->size++;
}

Passenger* pop_front(List *l) {
    if (l->head == NULL) return NULL;
    Passenger *p = l->head;
    l->head = p->next;
    if (l->head == NULL) l->tail = NULL;
    p->next = NULL;
    l->size--;
    return p;
}

const char* gender_to_str(Gender g) {
    if (g == FEMENINO) return "Femenino";
    if (g == MASCULINO) return "Masculino";
    return "No Binario";
}

void print_list(const List *l) {
    if (l->head == NULL) {
        printf("  (vacia)\n");
        return;
    }
    Passenger *cur = l->head;
    int i = 1;
    while (cur) {
        printf("%2d. %-15s | %s\n", i, cur->last_name, gender_to_str(cur->genero));
        cur = cur->next;
        i++;
    }
}

void free_list(List *l) {
    Passenger *cur;
    while ((cur = pop_front(l)) != NULL) {
        free(cur);
    }
}

void leer_cadena(char *buf, int len) {
    if (fgets(buf, len, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    size_t L = strlen(buf);
    if (L > 0 && buf[L - 1] == '\n') buf[L - 1] = '\0';
}

int leer_entero_validado(const char *prompt, int minv, int maxv) {
    char line[100];
    int val;
    while (1) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) {
            printf("Entrada invalida. Intente de nuevo.\n");
            continue;
        }
        if (sscanf(line, "%d", &val) != 1) {
            printf("Entrada invalida. Ingrese un numero.\n");
            continue;
        }
        if (val < minv || val > maxv) {
            printf("Valor fuera de rango (%d - %d). Intente otra vez.\n", minv, maxv);
            continue;
        }
        return val;
    }
}

void vender_tiquete(List *vendidos, int max_sobreventa, int abordaje_iniciado) {
    if (abordaje_iniciado) {
        printf("No se pueden vender tiquetes: el abordaje ya fue iniciado.\n");
        return;
    }
    if (vendidos->size >= max_sobreventa) {
        printf("No se pueden vender mas tiquetes: se alcanzo el limite de sobreventa (%d).\n", max_sobreventa);
        return;
    }
    char apellido[MAX_APELLIDOS];
    int g;
    printf("Ingrese primer apellido del pasajero: ");
    leer_cadena(apellido, sizeof(apellido));
    if (strlen(apellido) == 0) {
        printf("Apellido vacio. Venta cancelada.\n");
        return;
    }
    g = leer_entero_validado("Genero (1-Femenino, 2-Masculino, 3-No binario): ", 1, 3);
    Passenger *p = create_passenger(apellido, (Gender)g);
    append_list(vendidos, p);
    printf("Tiquete vendido correctamente. Posicion en cola de venta: %d\n", vendidos->size);
}

void iniciar_abordaje(List *vendidos, List *abordados, List *no_abordados, int capacidad) {
    if (vendidos->size == 0) {
        printf("No hay pasajeros que hayan comprado tiquete.\n");
        return;
    }
    int cont = 0;
    Passenger *p;
    while (cont < capacidad && (p = pop_front(vendidos)) != NULL) {
        append_list(abordados, p);
        cont++;
    }
    while ((p = pop_front(vendidos)) != NULL) {
        append_list(no_abordados, p);
    }
    printf("Abordaje finalizado. Abordaron %d pasajeros. No abordaron %d pasajeros.\n",
           abordados->size, no_abordados->size);
}

int main(void) {
    List vendidos, abordados, no_abordados;
    init_list(&vendidos);
    init_list(&abordados);
    init_list(&no_abordados);

    int capacidad = -1;
    int max_sobreventa = -1;
    int abordaje_iniciado = 0;
    int opcion;

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Establecer Capacidad (solo al inicio)\n");
        printf("2. Vender Tiquete\n");
        printf("3. Iniciar Abordaje\n");
        printf("4. Ver Abordados\n");
        printf("5. Ver No Abordados\n");
        printf("6. Salir\n");
        opcion = leer_entero_validado("Elija una opcion: ", 1, 6);

        if (opcion == 1) {
            if (capacidad != -1) {
                printf("La capacidad ya fue establecida (%d). No puede cambiarse.\n", capacidad);
            } else {
                capacidad = leer_entero_validado("Ingrese la capacidad maxima del avion (>=1): ", 1, 1000000);
                max_sobreventa = capacidad + capacidad / 10;
                printf("Capacidad establecida: %d. Limite de sobreventa: %d\n", capacidad, max_sobreventa);
            }
        } else if (opcion == 2) {
            if (capacidad == -1) {
                printf("Primero establezca la capacidad (opcion 1).\n");
            } else {
                vender_tiquete(&vendidos, max_sobreventa, abordaje_iniciado);
            }
        } else if (opcion == 3) {
            if (capacidad == -1) {
                printf("Primero establezca la capacidad (opcion 1).\n");
            } else if (abordaje_iniciado) {
                printf("El abordaje ya fue iniciado anteriormente.\n");
            } else {
                abordaje_iniciado = 1;
                iniciar_abordaje(&vendidos, &abordados, &no_abordados, capacidad);
            }
        } else if (opcion == 4) {
            printf("\n--- Abordados (%d) ---\n", abordados.size);
            print_list(&abordados);
        } else if (opcion == 5) {
            printf("\n--- No Abordados (%d) ---\n", no_abordados.size);
            print_list(&no_abordados);
        } else if (opcion == 6) {
            printf("Saliendo. Liberando memoria...\n");
            free_list(&vendidos);
            free_list(&abordados);
            free_list(&no_abordados);
            break;
        }
    }

    return 0;
}
