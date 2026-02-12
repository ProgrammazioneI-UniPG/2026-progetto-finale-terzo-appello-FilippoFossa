#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gamelib.h"

// --------------------------------------------------------------------------------
// VARIABILI GLOBALI STATICHE
// --------------------------------------------------------------------------------

static struct Zona_mondoreale *prima_zona_mondoreale = NULL;
static struct Zona_soprasotto *prima_zona_soprasotto = NULL;
static struct Giocatore *giocatori[4] = {NULL, NULL, NULL, NULL};

static int mappa_chiusa = 0;
static int numero_giocatori = 0;
static int azione_movimento_usata = 0;
static int undici_presente = 0;
static char vincitori[3][50] = {"Nessuno", "Nessuno", "Nessuno"};
static char ultimo_messaggio[512] = "Partita iniziata. Buona fortuna!";

// --------------------------------------------------------------------------------
// DICHIARAZIONE FUNZIONI
// --------------------------------------------------------------------------------

// Utility
static void svuota_buffer(void);
static void pulisci_mappa(void);
static void pulisci_giocatori(void);
static void pulisci_schermo(void);
static void premi_invio(void);
static const char *nome_zona(enum Tipo_zona t);
static const char *nome_nemico(enum Tipo_nemico n);
static const char *nome_oggetto(enum Tipo_oggetto o);

// Creazione Mappa
static struct Zona_mondoreale *crea_nuova_coppia_zone(void); // FUNZIONE HELPER CREAZIONE
static void genera_mappa(void);
static void inserisci_zona(void);
static void cancella_zona(void);
static void stampa_dettaglio_reale(struct Zona_mondoreale *z);      // FUNZIONE HELPER STAMPA MONDO REALE
static void stampa_dettaglio_soprasotto(struct Zona_soprasotto *z); // FUNZIONE HELPER STAMPA SOPRASOTTO
static void stampa_mappa(void);
static void stampa_zona_gm(void);
static void chiudi_mappa(void);

// Gioco
static void turno_giocatore(int i);
static void avanza(int i);
static void indietreggia(int i);
static void cambia_mondo(int i);
static void combatti(int i, int obbligatorio);
static void raccogli_oggetto(int i);
static void utilizza_oggetto(int i);
static void stampa_giocatore(int i);
static void stampa_zona_gioco(int i);

// --------------------------------------------------------------------------------
// DEFINIZIONE FUNZIONI PUBBLICHE
// --------------------------------------------------------------------------------

