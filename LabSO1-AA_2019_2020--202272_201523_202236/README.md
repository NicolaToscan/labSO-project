LabSO1-AA_2019_2020--202272_201523_202236

# Componenti gruppo:
- Alessio Gandelli, alessio.gandelli@studenti.unitn.it, 202272
- Lorenzo Godi, lorenzo.godi@studenti.unitn.it, 201523
- Nicola Toscan, nicola.toscan@studenti.unitn.it, 202236

# Informazioni
A seguire una guida su come utilizzare l'applicazione e una descrizione base sulla sua struttura.

## Come testare il progetto
- M dispone di un comando help per conoscere tutti i comandi disponibili
- Sequenza
  - Avvia M (Main)
  - Cambia numero di P e Q
  - Aggiungi/Rimuovi file
  - Avvia R (Report)
  - Richiedi report
- Inoltre, tutte le operazioni (tranne l'avvio di M) sono ripetibili più volte in qualunque ordine (esempio: dopo aver chiesto un report, aggiungo altri file, cambio P e/o Q per poi chiedere un altro report)

## Scelte implementative
- Ampio utilizzo di pipe anonime per la comunicazione tra processi
- Utilizzo di threads in alcuni casi per evitare deadlocks

## Struttura dell'applicazione
L'applicazione è composta da sei processi/sottosistemi:
- **Main**
	- Interfaccia utente
	- Avvia sottoprocessi Report e Analyzer
- **Report**
	- Riceve da Main istruzioni e aggiornamanenti sui files
	- Riceve da Analyzer le analisi dei files
	- Crea report dettagliati su richiesta
- **Analyzer**
	- Avvia sottoprocesso Counter a cui inoltra files e variabili n, m
	- Riceve da Counter le analisi "grezze" e le invia a Report
- **Counter**
	- Genera sottoprocessi P in numero variabile e gli assegna gruppi di files
- **P**
	- Genera sottoprocessi Q in numero variabile e gli assegna parti di files
- **Q**
	- Calcola le analisi parziali dei files

Sono presenti inoltre quattro librerie:
- **Analisys**
  - Lettura dei file, calcolo analisi e stampa analisi
- **Commands**
  - Raccolta di comandi standard usati per comunicazioni tra processi (header - only)
- **Common**
  - Raccolta di define, typedef e funzioni comuni
- **Communication**
  - Insieme di funzioni standard per invio e ricezione di dati tra processi