# project-allocator
Proiect PC 2017
Alocator de Memorie
Hermeniuc Stefan-Alexandru 314CA




Programul nostru va simula memoria unui calculator folosind un vector de tip 
unsigned char. Acesta trebuie sa indeplineasca functii elementare, necesare 
unei aplicatii de acest gen precum initializarea arena (termenul pe care 
il vom folosi de acum pentru a denumi vectorul de date), finalizarea 
programului (eliberarea memoriei alocate initial pentru arena), DUMP 
(care afiseaza harta de memorie intr-un format specific, similar unor 
aplicatii care afiseaza continutul unor fisiere binare), ALLOC (care 
rezerva o anumita zona de memorie compacta, continua pe care utilizatorul 
o poate folosi), FREE (care elibereaza o zona rezervata anterior), FILL 
(care adauga date in zone rezervare), SHOW care afiseaza informatii 
statistice despre starea arenei. In implementarea acestor comenzi am definit 
diverse functii:




a) INITIALIZE <N>: Aceasta functie aloca dinamic N octeti in memoria 
"reala" a calculatorului si o initializeaza cu 0 folosind functia 
predefinita din biblioteca stdlib.h calloc, daca se poate.



b) FINALIZE: Functia elibereaza memoria alocata initial pentru arena si 
finalizeaza rularea programului.



c) DUMP: Functia afiseaza continutul efectiv al arenei. Pentru 
implementare am iterat prin arena si am afisat pe fiecare linie cate 
16 elemente, cu spatiu intre al 8-lea si al 9lea element. Am observat ca
elementele de pe pozitia a 8a de pe fiecare rand au in comun faptul ca 
restul impartirii la 8 este 7 (indicele de inceput este 0 deci pozitia a 
8a este de fapt a 7a). A trebuit sa afisam la fiecare 16 elemente indicele 
curent, in inceputul liniei. La final am afisat dimensiunea arenei. Toate 
afisarile au fost facute in format hexazecimal cu 2 sau 8 cifre.



d) ALLOC <SIZE>: Aceasta functie are ca rol adaugarea unui bloc intr-o 
zona libera, care e suficient de incapatoare incat sa tina size octeti 
plus o zona de gestiune formata din 3 numere intregi (4 octeti fiecare), 
care contine indicele urmatorului bloc, indicele blocului precedent si 
lungima totala a blocului. Se pot distinge niste cazuri care trebuie 
tratate separat fiecare. Daca arena este goala si suficient de mare 
alocam un bloc imediat dupa zona de gestiune 0. Atat indicele blocului 
precedent cat si indicele blocului urmator ai zonei nou alocate vor fi 
setati pe 0. Daca arena nu este goala facem o serie de cautari pana 
cand gasim primul loc convenabil pentru alocare. Daca este suficent 
spatiu intre primul bloc alocat si zona 0, alocam un bloc imediat dupa 
prima zona de gestiune, modificand indicele de inceput si indicele 
blocului precedent al zonei urmatoare de gestiune. In caz contrar, 
cautam spatiu intre oricare 2 blocuri din arena. Daca gasim suficient 
loc pentru a aloca un bloc intre ele le updatam zonele de gestiune astfel 
incat primii 4 octeti de gestiune ai primului bloc si octetii 4-8 ai 
celui de al doilea bloc contin indicele de start al blocului nou alocat. 
Daca nici in acest caz nu gasim cautam intre ultimul bloc si finalul arenei, 
facand modificarile necesare pentru zona de gestiune a ultimului bloc 
intr-un caz favorabil. In toate cazurile vom returna indicele de inceput 
al zonei de date (nu al zonei de gestiune) al blocului nou alocat folosind 
lungimea blocului precedent. Daca, in final, nu ne-am incadrat in niciunul 
dintre cazurile de mai sus, functia va intoarce valoarea 0, intrucat nu am 
gasit nicaieri suficient spatiu sa alocam un bloc nou de memorie.



e) FREE <INDEX>: Functia elibereaza ("dealoca") un bloc de memorie care incepe 
cu un index returnat de apelul functiei ALLOC, deci unul care indica inceputul 
zonei de gestiune a utilizatorului. Distingem 4 cazuri: cand eliberam singurul 
bloc din arena (zonele care indica indicii vecinilor sunt 0), cand eliberam 
primul bloc din arena, care nu e unic (zona care indica indicele urmatorului 
bloc e diferita de 0 si cea care indica indicele blocului precedent e 0), cand
eliberam un bloc nemarginas, cand eliberam ultimul bloc. In toate acestea nu 
facem altceva decat sa modificam zonele de gestiune ale vecinilor. Nu setam 
pe 0 nimic din fostul bloc alocat.


f)FILL <INDEX> <SIZE> <VALUE>: Functia va mofifica <SIZE> octeti, incepand cu
octetul care are indexul <INDEX>, setandu-i la valoarea <VALUE>. Pentru aceasta
iteram de la <INDEX> la <INDEX + SIZE> - 1 setand valorile din arena la <VALUE>. 



g) SHOW: Aceasta functie afiseaza informatii statistice despre arena; are mai 
multe subfunctii:

	0) In implementarea functiei show am definit o functie numita 
	count_free() care numara cate zone libere(nealocate) sunt in arena
	 noastra. Aceasta functie parcurge arena si verifica daca spatiul 
	intre oricare 2 blocui sau intre finalul arenei si ultimul bloc 
	este mai mare decat 0. In caz afirmativ numarul de blocuri libere 
	creste cu 1.


	1) FREE: functia afiseaza numarul de zone si octeti liberi. Aceasta 
	merge pe aceeasi idee ca si count_free: in momentul in care intre 
	oricare 2 blocuri spatiul este mai mare de 0  atunci aceasta diferenta
	este adaugata la suma de octeti liberi.


	2) USAGE: functia afiseaza numarul de octeti folositi din arena 
	si dimensiunea acestora(doar octeti cu datele utilizatorului, 
	fara octeti de gestiune), eficienta (numarul de octeti folositi 
	raportat la numarul de octeti rezervati) si fragmentarea (numarul 
	de zone libere (aflat cu functia count_free) raportat la numarul 
	de blocuri alocate). Folosind faptul ca primii 4 octeti din zona 
	de gestiune contin indicele urmator si ca octetii 8-12 din zona 
	de gestiune contin dimensiunea blocului parcurgem arena si calculam 
	suma octetilor folositi, suma octetilor rezervati si numarul de 
	blocuri alocate. Afisam apoi rapoartele cerute.


	3) ALLOCATIONS: Functia va afisa zonele libere si alocate 
	(si dimensiunea lor) in ordinea in care acestea apar in arena. 
	In orice caz, primii 4 octeti ai arenei sunt ocupati, acestia fiind 
	prima zona de gestiune. In cazul unei arene goale avem exact 
	N - sizeof(int) octeti liberi. In continuare, verificam afisam 
	OCCUPIED si dimensiunea fiecarui bloc si FREE in cazul in care intre 
	oricare 2 blocuri avem spatiu liber.



h) In final, avem o functie cmd_execute() prin care citim comenzile de la stdin 
si le executam, apeland functiile mai sus definite.
	

