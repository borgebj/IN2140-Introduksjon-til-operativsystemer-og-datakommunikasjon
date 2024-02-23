<h1> IN2140 </h1>
Denne repositorien inneholder filer knyttet til IN2140 UiO. <br>
Filer er sortert basert på bl.a. ukenummer

<h2> C programmering </h2>
<b>Makefile</b> er filen brukt for å automatisere kompilering av C-programmene. <br>


<h3> Kompilering gjennom makefile </h3>

|     |  |    |
| --- | -------------- | ------------- |
| 1.  | `make #FILNAVN#` | (kompilering) |
| 2.  | `./#OUTNAVN#`    | (kjøring)      |

<h3> Manuell kompilering </h3>

|     |                              |               |
| --- | ---------------------------- | ------------- |
| 1.  | `gcc #FILNAVN#.c -o #UTNAVN#` | (kompilering) |
| 1.1 | `gcc -std=gnu11 #FILNAVN#.c -o #UTNAVN#` | (kompilering) (C11 standard)
| 1.2 | `gcc -Wall -Wextra #FILNAVN#.c -o #UTNAVN#` | (kompilering) med flagg for error-meldinger|
| 2.  | `./#OUTNAVN#`                 | (kjøring)     |

<b> -o </b> er flagg for å spesifisere "output" navn <br>
<b> gcc </b> er kompilator for C/C++ (skaffet gjennomt mingw64)