void imposta_gioco(void)
{
    pulisci_mappa();
    pulisci_giocatori();
    mappa_chiusa = 0;
    undici_presente = 0;

    // Buffer locale per i messaggi del GM
    sprintf(ultimo_messaggio, "Benvenuto nel Menu GM. Configura la partita.");

    do
    {
        printf("Inserisci il numero di giocatori (1-4): ");
        if (scanf("%d", &numero_giocatori) != 1)
        {
            numero_giocatori = 0;
        }
        svuota_buffer();
    } while (numero_giocatori < 1 || numero_giocatori > 4);

    for (int i = 0; i < numero_giocatori; i++)
    {
        giocatori[i] = (struct Giocatore *)malloc(sizeof(struct Giocatore));

        printf("\n--- Giocatore %d ---\n", i + 1);
        printf("Inserisci nome: ");
        scanf("%19s", giocatori[i]->nome);
        svuota_buffer();

        giocatori[i]->attacco_psichico = (rand() % 20) + 1;
        giocatori[i]->difesa_psichica = (rand() % 20) + 1;
        giocatori[i]->fortuna = (rand() % 20) + 1;
        giocatori[i]->mondo = 0;

        for (int j = 0; j < 3; j++)
        {
            giocatori[i]->zaino[j] = nessun_oggetto;
        }

        printf("Statistiche base -> ATK: %d | DEF: %d | LUCK: %d\n", giocatori[i]->attacco_psichico, giocatori[i]->difesa_psichica, giocatori[i]->fortuna);

        int scelta_valida = 0;
        do
        {
            printf("Scegli classe:\n");
            printf("1) +3 ATK / -3 DEF\n");
            printf("2) +3 DEF / -3 ATK\n");

            if (undici_presente == 0)
            {
                printf("3) UndiciVirgolaCinque\n");
            }
            else
            {
                printf("3) UndiciVirgolaCinque (NON DISPONIBILE)\n");
            }
            printf("4) Nessuna\n");
            printf("Scelta: ");

            int scelta_mod;
            scanf("%d", &scelta_mod);
            svuota_buffer();

            if (scelta_mod == 1)
            {
                giocatori[i]->attacco_psichico += 3;
                giocatori[i]->difesa_psichica -= 3;
                scelta_valida = 1;
            }
            else if (scelta_mod == 2)
            {
                giocatori[i]->difesa_psichica += 3;
                giocatori[i]->attacco_psichico -= 3;
                scelta_valida = 1;
            }
            else if (scelta_mod == 3)
            {
                if (undici_presente == 0)
                {
                    strcpy(giocatori[i]->nome, "UndiciVirgolaCinque");
                    giocatori[i]->attacco_psichico += 4;
                    giocatori[i]->difesa_psichica += 4;
                    giocatori[i]->fortuna -= 7;
                    undici_presente = 1;
                    printf("Sei diventato Undici VirgolaCinque!\n");
                    scelta_valida = 1;
                }
                else
                {
                    printf("\nERRORE: Questa classe e' gia' stata presa da un altro giocatore! Scegline un'altra.\n");
                    scelta_valida = 0;
                }
            }
            else if (scelta_mod == 4)
            {
                printf("Nessuna modifica applicata.\n");
                scelta_valida = 1;
            }
            else
            {
                printf("Opzione non valida, riprova.\n");
                scelta_valida = 0;
            }
        } while (scelta_valida == 0);
    }

    int scelta_gm = 0;
    do
    {
        pulisci_schermo();

        printf("--------------------------------------------------\n");
        printf("           MENU GAME MASTER / CONFIGURAZIONE      \n");
        printf("--------------------------------------------------\n");

        if (strlen(ultimo_messaggio) > 0)
        {
            printf("\n >>> %s <<<\n\n", ultimo_messaggio);
        }

        printf("--------------------------------------------------\n");
        printf(" 1. Genera Mappa (Automatico)\n");
        printf(" 2. Inserisci Zona (Manuale)\n");
        printf(" 3. Cancella Zona\n");
        printf(" 4. Stampa Mappa\n");
        printf(" 5. Stampa Zona (Dettaglio)\n");
        printf(" 6. Chiudi Mappa e Inizia\n");
        printf("==================================================\n");
        printf("Scelta: ");

        scanf("%d", &scelta_gm);
        svuota_buffer();

        // Reset parziale del messaggio (opzionale, o lo sovrascriviamo negli switch)
        // strcpy(ultimo_messaggio, "");

        switch (scelta_gm)
        {
        case 1:
            genera_mappa();
            sprintf(ultimo_messaggio, COL_VERDE "Mappa generata casualmente (15 Zone)." COL_RESET);
            break;
        case 2:
            pulisci_schermo();
            inserisci_zona();
            sprintf(ultimo_messaggio, COL_VERDE "Nuova zona inserita manualmente." COL_RESET);
            break;
        case 3:
            pulisci_schermo();
            cancella_zona();
            sprintf(ultimo_messaggio, COL_GIALLO "Zona cancellata." COL_RESET);
            break;
        case 4:
            pulisci_schermo();
            stampa_mappa();
            premi_invio();
            sprintf(ultimo_messaggio, "Hai visualizzato la mappa completa.");
            break;
        case 5:
            pulisci_schermo();
            stampa_zona_gm();
            premi_invio();
            sprintf(ultimo_messaggio, "Hai ispezionato una zona specifica.");
            break;
        case 6:
            chiudi_mappa();
            if (mappa_chiusa)
            {
                printf(COL_VERDE "Configurazione completata!" COL_RESET "\n");
                premi_invio();
            }
            else
            {
                sprintf(ultimo_messaggio, COL_ROSSO "Errore: Mappa non valida (Serve 15 zone e 1 Boss)." COL_RESET);
            }
            break;
        default:
            sprintf(ultimo_messaggio, "Scelta non valida.");
            break;
        }
    } while (mappa_chiusa == 0);
}

void gioca(void)
{
    if (mappa_chiusa == 0)
    {
        printf(COL_ROSSO "\nERRORE CRITICO: Non hai ancora impostato la mappa!" COL_RESET "\n");
        printf("Vai su 'Imposta Gioco' nel menu principale.\n");

        premi_invio();
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        if (giocatori[i] != NULL)
        {
            giocatori[i]->pos_mondoreale = prima_zona_mondoreale;
            giocatori[i]->pos_soprasotto = prima_zona_soprasotto;
            giocatori[i]->mondo = 0;
        }
    }

    printf("\n--- INIZIO PARTITA ---\n");
    int gioco_in_corso = 1;

    while (gioco_in_corso)
    {
        int ordine[4] = {0, 1, 2, 3};
        for (int k = 0; k < 4; k++)
        {
            int r = rand() % 4;
            int temp = ordine[k];
            ordine[k] = ordine[r];
            ordine[r] = temp;
        }

        int vivi = 0;
        for (int k = 0; k < 4; k++)
        {
            int i = ordine[k];

            if (giocatori[i] != NULL && i < numero_giocatori)
            {
                vivi++;
                azione_movimento_usata = 0;
                turno_giocatore(i);

                if (giocatori[i] == NULL)
                {
                    vivi--;
                }
            }
        }

        if (vivi == 0)
        {
            printf("\nTUTTI MORTI. GAME OVER.\n");
            gioco_in_corso = 0;
        }
    }
}

void termina_gioco(void)
{
    printf("GRAZIE PER AVER GIOCATO!\n");
    pulisci_giocatori();
    pulisci_mappa();
}

