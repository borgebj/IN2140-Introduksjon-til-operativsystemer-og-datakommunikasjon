# MSN

Et enkelt program som viser hvordan datakommunikasjon kan foregå i C gjennom socket programmering.

For å kjøre:

- Trenger 2 terminal-vinduer:
> kan bruke shift + høyreklikk, og åpne i powershell/linux shell
> i addresse i filutforsker, fjern og skriv 'cmd'

instruksjoner:
1. Hent ip-addresse
	linux:   'hostname -i'
	windows: 'ipconfig' ?

2. skrive i terminal 1 : './msn 1234 4321 <ip>'
3. skrive i terminal 2 : './msn 4321 1234 <ip>'      

- 1234 og 4321 er vilkårlige port-nummer, kan være hva som helst
