[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/5fsIc7xe)
# Progetto-finale-2025-Cosestrane
Progetto finale Programmazione Procedurale UniPG Informatica

## Nome:        Filippo

## Cognome:     Fossa

## Matricola:   391808

È stato implementato un sistema di gestione dell'output che simula un'interfaccia grafica persistente sul terminale. 
La schermata di gioco è ora suddivisa in aree logiche distinte:

Header e Notifiche: La parte superiore è riservata alle comunicazioni di stato, serve a garantire che il giocatore rimanga informato su qunato accade e su cosa non si può fare

Area Comandi: Sezione che rimane invariata durante tutto il corso del gioco, sono presenti i comandi a disposizione del giocatore

Area Log: L'area riservata ad informazioni estese che necessitano di più tempo per essere lette come l'esito dei combattimenti o la consultazione delle statistiche riguardanti il giocatore.
Vengono mostrate sotto i comandi e gestite tramite un sistema di pausa ("Premi Invio per continuare") implementato con una funzione di utlity permettendo ai giocatori di leggere dati con calma prima del refresh.

Questo sistema è stato implementato grazie all'utilizzo della funzione sprint() e di un buffer ultimo_messaggio[512] per quanto riguarda il buffering dei messaggi mentre per la pulizia dello schermo ad ogni refresh è stata utilizzata la sequenza Escape ANSI