void crediti(void)
{
    pulisci_schermo();

    printf("\n--------------------------------------------------\n");
    printf("               C R E D I T I                      \n");
    printf("--------------------------------------------------\n");
    printf(" Sviluppato da: Filippo Fossa\n");
    printf(" Diretto da: Filippo Fossa\n");
    printf(" Sceneggiato da: Filippo Fossa\n");

    printf(" --- HALL OF FAME (Ultimi Vincitori) ---\n");
    printf(" 1. %s%s%s\n", COL_GIALLO, vincitori[0], COL_RESET);
    printf(" 2. %s\n", vincitori[1]);
    printf(" 3. %s\n", vincitori[2]);
    printf("--------------------------------------------------\n");

    premi_invio();
}

// --------------------------------------------------------------------------------
// DEFINIZIONE FUNZIONI DI UTILITY
// --------------------------------------------------------------------------------

static const char *nome_zona(enum Tipo_zona t)
{
    switch (t)
    {
    case bosco:
        return "Bosco";
    case scuola:
        return "Scuola";
    case laboratorio:
        return "Laboratorio";
    case caverna:
        return "Caverna";
    case strada:
        return "Strada";
    case giardino:
        return "Giardino";
    case supermercato:
        return "Supermercato";
    case centrale_elettrica:
        return "Centrale Elettrica";
    case deposito_abbandonato:
        return "Deposito Abbandonato";
    case stazione_polizia:
        return "Stazione Polizia";
    default:
        return "Sconosciuto";
    }
}

static const char *nome_nemico(enum Tipo_nemico n)
{
    switch (n)
    {
    case nessun_nemico:
        return "Nessuno";
    case billi:
        return COL_ROSSO "Billi" COL_RESET;
    case democane:
        return COL_ROSSO "Democane" COL_RESET;
    case demotorzone:
        return COL_VIOLA "DEMOTORZONE" COL_RESET;
    default:
        return "???";
    }
}

static const char *nome_oggetto(enum Tipo_oggetto o)
{
    switch (o)
    {
    case nessun_oggetto:
        return "Vuoto";
    case bicicletta:
        return COL_CIANO "Bicicletta" COL_RESET;
    case maglietta_fuocoinferno:
        return COL_CIANO "Maglietta Fuocoinferno" COL_RESET;
    case bussola:
        return COL_CIANO "Bussola" COL_RESET;
    case schitarrata_metallica:
        return COL_CIANO "Schitarrata Metallica" COL_RESET;
    default:
        return "???";
    }
}

// Funzione che svuota il buffer di input
static void svuota_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

// Funzione che dealloca la memoria dinamica dei della mappa
static void pulisci_mappa(void)
{
    struct Zona_mondoreale *c = prima_zona_mondoreale;
    while (c)
    {
        struct Zona_mondoreale *t = c;
        if (c->link_soprasotto)
            free(c->link_soprasotto);
        c = c->avanti;
        free(t);
    }
    prima_zona_mondoreale = NULL;
    prima_zona_soprasotto = NULL;
}

// Funzione che dealloca la memoria dinamica dei giocatori
static void pulisci_giocatori(void)
{
    for (int k = 0; k < 4; k++)
    {
        if (giocatori[k])
        {
            free(giocatori[k]);
            giocatori[k] = NULL;
        }
    }
}

void stampa_logo(void)
{
    printf("\033[1;31m"); // Colore ROSSO
    printf("   ____ ___  ____  _____    ____ _____ ____      _    _   _ _____\n");
    printf("  / ___/ _ \\/ ___|| ____|  / ___|_   _|  _ \\    / \\  | \\ | | ____|\n");
    printf(" | |  | | | \\___ \\|  _|    \\___ \\ | | | |_) |  / _ \\ |  \\| |  _|\n");
    printf(" | |__| |_| |___) | |___    ___) || | |  _ <  / ___ \\| |\\  | |___\n");
    printf("  \\____\\___/|____/|_____|  |____/ |_| |_| \\_\\/_/   \\_\\_| \\_|_____|\n");
    printf("\n");
    printf("         B E N V E N U T I   A   O C C H I N Z\n");
    printf("\033[0m"); // Reset colore
    printf("==================================================================\n");
}

// Funzione per pulire la console
static void pulisci_schermo(void)
{
    printf(ANSI_CLEAR);
    stampa_logo();
}

// Funzione per mettere in pausa finché non si preme invio
static void premi_invio(void)
{
    printf("\n> Premi INVIO per continuare...");
    getchar(); // Attende un input
}

// --------------------------------------------------------------------------------
// DEFINIZIONE FUNZIONI DI GESTIONE MAPPA
// --------------------------------------------------------------------------------

// Crea una coppia di zone (Reale + Soprasotto) già collegate e con tipo impostato
static struct Zona_mondoreale *crea_nuova_coppia_zone(void)
{
    // Allocazione memoria per entrambe le zone
    struct Zona_mondoreale *nuova_zona_reale = (struct Zona_mondoreale *)malloc(sizeof(struct Zona_mondoreale));
    struct Zona_soprasotto *nuova_zona_soprasotto = (struct Zona_soprasotto *)malloc(sizeof(struct Zona_soprasotto));

