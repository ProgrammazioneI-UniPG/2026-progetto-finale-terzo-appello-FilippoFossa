// Dichiarazione delle funzioni da chiamare in main.c
void stampa_logo(void);
void imposta_gioco(void);
void gioca(void);
void termina_gioco(void);
void crediti(void);

// Definizione macro ANSI
#define COL_RESET "\033[0m"
#define COL_ROSSO "\033[1;31m"
#define COL_VERDE "\033[1;32m"
#define COL_GIALLO "\033[1;33m"
#define COL_BLU "\033[1;34m"
#define COL_VIOLA "\033[1;35m"
#define COL_CIANO "\033[1;36m"
#define ANSI_CLEAR "\033[H\033[2J\033[3J"

// Enum per definizione tipologia zona
typedef enum Tipo_zona
{
    bosco,
    scuola,
    laboratorio,
    caverna,
    strada,
    giardino,
    supermercato,
    centrale_elettrica,
    deposito_abbandonato,
    stazione_polizia
} Tipo_zona;

// Enum per definizione tipo nemico
typedef enum Tipo_nemico
{
    nessun_nemico,
    billi,
    democane,
    demotorzone
} Tipo_nemico;

// Enum per definizione tipo oggetto
typedef enum Tipo_oggetto
{
    nessun_oggetto,
    bicicletta,
    maglietta_fuocoinferno,
    bussola,
    schitarrata_metallica
} Tipo_oggetto;

typedef enum bool
{
    true = 1,
    false = 0
} bool;

// Strcut per definizione giocatore
typedef struct Giocatore
{
    char nome[20];
    short int mondo; // 0 = mondoreale,   1 = soprasotto
    struct Zona_mondoreale *pos_mondoreale;
    struct Zona_soprasotto *pos_soprasotto;
    unsigned short int attacco_psichico;
    unsigned short int difesa_psichica;
    unsigned short int fortuna;
    enum Tipo_oggetto zaino[3];
} Giocatore;

// Strcut per definizione zona mondoreale
typedef struct Zona_mondoreale
{
    enum Tipo_zona tipo;
    enum Tipo_nemico nemico;
    enum Tipo_oggetto oggetto;
    struct Zona_mondoreale *indietro;
    struct Zona_mondoreale *avanti;
    struct Zona_soprasotto *link_soprasotto;
} Zona_mondoreale;

// Strcut per definizione zona soprasotto
typedef struct Zona_soprasotto
{
    enum Tipo_zona tipo;
    enum Tipo_nemico nemico;
    struct Zona_soprasotto *indietro;
    struct Zona_soprasotto *avanti;
    struct Zona_mondoreale *link_mondoreale;
} Zona_soprasotto;
