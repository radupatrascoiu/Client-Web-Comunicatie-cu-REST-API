**Pătrășcoiu Ion - Radu, 322CD**

### Descriere Client Web

    - in implementarea temei am folosit o parte din scheletul de laborator, la
care am adaugat functia *compute_delete_request*, care trimite un request de
tip *delete*. De asemenea am folosit biblioteca sugerata **nlohmann**, intrucat
tema este implementata in C++;

    - am plecat cu 3 stringuri care au acelasi continut `*`, astfel se poate
verifica daca s-a primit cookie-ul bun si/sau token-ul JWT, prin compararea cu
*flag_to_check*;

    - aplicatia cicleaza intr-un while(1), pana la aparitia comenzii **exit**,
in acest timp se citesc comenzile de la tastatura si se executa pe rand.
Aparitia unei comenzi necunoscute este ignorata de program;

    - la fiecare comanda se afiseaza mesaje specifice: de adaugare a unor date,
erori de input sau mesaje de avertizare. Pentru fiecare comanda se trimite un
tip de request catre server, construit cu ajutorul bibliotecii precizate mai
sus, se verifica prezenta **cookie-ului** potrivit in cazul *login-ului* si al
*enter_library-ului*, si a **token-ului JWT** in cazul comenzilor din
interiorul librariei(*get_books*, *get_book*, *add_book*, *delete_book*),
aceasta fiind o metoda in plus de securitate;

    - parsarea raspunsului primit de la server s-a realizat tot prin
intermediul bibliotecii precizate, utilizand functia **json::parse**, pentru un
output mai lizibil. Pentru fiecare raspuns se verifica daca exista vreo eroare,
in caz afirmativ se afiseaza un mesaj personalizat(valabil atat pentru
permisiunile de accesare, cat si pentru actiunile din interiorul librariei).

    - 