    // Impostazione Tipo zone
    nuova_zona_reale->tipo = rand() % 10;
    nuova_zona_soprasotto->tipo = nuova_zona_reale->tipo;

    // Collegamento tra i due mondi
    nuova_zona_reale->link_soprasotto = nuova_zona_soprasotto;
    nuova_zona_soprasotto->link_mondoreale = nuova_zona_reale;

    // Inizializzazione dei puntatori
    nuova_zona_reale->avanti = NULL;
    nuova_zona_reale->indietro = NULL;
    nuova_zona_soprasotto->avanti = NULL;
    nuova_zona_soprasotto->indietro = NULL;

    // Restituzione zona mondo reale con collegamento al soprasotto
    return nuova_zona_reale;
}

static void genera_mappa(void)
{
    pulisci_mappa();

    struct Zona_mondoreale *ultima_zona_reale = NULL;
    struct Zona_soprasotto *ultima_zona_soprasotto = NULL;

    // Scelta casuale di una casella tra la 10 e la 14
    // Questo garantisce che ci sia SEMPRE un solo boss e che sia verso la fine.
    int indice_zona_boss = (rand() % 5) + 10;

    for (int indice_zona = 0; indice_zona < 15; indice_zona++)
    {
        // UTILIZZO DELLA FUNZIONE HELPER
        struct Zona_mondoreale *nuova_zona_reale = crea_nuova_coppia_zone();
        struct Zona_soprasotto *nuova_zona_soprasotto = nuova_zona_reale->link_soprasotto;

        // Nemico Mondo Reale
        if ((rand() % 100) < 60)
        {
            nuova_zona_reale->nemico = (rand() % 2) + 1;
        }
        else
        {
            nuova_zona_reale->nemico = nessun_nemico;
        }

        // Nemico Soprasotto
        if (indice_zona == indice_zona_boss)
        {
            nuova_zona_soprasotto->nemico = demotorzone;
        }
        else
        {
            if ((rand() % 100) < 65)
            {
                nuova_zona_soprasotto->nemico = democane;
            }
            else
            {
                nuova_zona_soprasotto->nemico = nessun_nemico;
            }
        }

        // Oggetto
        if ((rand() % 100) < 50)
        {
            nuova_zona_reale->oggetto = (rand() % 4) + 1;
        }
        else
        {
            nuova_zona_reale->oggetto = nessun_oggetto;
        }

        // inserimento in lista
        if (prima_zona_mondoreale == NULL)
        {
            prima_zona_mondoreale = nuova_zona_reale;
            prima_zona_soprasotto = nuova_zona_soprasotto;
        }
        else
        {
            ultima_zona_reale->avanti = nuova_zona_reale;
            nuova_zona_reale->indietro = ultima_zona_reale;

            ultima_zona_soprasotto->avanti = nuova_zona_soprasotto;
            nuova_zona_soprasotto->indietro = ultima_zona_soprasotto;
        }

        // Aggiornamento puntatori
        ultima_zona_reale = nuova_zona_reale;
        ultima_zona_soprasotto = nuova_zona_soprasotto;
    }
    printf("Mappa generata con successo (15 zone).\n");
}

static void inserisci_zona(void)
{
    // --- UTILIZZO DELLA FUNZIONE HELPER ---
    struct Zona_mondoreale *nuova_zona_reale = crea_nuova_coppia_zone();
    struct Zona_soprasotto *nuova_zona_soprasotto = nuova_zona_reale->link_soprasotto;

    printf("Posizione inserimento (0 per testa): ");
    int posizione_scelta;
    scanf("%d", &posizione_scelta);
    svuota_buffer();

    printf("Nemico Reale (0=Nessuno, 1=Billi, 2=Democane): ");
    int scelta_nemico;
    scanf("%d", &scelta_nemico);

    printf("Oggetto Reale (0=Vuoto, 1=Bici, 2=Maglia, 3=Bussola, 4=Chitarra): ");
    int scelta_oggetto;
    scanf("%d", &scelta_oggetto);
    svuota_buffer();

    nuova_zona_reale->nemico = scelta_nemico;
    nuova_zona_reale->oggetto = scelta_oggetto;

    // Il Soprasotto resta casuale anche nell'inserimento manuale (come da specifiche traccia base)
    nuova_zona_soprasotto->nemico = rand() % 3;

    // Inserimento in lista

    // Inserimento in TESTA o Lista Vuota
    if (posizione_scelta <= 0 || prima_zona_mondoreale == NULL)
    {
        nuova_zona_reale->avanti = prima_zona_mondoreale;
        nuova_zona_soprasotto->avanti = prima_zona_soprasotto;

        if (prima_zona_mondoreale != NULL)
        {
            prima_zona_mondoreale->indietro = nuova_zona_reale;
            prima_zona_soprasotto->indietro = nuova_zona_soprasotto;
        }

        prima_zona_mondoreale = nuova_zona_reale;
        prima_zona_soprasotto = nuova_zona_soprasotto;
    }
    // Inserimento MEZZO o CODA
    else
    {
        struct Zona_mondoreale *corrente_reale = prima_zona_mondoreale;
        struct Zona_soprasotto *corrente_soprasotto = prima_zona_soprasotto;
        int contatore = 0;

        while (contatore < posizione_scelta - 1 && corrente_reale->avanti != NULL)
        {
            corrente_reale = corrente_reale->avanti;
            corrente_soprasotto = corrente_soprasotto->avanti;
            contatore++;
        }

        // Collegamento ai nodi successivi
        nuova_zona_reale->avanti = corrente_reale->avanti;
        nuova_zona_reale->indietro = corrente_reale;

        nuova_zona_soprasotto->avanti = corrente_soprasotto->avanti;
        nuova_zona_soprasotto->indietro = corrente_soprasotto;

        // Ricollegamento dei nodi successivi ai nuovi
        if (corrente_reale->avanti != NULL)
        {
            corrente_reale->avanti->indietro = nuova_zona_reale;
            corrente_soprasotto->avanti->indietro = nuova_zona_soprasotto;
        }

        // Collegamento dei nodi precedenti ai nuovi
        corrente_reale->avanti = nuova_zona_reale;
        corrente_soprasotto->avanti = nuova_zona_soprasotto;
    }
    printf("Zona inserita manualmente con successo.\n");
}

