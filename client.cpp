#include "helpers.h"

int main() {
  int sockfd;
  //* obtinere ip
  char *ip = inet_ntoa((struct in_addr) *
                       ((struct in_addr *)gethostbyname(
                            "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com")
                            ->h_addr_list[0]));
  char *message;
  char *response;
  //* se pune caracterul '*' in fiecare string pentru a verificat ulterior
  //* daca s-a modificat, in caz afirmativ s-a primit cookie-ul sau JWT-ul
  string autorization = "*";
  string token_JWT = "*";
  string flag_to_check = "*";

  //* se va iesi la tastarea comenzii 'exit'
  while (1) {
    //* se citesc comenzile de la consola
    char *command = (char *)malloc(50 * sizeof(char));
    fgets(command, 50, stdin);

    if (!strncmp(command, "register", 8)) {
      string username;
      cout << "username=";
      std::getline(std::cin, username);

      string password;
      cout << "password=";
      std::getline(std::cin, password);

      // ! POST REQUEST

      //* se construieste json-ul
      json j = {
          {"username", username},
          {"password", password},
      };

      //* se construieste mesajul de post_request ce va trimis catre server
      message = compute_post_request(
          (const char *)"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
          (const char *)"/api/v1/tema/auth/register",
          (const char *)"application/json", j.dump(), 1, NULL, 0);

      //* se deschide conexiunea cu serverul
      sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

      //* se trimite mesajul format catre server pe socketul deschis anterior
      send_to_server(sockfd, message);

      //* primire raspuns de la server
      response = receive_from_server(sockfd);

      //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
      //* un mesaj corespunzator
      char *ptr = strstr(response, "error");
      if (ptr != NULL) {
        cout << "Error! The username " << username << " is taken!" << endl;
      } else {
        cout << "User added successfully!" << endl;
      }

      //* inchidere conexiune
      close_connection(sockfd);

    } else if (!strncmp(command, "login", 5)) {
      string username;
      cout << "username=";
      std::getline(std::cin, username);

      string password;
      cout << "password=";
      std::getline(std::cin, password);

      // ! POST REQUEST

      //* se construieste json-ul
      json json_message = {
          {"username", username},
          {"password", password},
      };

      //* se construieste mesajul de post_request ce va trimis catre server
      message = compute_post_request(
          "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
          "/api/v1/tema/auth/login", "application/json", json_message.dump(), 1,
          NULL, 0);

      //* se deschide conexiunea cu serverul
      sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

      //* se trimite mesajul format catre server pe socketul deschis anterior
      send_to_server(sockfd, message);

      //* primire raspuns de la server
      response = receive_from_server(sockfd);

      //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
      //* un mesaj corespunzator
      char *ptr = strstr(response, "error");
      if (ptr != NULL) {
        cout << "Error! Credentials are not good!" << endl;
      } else {
        //* se extrage cookie-ul
        std::string str(response);
        unsigned first = str.find("Cookie");
        unsigned last = str.find("Path");

        autorization = str.substr(first, last - first - 2);
      }

      //* inchidere conexiune
      close_connection(sockfd);

    } else if (!strncmp(command, "enter_library", 13)) {
      // ! GET REQUEST

      //* se verifica daca s-a primit cookie-ul potrivit
      if (autorization.compare(flag_to_check) == 0) {
        cout << "Error! You are not logged in!" << endl;
      } else {
        string cookies[1];
        cookies[0] = autorization;

        //* se construieste mesajul de get_request ce va fi trimis catre server
        message = compute_get_request(
            (const char *)"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
            (const char *)"/api/v1/tema/library/access", NULL, cookies, 1);

        //* se deschide conexiunea cu serverul
        sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

        //* se trimite mesajul format la server
        send_to_server(sockfd, message);

        //* primire mesaj
        response = receive_from_server(sockfd);

        //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
        //* un mesaj corespunzator
        char *ptr = strstr(response, "error");
        if (ptr != NULL) {
          cout << "Error! You are not logged in!" << endl;
        } else {
          //* se parseaza token-ul primit
          std::string str(response);
          unsigned first = str.find("token");
          unsigned last = str.find("}");

          //* se formeaza token-ul JWT
          token_JWT = "";
          token_JWT = token_JWT + "Authorization: Bearer " +
                      str.substr(first + strlen("token:") + 2,
                                 last - first - strlen("token:") - 3);
        }

        //* se inchide conexiunea cu serverul
        close_connection(sockfd);
      }

    } else if (!strncmp(command, "get_books", 9)) {
      // ! GET REQUEST

      //* se verifica daca s-a primit token-ul JWT potrivit, in caz negativ
      //* se afiseaza un mesaj de eroare
      if (token_JWT.compare(flag_to_check) == 0) {
        cout << "Error! You do not have access to the library!" << endl;
      } else {
        string tokens[1];
        tokens[0] = token_JWT;

        //* se construieste mesajul de get_request ce va fi trimis catre server
        message = compute_get_request(
            (const char *)"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
            (const char *)"/api/v1/tema/library/books", NULL, tokens, 1);

        //* deschidere conexiune cu serverul
        sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

        //* se trimite mesajul catre server
        send_to_server(sockfd, message);


        //* primire raspuns de la server
        response = receive_from_server(sockfd);

        //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
        //* un mesaj corespunzator
        char *ptr = strstr(response, "error");
        if (ptr != NULL) {
          cout << "Error! Something bad happened!" << endl;
        } else {
          //* se parseaza cartile primite
          std::string str(response);

          //* daca nu exita setul de paranteze inseamna ca
          //* nu exista carti adaugate
          if (str.find("[{") != std::string::npos &&
              str.find("}]") != std::string::npos) {
            unsigned first = str.find("[{");
            unsigned last = str.find("}]");
            string to_print = str.substr(first, last - first + 2);

            json array_json = json::parse(to_print);

            //* se afiseaza intr-un mod mai lizibil
            for (int i = 0; i < static_cast<int>(array_json.size()); ++i) {
              cout << "id: " << array_json[i]["id"] << "," << endl;
              cout << "title: " << array_json[i]["title"] << endl;
            }
          } else {
            cout << "There are no books!" << endl;
          }
        }

        //* se inchide conexiunea cu serverul
        close_connection(sockfd);
      }

    } else if (!strncmp(command, "get_book", 8)) {
      //* se verifica daca s-a primit token-ul JWT potrivit, in caz negativ
      //* se afiseaza un mesaj de eroare
      if (token_JWT.compare(flag_to_check) == 0) {
        cout << "Error! You do not have access to the library!" << endl;
      } else {
        int id;
        cout << "id=";
        cin >> id;

        // ! GET REQUEST

        //* se construieste stringul ce va fi trimis la server
        char string_to_put[50] = "/api/v1/tema/library/books/";
        char integer_string[32];

        sprintf(integer_string, "%d", id);
        strcat(string_to_put, integer_string);

        string tokens[1];
        tokens[0] = token_JWT;

        //* se trimite la server mesajul si token-ul JWT, necesar
        //* pentru accesul in biblioteca
        message = compute_get_request(
            (const char *)"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
            (const char *)(string_to_put), NULL, tokens, 1);

        //* se deschide conexiunea cu serverul
        sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

        //* se trimite mesajul catre server
        send_to_server(sockfd, message);

        //* se primeste raspunsul de la server
        response = receive_from_server(sockfd);

        //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
        //* un mesaj corespunzator
        char *ptr = strstr(response, "error");
        if (ptr != NULL) {
          cout << "No book was found!" << endl;
        } else {
          std::string str(response);

          //* daca nu exita setul de paranteze inseamna ca
          //* nu exista carti adaugate
          if (str.find("[{") != std::string::npos &&
              str.find("}]") != std::string::npos) {
            unsigned first = str.find("[{");
            unsigned last = str.find("}]");

            string to_print = str.substr(first, last - first + 2);

            json array_json = json::parse(to_print);

            //* se afiseaza informatiile cartii intr-un mod mai lizibil
            for (int i = 0; i < static_cast<int>(array_json.size()); ++i) {
              cout << "id: " << id << endl;
              cout << "title: " << array_json[i]["title"] << endl;
              cout << "author: " << array_json[i]["author"] << "," << endl;
              cout << "publisher: " << array_json[i]["publisher"] << ","
                   << endl;
              cout << "genre: " << array_json[i]["genre"] << "," << endl;
              cout << "page_count: " << array_json[i]["page_count"] << endl;
            }
          } else {
            cout << "There are no books!" << endl;
          }
        }

        //* se inchide conexiunea cu serverul
        close_connection(sockfd);
      }

    } else if (!strncmp(command, "add_book", 8)) {
      //* se verifica daca s-a primit token-ul JWT potrivit, in caz negativ
      //* se afiseaza un mesaj de eroare
      if (token_JWT.compare(flag_to_check) == 0) {
        cout << "Error! You do not have access to the library!" << endl;
      } else {
        string title;
        cout << "title=";
        std::getline(std::cin, title);

        string author;
        cout << "author=";
        std::getline(std::cin, author);

        string genre;
        cout << "genre=";
        std::getline(std::cin, genre);

        string publisher;
        cout << "publisher=";
        std::getline(std::cin, publisher);

        string page_count;
        cout << "page_count=";
        std::getline(std::cin, page_count);

        // ! POST REQUEST

        //* se construieste mesajul ce va fi trimis ca post_request
        json json_message = {
            {"title", title},         {"author", author},
            {"genre", genre},         {"page_count", page_count},
            {"publisher", publisher},
        };

        string tokens[1];
        tokens[0] = token_JWT;

        //* construire mesaj
        message = compute_post_request(
            "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
            "/api/v1/tema/library/books", "application/json",
            json_message.dump(), 1, tokens, 1);

        //* deschidere conexiune cu serverul
        sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

        //* se trimite atat mesajul, cat si token-ul JWT,
        //* necesar accesarii bibliotecii
        send_to_server(sockfd, message);

        //* se primeste mesajul de la server
        response = receive_from_server(sockfd);

        //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
        //* un mesaj corespunzator
        char *ptr = strstr(response, "error");
        if (ptr != NULL) {
          cout << "Error! Something bad happened! You probably entered the "
                  "wrong "
                  "input!"
               << endl;
        } else {
          cout << "Book successfully added!" << endl;
        }

        //* se inchide conexiunea cu serverul
        close_connection(sockfd);
      }

    } else if (!strncmp(command, "delete_book", 11)) {
      //* se verifica daca s-a primit token-ul JWT potrivit, in caz negativ
      //* se afiseaza un mesaj de eroare
      if (token_JWT.compare(flag_to_check) == 0) {
        cout << "Error! You do not have access to the library!" << endl;
      } else {
        int id;
        cout << "id=";
        cin >> id;

        // ! DELETE REQUEST

        //* se construieste mesajul ce va fi transmis ca delete_request
        char string_to_put[50] = "/api/v1/tema/library/books/";
        char integer_string[32];

        sprintf(integer_string, "%d", id);
        strcat(string_to_put, integer_string);

        string tokens[1];
        tokens[0] = token_JWT;

        //* se construieste mesajul cu ajutorul functiei
        //* 'compute_delete_request'
        message = compute_delete_request(
            "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
            (const char *)(string_to_put), NULL, tokens, 1);

        //* deschidere conexiune cu serverul
        sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

        //* se trimite atat mesajul, cat si token-ul JWT,
        //* necesar accesarii bibliotecii
        send_to_server(sockfd, message);

        //* se primeste mesajul de la server
        response = receive_from_server(sockfd);

        //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
        //* un mesaj corespunzator
        char *ptr = strstr(response, "error");
        if (ptr != NULL) {
          cout << "No book was deleted!" << endl;
        } else {
          cout << "The book has been successfully deleted!" << endl;
        }

        //* se inchide conexiunea cu serverul
        close_connection(sockfd);
      }

    } else if (!strncmp(command, "logout", 6)) {
      // ! GET REQUEST

      //* se verifica daca s-a primit cookie-ul potrivit, in caz negativ
      //* se afiseaza un mesaj de eroare
      if (autorization.compare(flag_to_check) == 0) {
        cout << "You are not logged in!" << endl;
      } else {
        string cookies[1];
        cookies[0] = autorization;

        //* se construieste mesajul in care va fi pus si cookie-ul primit
        message = compute_get_request(
            (const char *)"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
            (const char *)"/api/v1/tema/auth/logout", NULL, cookies, 1);

        //* se deschide conexiunea cu serverul
        sockfd = open_connection(ip, SERVER_PORT, PF_INET, SOCK_STREAM, 0);

        //* se trimit datele catre server
        send_to_server(sockfd, message);

        //* se primeste raspunsul de la server
        response = receive_from_server(sockfd);

        //* se verifica ce mesaj s-a primit, in caz de eroare se afiseaza
        //* un mesaj corespunzator
        char *ptr = strstr(response, "error");
        if (ptr != NULL) {
          cout << "Error! Unsucessful logout!" << endl;
        } else {
          cout << "Logout successful" << endl;
        }

        //* se sterge cookie-ul si token-ul JWT
        autorization = '*';
        token_JWT = '*';

        //* se inchide conexiunea cu serverul
        close_connection(sockfd);
      }
    } else if (!strncmp(command, "exit", 4)) {
      //* se opreste clientul
      break;
    }
  }
}