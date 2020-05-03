# labSO-project 2020

## Main
- Interaccia utente
- Comandi:
    - Crea report
        - Quanti P e Q
        - Lista di file da analizzare
        - Scelta tipo visualizzazione report
        - Possibilità di aggiungere altri file e modificare
        - Possibilità di modificare P e Q
        - Ricalcola
        - Chiudi

## Report
- Riceve dati da A e li formatta a video

## Analyzer
- Prende da M file e cartelle, m e n
- Crea una lista di file
- La inoltra a C
- Se arrivano altri file/cartelle li compara alla lista
- Invia i file aggiunti a C
- Possibilità di reset

## Counter
- Riceve da A una lista di file e m e n
- Divide i file in m gruppi
- Crea m P e gli passa i file
- Se riceve altri file gli passa a P a ciclo
- se cambia m uccide quelli in più, o ne genera nuovi
- Riceve i dati dai P e li somma

## P
- Riceve i file (a gruppi) da C e n
- crea i n Q
- inoltra i file a Q
- riceve il risultato dai Q e li somma
- ritorna il risultato a C
- se n cambia crea o rimuove Q

## Q
- Riceve i file (a gruppi) da P e la sua parte di file
- analizza i file
- ritorna il risultato
- Puo essere cambiata la sua parte