static void cancella_zona(void)
{
    if (!prima_zona_mondoreale)
    {
        printf("La mappa e' vuota, nulla da cancellare.\n");
        return;
    }

    printf("Posizione da cancellare: ");
    int posizione_da_cancellare;
    scanf("%d", &posizione_da_cancellare);
    svuota_buffer();

    struct Zona_mondoreale *zona_corrente_reale = prima_zona_mondoreale;
    struct Zona_soprasotto *zona_corrente_soprasotto = prima_zona_soprasotto;
    int contatore = 0;

    while (contatore < posizione_da_cancellare && zona_corrente_reale->avanti != NULL)
    {
        zona_corrente_reale = zona_corrente_reale->avanti;
        zona_corrente_soprasotto = zona_corrente_soprasotto->avanti;
        contatore++;
    }

    if (contatore < posizione_da_cancellare)
    {
        printf("Errore: Posizione %d non valida (la mappa ha solo %d zone).\n", posizione_da_cancellare, contatore + 1);
        return;
    }

    if (zona_corrente_reale->indietro != NULL)
    {
        zona_corrente_reale->indietro->avanti = zona_corrente_reale->avanti;
        zona_corrente_soprasotto->indietro->avanti = zona_corrente_soprasotto->avanti;
    }
    else
    {
        prima_zona_mondoreale = zona_corrente_reale->avanti;
        prima_zona_soprasotto = zona_corrente_soprasotto->avanti;
    }

    // Aggiornamento puntatori SUCCESSIVI
    if (zona_corrente_reale->avanti != NULL)
    {
        zona_corrente_reale->avanti->indietro = zona_corrente_reale->indietro;
        zona_corrente_soprasotto->avanti->indietro = zona_corrente_soprasotto->indietro;
    }

    // Liberazione della memoria
    free(zona_corrente_reale);
    free(zona_corrente_soprasotto);
    printf("Zona in posizione %d cancellata.\n", contatore);
}

static void chiudi_mappa(void)
{
    int conta_zone = 0;
    int conta_demotorzone = 0;

    // Puntatori per scorrere le liste dall'inizio alla fine
    struct Zona_mondoreale *scorri_reale = prima_zona_mondoreale;
    struct Zona_soprasotto *scorri_soprasotto = prima_zona_soprasotto;

    // Conta il numero totale di zone scorrendo il Mondo Reale
    while (scorri_reale != NULL)
    {
        conta_zone++;
        scorri_reale = scorri_reale->avanti;
    }

    // Cerca il Demotorzone scorrendo il Soprasotto
    while (scorri_soprasotto != NULL)
    {
        if (scorri_soprasotto->nemico == demotorzone)
            conta_demotorzone++;
        scorri_soprasotto = scorri_soprasotto->avanti;
    }

    // Verifica i requisiti: almeno 15 zone e esattamente 1 boss
    if (conta_zone >= 15 && conta_demotorzone == 1)
    {
        mappa_chiusa = 1;
        printf("Mappa chiusa e valida (Zone: %d, Boss: OK).\n", conta_zone);
    }
    else
    {
        printf("Errore: servono min 15 zone (attuali: %d) e 1 Demotorzone (attuali: %d).\n",
               conta_zone, conta_demotorzone);
    }
}

// --------------------------------------------------------------------------------
// FUNZIONI DI GIOCO
// --------------------------------------------------------------------------------

