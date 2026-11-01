# Evil Portal — template (cloni captive portal)

Solo per **test autorizzati** (reti tue, audit con permesso scritto, CTF, security awareness).

Ogni `*.html` qui dentro compare nella lista template dell'app Evil Portal sul Cardputer.
Percorso sul device: `/sd/evilportal/portals/`.

## Regole
- **Tutto inline**: niente CSS/JS/immagini esterni — un client captive non ha internet, quindi
  asset remoti non caricano. Metti stile e immagini (data URI) dentro l'HTML.
- Il form deve fare `method="POST" action="/login"` (oppure GET con query): qualunque campo
  inviato viene salvato in `/sd/evilportal/loot/`.
- I nomi dei campi guidano l'etichettatura: chi contiene `pass/pwd/pin` = password; `user/email/
  account/tel/...` = utente. Tutto il resto è comunque salvato grezzo.
- `{{SSID}}` viene sostituito a runtime col SSID realmente trasmesso.

## Organizzazione per lingua (sottocartelle)
Oltre ai `*.html` in questa cartella, l'engine elenca anche i `*.html` **un livello sotto**, in
sottocartelle per lingua: `portals/en/airport.html` compare come `en/airport`, `portals/pt-br/...`
come `pt-br/...`. La cartella `a/` (asset dei cloni live) è esclusa. I file flat continuano a
funzionare: le sottocartelle servono solo a tenere ordinati pacchetti per lingua.

I template in `en/` qui inclusi sono archetipi **generici** (aeroporto, bar, conferenza, centro
commerciale, coworking) — non imitano un brand reale. Per cloni brand-accurate usa il clone live
(F1) da un target autorizzato oppure il generatore LLM `tools/evilportal/generate.mjs`.

## Aggiungere un clone
Copia un `.html` qui (o in una sottocartella lingua) e risincronizza la SD (`tools/sd-sync.ps1`).

Le catture finiscono in `/sd/evilportal/loot/` (una CSV per sessione + `all-credentials.csv`).
