#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

static void svuota_buffer_main(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

static void pulisci_schermo_main(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    stampa_logo();
}

int main(void)
{
    // Inizializzazione del generatore di numeri casuali (una sola volta nel programma)
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);

    int scelta_utente = 0;

    do
    {
        pulisci_schermo_main();

        printf("\n--- MENU PRINCIPALE ---\n");
        printf("1. Imposta gioco\n");
        printf("2. Gioca\n");
        printf("3. Termina gioco\n");
        printf("4. Visualizza i crediti\n");
        printf("Scegli un'opzione: ");

        if (scanf("%d", &scelta_utente) != 1)
        {
            printf("\nErrore: inserisci un valore numerico.\n");
            svuota_buffer_main();
            continue;
        }
        svuota_buffer_main();

        switch (scelta_utente)
        {
        case 1:
        {
            imposta_gioco();
            break;
        }
        case 2:
        {
            gioca();
            break;
        }
        case 3:
        {
            termina_gioco();
            break;
        }
        case 4:
        {
            crediti();
            break;
        }
        default:
        {
            printf("\nComando sbagliato! Riprova.\n");
            break;
        }
        }

    } while (scelta_utente != 3);

    return 0;
}