static void turno_giocatore(int i)
{
    int turno_finito = 0;

    // Reset messaggio all'inizio del turno di un nuovo giocatore
    sprintf(ultimo_messaggio, "E' il turno di %s.", giocatori[i]->nome);

    while (turno_finito == 0 && giocatori[i] != NULL)
    {
        pulisci_schermo();

        printf("==================================================\n");
        printf(" GIOCATORE: %-20s | MONDO: %s\n", giocatori[i]->nome, (giocatori[i]->mondo == 0) ? "REALE" : "SOPRASOTTO");
        printf("--------------------------------------------------\n");

        if (strlen(ultimo_messaggio) > 0)
        {
            printf("\n >>> %s <<<\n\n", ultimo_messaggio);
        }

        printf("--------------------------------------------------\n");
        printf(" 1. Avanza           2. Indietreggia\n");
        printf(" 3. Cambia Mondo     4. Combatti\n");
        printf(" 5. Raccogli         6. Usa Oggetto\n");
        printf(" 7. Statistiche      8. Info Zona\n");
        printf(" 9. Passa Turno\n");
        printf("==================================================\n");
        printf("Scelta: ");

        int s;
        scanf("%d", &s);
        svuota_buffer();

        switch (s)
        {
        case 1:
            avanza(i);
            break;
        case 2:
            indietreggia(i);
            break;
        case 3:
            cambia_mondo(i);
            break;
        case 4:
            combatti(i, 0);
            premi_invio();
            sprintf(ultimo_messaggio, "Combattimento terminato.");
            break;
        case 5:
            raccogli_oggetto(i);
            break;
        case 6:
            utilizza_oggetto(i);
            break;
        case 7:
            stampa_giocatore(i);
            premi_invio();
            sprintf(ultimo_messaggio, "Hai controllato lo zaino.");
            break;
        case 8:
            stampa_zona_gioco(i);
            premi_invio();
            sprintf(ultimo_messaggio, "Hai osservato la zona.");
            break;
        case 9:
            turno_finito = 1;
            break;
        default:
            sprintf(ultimo_messaggio, "Comando non valido!");
        }
    }
}
static void combatti(int i, int obbligatorio)
{
    struct Giocatore *g = giocatori[i];
    int nem = (g->mondo == 0) ? g->pos_mondoreale->nemico : g->pos_soprasotto->nemico;

    if (nem == nessun_nemico)
    {
        if (!obbligatorio)
        {
            sprintf(ultimo_messaggio, "Ti sei guardato intorno, ma non c'e' nessuno da combattere.");
        }
        return;
    }

    printf("\n--- INIZIO COMBATTIMENTO contro: %s ---\n", nome_nemico(nem));

    // Ogni nemico ha statistiche variabili entro un range definito per ognuno
    int difesa_nemico = 0;
    int attacco_nemico = 0;

    switch (nem)
    {
    case billi:
        difesa_nemico = 10 + (rand() % 6);
        attacco_nemico = 8 + (rand() % 5);
        break;
    case democane:
        difesa_nemico = 15 + (rand() % 8);
        attacco_nemico = 11 + (rand() % 7);
        break;
    case demotorzone:
        difesa_nemico = 26 + (rand() % 8);
        attacco_nemico = 20 + (rand() % 9);
        break;
    default:
        break;
    }

    int dado_atk = (rand() % 20) + 1;
    int totale_atk = dado_atk + g->attacco_psichico;

    printf("Tuo Attacco: Dado %d + ATK %d = %d (Soglia richiesta: %d)\n",
           dado_atk, g->attacco_psichico, totale_atk, difesa_nemico);

    if (totale_atk >= difesa_nemico)
    {
        printf(COL_VERDE ">>> COLPO A SEGNO! Vittoria!" COL_RESET "\n");

        if (nem == demotorzone)
        {
            printf("\n***********************************************\n");
            printf(" IL DEMOTORZONE E' STATO SCONFITTO DA %s!\n", g->nome);
            printf(" HAI SALVATO OCCHINZ E VINTO IL GIOCO!\n");
            printf("***********************************************\n");

            strcpy(vincitori[2], vincitori[1]);
            strcpy(vincitori[1], vincitori[0]);
            strcpy(vincitori[0], g->nome);

            premi_invio();
            exit(0);
        }

        printf("Il nemico %s cade a terra sconfitto.\n", nome_nemico(nem));

        if (g->mondo == 0)
            g->pos_mondoreale->nemico = nessun_nemico;
        else
            g->pos_soprasotto->nemico = nessun_nemico;

        sprintf(ultimo_messaggio, COL_VERDE "Hai sconfitto il %s!" COL_RESET, nome_nemico(nem));
    }
    else
    {
        printf(">>> HAI MANCATO IL BERSAGLIO! Il nemico contrattacca!\n");

        int dado_def = (rand() % 20) + 1;
        int totale_def = dado_def + g->difesa_psichica;

        printf("Tua Difesa: Dado %d + DEF %d = %d (Soglia salvezza: %d)\n",
               dado_def, g->difesa_psichica, totale_def, attacco_nemico);

        if (totale_def >= attacco_nemico)
        {
            printf(COL_GIALLO "!!! PARATA RIUSCITA! Sei vivo, ma il nemico e' ancora qui." COL_RESET "\n");

            sprintf(ultimo_messaggio, COL_GIALLO "Scontro in stallo: hai parato il colpo del %s." COL_RESET, nome_nemico(nem));
        }
        else
        {
            printf(COL_ROSSO "!!! COLPO MORTALE SUBITO !!!" COL_RESET "\n");
            printf("Sconfitta... %s muore tragicamente.\n", g->nome);

            free(g);
            giocatori[i] = NULL;

            sprintf(ultimo_messaggio, COL_ROSSO "Sei stato ucciso da un %s." COL_RESET, nome_nemico(nem));
        }
    }
    premi_invio();
}

static void avanza(int i)
{
    if (azione_movimento_usata)
    {
        sprintf(ultimo_messaggio, "Hai gia' usato il movimento in questo turno!");
        return;
    }
    if (!giocatori[i])
        return;

    int nemico_presente = (giocatori[i]->mondo == 0) ? giocatori[i]->pos_mondoreale->nemico : giocatori[i]->pos_soprasotto->nemico;
    if (nemico_presente != nessun_nemico)
    {
        sprintf(ultimo_messaggio, "Il nemico ti blocca la strada! Devi sconfiggerlo.");
        return;
    }

    if (giocatori[i]->mondo == 0)
    {
        if (giocatori[i]->pos_mondoreale->avanti)
        {
            giocatori[i]->pos_mondoreale = giocatori[i]->pos_mondoreale->avanti;
            giocatori[i]->pos_soprasotto = giocatori[i]->pos_mondoreale->link_soprasotto;
            azione_movimento_usata = 1;
            sprintf(ultimo_messaggio, COL_VERDE "Sei avanzato nel Mondo Reale." COL_RESET);
        }
        else
            sprintf(ultimo_messaggio, "Sei alla fine della mappa, non puoi avanzare.");
    }
    else
    {
        if (giocatori[i]->pos_soprasotto->avanti)
        {
            giocatori[i]->pos_soprasotto = giocatori[i]->pos_soprasotto->avanti;
            giocatori[i]->pos_mondoreale = giocatori[i]->pos_soprasotto->link_mondoreale;
            azione_movimento_usata = 1;
            sprintf(ultimo_messaggio, "Sei avanzato nel Soprasotto.");
        }
        else
            sprintf(ultimo_messaggio, "Sei alla fine della mappa, non puoi avanzare.");
    }
}

static void indietreggia(int i)
{
    if (!giocatori[i])
        return;

    int nemico_presente = (giocatori[i]->mondo == 0) ? giocatori[i]->pos_mondoreale->nemico : giocatori[i]->pos_soprasotto->nemico;
    if (nemico_presente != nessun_nemico)
    {
        sprintf(ultimo_messaggio, "Non puoi ritirarti con un nemico davanti!");
        return;
    }

    if (giocatori[i]->mondo == 0)
    {
        if (giocatori[i]->pos_mondoreale->indietro)
        {
            giocatori[i]->pos_mondoreale = giocatori[i]->pos_mondoreale->indietro;
            giocatori[i]->pos_soprasotto = giocatori[i]->pos_mondoreale->link_soprasotto;
            sprintf(ultimo_messaggio, "Sei indietreggiato nel Mondo Reale.");
        }
        else
            sprintf(ultimo_messaggio, "Sei all'inizio della mappa.");
    }
    else
    {
        if (giocatori[i]->pos_soprasotto->indietro)
        {
            giocatori[i]->pos_soprasotto = giocatori[i]->pos_soprasotto->indietro;
            giocatori[i]->pos_mondoreale = giocatori[i]->pos_soprasotto->link_mondoreale;
            sprintf(ultimo_messaggio, "Sei indietreggiato nel Soprasotto.");
        }
        else
            sprintf(ultimo_messaggio, "Sei all'inizio della mappa.");
    }
}

static void cambia_mondo(int i)
{
    if (giocatori[i]->mondo == 0)
    {
        if (azione_movimento_usata)
        {
            sprintf(ultimo_messaggio, "Gia' mosso in questo turno.");
            return;
        }
        if (!giocatori[i])
            return;

        if (giocatori[i]->pos_mondoreale->nemico != nessun_nemico)
        {
            sprintf(ultimo_messaggio, "Non puoi cambiare mondo con un nemico vicino!");
            return;
        }

        giocatori[i]->mondo = 1;
        azione_movimento_usata = 1;
        sprintf(ultimo_messaggio, "Sei stato catapultato nel SOPRASOTTO!");
    }
    else
    {
        if ((rand() % 20) + 1 < giocatori[i]->fortuna)
        {
            giocatori[i]->mondo = 0;
            sprintf(ultimo_messaggio, "Sei riuscito a scappare nel Mondo Reale!");
        }
        else
            sprintf(ultimo_messaggio, "Fuga fallita! Sei bloccato nel Soprasotto.");
    }
}

static void raccogli_oggetto(int i)
{
    struct Giocatore *g = giocatori[i];
    if (g->mondo == 1)
    {
        sprintf(ultimo_messaggio, "Non ci sono oggetti nel Soprasotto.");
        return;
    }
    if (g->pos_mondoreale->nemico != nessun_nemico)
    {
        sprintf(ultimo_messaggio, "Devi prima sconfiggere il nemico!");
        return;
    }
    if (g->pos_mondoreale->oggetto == nessun_oggetto)
    {
        sprintf(ultimo_messaggio, "Qui non c'e' nulla da raccogliere.");
        return;
    }

    for (int k = 0; k < 3; k++)
    {
        if (g->zaino[k] == nessun_oggetto)
        {
            g->zaino[k] = g->pos_mondoreale->oggetto;
            const char *nome_obj = nome_oggetto(g->pos_mondoreale->oggetto);
            g->pos_mondoreale->oggetto = nessun_oggetto;

            sprintf(ultimo_messaggio, "Hai raccolto: %s (Slot %d)", nome_obj, k + 1);
            return;
        }
    }
    sprintf(ultimo_messaggio, "Il tuo zaino e' pieno!");
}

static void utilizza_oggetto(int i)
{
    struct Giocatore *g = giocatori[i];

    pulisci_schermo();
    printf("--- INVENTARIO ---\n");
    printf("0: %s | 1: %s | 2: %s\n", nome_oggetto(g->zaino[0]), nome_oggetto(g->zaino[1]), nome_oggetto(g->zaino[2]));
    printf("Scegli slot (0-2): ");

    int s;
    scanf("%d", &s);
    svuota_buffer();

    if (s < 0 || s > 2 || g->zaino[s] == nessun_oggetto)
    {
        sprintf(ultimo_messaggio, "Slot vuoto o non valido.");
        return;
    }

    switch (g->zaino[s])
    {
    case bicicletta:
        g->fortuna += 2;
        sprintf(ultimo_messaggio, "Hai usato Bicicletta: Fortuna +2");
        break;
    case bussola:
        azione_movimento_usata = 0;
        sprintf(ultimo_messaggio, "Hai usato Bussola: Azione recuperata!");
        break;
    case maglietta_fuocoinferno:
        g->difesa_psichica += 2;
        sprintf(ultimo_messaggio, "Hai usato Maglietta: Difesa +2");
        break;
    case schitarrata_metallica:
        g->attacco_psichico += 2;
        sprintf(ultimo_messaggio, "Hai usato Schitarrata: Attacco +2");
        break;
    default:
        break;
    }
    g->zaino[s] = nessun_oggetto;
}

static void stampa_giocatore(int i)
{
    struct Giocatore *g = giocatori[i];
    printf("Nome: %s | ATK: %d | DEF: %d | LUCK: %d\n", g->nome, g->attacco_psichico, g->difesa_psichica, g->fortuna);
    printf("Zaino: [%s] [%s] [%s]\n", nome_oggetto(g->zaino[0]), nome_oggetto(g->zaino[1]), nome_oggetto(g->zaino[2]));
}

// funzione ceh varia a seconda della presenza di un nemico poiché tiene conto dei caratteri ANSI
static void stampa_dettaglio_reale(struct Zona_mondoreale *z)
{
    if (z == NULL)
        return;

    printf("Reale - Zona: %-25s | Nemico: ", nome_zona(z->tipo));

    if (z->nemico == nessun_nemico)
    {
        printf("%-14s", nome_nemico(z->nemico));
    }
    else
    {
        printf("%-25s", nome_nemico(z->nemico));
    }

    printf(" | Oggetto: %s\n", nome_oggetto(z->oggetto));
}

static void stampa_dettaglio_soprasotto(struct Zona_soprasotto *z)
{
    if (z == NULL)
        return;
    printf("Soprasotto - Zona: %-25s | Nemico: %s\n", nome_zona(z->tipo), nome_nemico(z->nemico));
}

static void stampa_zona_gioco(int i)
{
    if (giocatori[i]->mondo == 0)
    {
        stampa_dettaglio_reale(giocatori[i]->pos_mondoreale);
    }
    else
    {
        stampa_dettaglio_soprasotto(giocatori[i]->pos_soprasotto);
    }
}

static void stampa_mappa(void)
{
    printf("Mondo (0: Reale, 1: Soprasotto): ");
    int m;
    scanf("%d", &m);
    svuota_buffer();

    printf("\n--- Mappa del Mondo %s ---\n", (m == 0) ? "Reale" : "Soprasotto");

    if (m == 0)
    {
        struct Zona_mondoreale *t = prima_zona_mondoreale;
        int count = 0;
        while (t)
        {
            printf("[%d] \t", count++);
            stampa_dettaglio_reale(t);
            t = t->avanti;
        }
    }
    else
    {
        struct Zona_soprasotto *t = prima_zona_soprasotto;
        int count = 0;
        while (t)
        {
            printf("[%d] \t", count++);
            stampa_dettaglio_soprasotto(t);
            t = t->avanti;
        }
    }
    printf("--- Fine Mappa ---\n");
}

static void stampa_zona_gm(void)
{
    printf("Posizione: ");
    int pos;
    scanf("%d", &pos);
    svuota_buffer();

    struct Zona_mondoreale *t = prima_zona_mondoreale;
    for (int k = 0; k < pos && t; k++)
        t = t->avanti;

    if (t)
    {
        stampa_dettaglio_reale(t);
        stampa_dettaglio_soprasotto(t->link_soprasotto);
    }
    else
    {
        printf("Zona non trovata.\n");
    }
}