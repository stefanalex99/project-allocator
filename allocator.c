// Copyright 2017 Hermeniuc Stefan-Alexandru

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CMD_SIZE 20
#define SHOW_SIZE 15

// Definim o functie care initializeaza arena de tip unsigned char
unsigned char* initialize(int N) {
  unsigned char *arena = (unsigned char *) calloc(N , sizeof(unsigned char));
  /*
   * Verificam daca avem suficient spatiu pentru a tine o arena de dimensiunea
   * dorita
   */
  if (arena == NULL) {
    printf("ERROR\n");
    exit(1);  // Iesim din program daca nu avem
  }

  return arena;
}


int alloc(unsigned char *arn, int s, int N) {
  int i;
  /*
   * Ne ocupam in mod special de o arena goala, adaugand un bloc daca arena
   * este suficient de mare
   */
  if (*((int *)arn) == 0) {
    if (N - sizeof(int) >= s + 3 * sizeof(int)) {
      *((int *)arn) = sizeof(int);
      *((int *)arn + 1) = 0;
      *((int *)arn + 2) = 0;
      *((int *)arn + 3) = s + 3 * sizeof(int);
      return 4 * sizeof(int);
    }
  } else {
    i = *((int *)arn);
    /*
     * Verificam daca putem aloca <size> octeti intre primul bloc si zona de
     * gestiune 0
     */
    if (i - sizeof(int) >= s + 3 * sizeof(int)) {
      /*
       * Introducem blocul in zona imediat urmatoare indicelui de start
       * (modificandu-l), anume pe pozitia 4
       */
      *((int *)arn) = sizeof(int);
      // Indicele blocului precedent al urmatorului bloc este modificat
      *((int *)(arn + i) + 1) = sizeof(int);
      // Initializam zona de gestiune a blocului nou alocat
      *((int *)arn + 1) = i;
      *((int *)arn + 2) = 0;
      *((int *)arn + 3) = s + 3 * sizeof(int);
      // Returnam inceputul zonei accesibile utilizatorului
      return 4 * sizeof(int);
    }

    // Incercam sa cautam spatiu intre oricare 2 blocuri deja alocate
    while (*((int *)(arn + i)) != 0) {
          if (*((int *)(arn + i)) - *((int *)(arn + i) + 2) - i >= s + 3
          * sizeof(int)) {
            // Initializam zona de gestiune a blocului nou alocat
            *((int *)(arn + i + *((int *)(arn + i) + 2))) = *((int *)(arn + i));
            *((int *)(arn + i + *((int *)(arn + i) + 2)) + 1) = i;
            *((int *)(arn + i + *((int *)(arn + i) + 2)) + 2) = s + 3
            * sizeof(int);
            // Modificam zonele de gestiune ale blocurilor vecine
            *((int *)(arn + *((int *)(arn + i))) + 1) = i + *((int *)(arn + i)
            + 2);
            *((int *)(arn + i)) = i + *((int *)(arn + i) + 2);
            // Returnam indicele
            return i + *((int *)(arn + i) + 2) + 3 * sizeof(int);
          }
        // Daca nu am gasit, cautam intre urmatoarele 2 blocuri
        i = *((int *)(arn + i));
      }

      // Verificam daca putem aloca spatiu intre ultimul bloc si spatiul ramas
      if (*((int *)(arn + i)) == 0) {
        if (N - *((int *)(arn + i) + 2) - i >= s + 3 * sizeof(int)) {
          // Initializam zona de gestiune a blocului nou alocat
          *((int *)(arn + i + *((int *)(arn + i) + 2))) = 0;
          *((int *)(arn + i + *((int *)(arn + i) + 2)) + 1) = i;
          *((int *)(arn + i + *((int *)(arn + i) + 2)) + 2) = s + 3
          * sizeof(int);
          // Modificam zona de gestiune a singurului vecin
          *((int *)(arn + i)) = i + *((int *)(arn + i) + 2);
          // Returnam indicele
          return i + *((int *)(arn + i) + 2) + 3 * sizeof(int);
        }
      }
    }

  // Daca nu am reusit sa gasim un spatiu suficient de mare returnam 0
  return 0;
}

void fill(unsigned char *arn, int idx, int s, int val, int N) {
  int i;

  for (i = idx; i < idx + s; i ++) {
    *(arn + i) = val;
  }
}

void dump(unsigned char *arn, int N) {
  int i;

  for (i = 0; i < N; i ++) {
    // Printam indicele curent cu tab dupa
    if (i % 16 == 0 && i != 0) {
      printf("\n%08X\t", i);
    }
    if (i == 0) {
      printf("%08X\t", i);
    }

    // Afisam continutul arenei
    printf("%02X ", arn[i]);

    // Lasam spatiu suplimentar intre elementele 8 si 9 de pe fiecare linie
    if (i % 8 == 7) {
      printf(" ");
    }
  }

  // Afisam dimensiunea arenei
  printf("\n%08X\n", N);
}

void myfree(unsigned char *arn, int N, int idx) {
  int i = idx - 3 * sizeof(int);

  // Ne ocupam de eliberarea primului bloc din arena
  if (*((int *)(arn + i) + 1) == 0 && *((int *)(arn + i)) != 0) {
    *((int *)arn) = *((int *)(arn + i));
    *((int *)(arn + *((int *)(arn + i))) + 1) = 0;
    return ;
  }

  // Ne ocupam de eliberarea unicului bloc din arena
  if (*((int *)(arn + i) + 1) == 0 && *((int *)(arn + i)) == 0) {
    *((int *)arn) = 0;
    return ;
  }

  // Ne ocupam de eliberarea ultimului bloc din arena
  if (*((int *)(arn + i) + 1) != 0 && *((int *)(arn + i)) == 0) {
    *((int *)(arn + *((int *)(arn + i) + 1))) = 0;
    return ;
  }

  // Ne ocupam de eliberarea unui bloc nemarginas
  if (*((int *)(arn + i) + 1) != 0 && *((int *)(arn + i)) != 0) {
    *((int *)(arn + *((int *)(arn + i) + 1))) = *((int *)(arn + i));
    *((int *)(arn + *((int *)(arn + i))) + 1) = *((int *)(arn + i) + 1);
    return ;
  }
}

// Definim o functie care numara cate blocuri libere avem in arena
int count_free(unsigned char *arn, int N) {
  int freen = 0;
  int i;

  if (*(int *)arn == 0) {
    freen = 1;  // In cazul unei arene goale avem o singura zona libera
  } else {
    /*
     * Cautam zone goale intre 2 blocuri sau intre sfarsitul arenei si ultimul
     * bloc
     */
    i = *((int *)arn);

    if (i - sizeof(int) > 0) {
      freen++;
    }

    while (*((int *)(arn + i)) != 0) {
        if (*((int *)(arn + i)) - *((int *)(arn + i) + 2) - i > 0) {
        freen++;
      }

      i = *((int *)(arn + i));
    }

    if (*((int *)(arn + i)) == 0) {
      if (N - *((int *)(arn + i) + 2) - i > 0) {
        freen++;
      }
    }
  }

  return freen;
}

void show_free(unsigned char *arn, int N) {
  int freeb = 0;
  int i;

  if (*(int *)arn == 0) {
    /*
     * Daca arena e goala spatiul liber este egal cu dimensiunea totala minus
     * dimensiunea primului bloc de gestiune
     */
    freeb = N - sizeof(int);
  } else {
    /*
     * Insumam orice spatiu nealocat aflat intre doua blocuri sau sfarsitul
     * arenei si ultimul bloc
     */
    if (*((int *)arn) - sizeof(int) > 0) {
      freeb += *((int *)arn) - sizeof(int);
    }

    i = *((int *)arn);

    while (*((int *)(arn + i)) != 0) {
        if (*((int *)(arn + i)) - *((int *)(arn + i) + 2) - i > 0) {
        freeb += *((int *)(arn + i)) - *((int *)(arn + i) + 2) - i;
      }

      i = *((int *)(arn + i));
    }

    if (*((int *)(arn + i)) == 0) {
      if (N - *((int *)(arn + i) + 2) - i > 0) {
        freeb += N - *((int *)(arn + i) + 2) - i;
      }
    }
  }

  printf("%d blocks (%d bytes) free\n", count_free(arn, N), freeb);
}

void show_usage(unsigned char *arn, int N) {
  int blocks = 0;
  int usdb = 0;
  int rsrb = sizeof(int);
  int i = *((int *)arn);
  int fragm;

  while (i != 0) {
    blocks++;
    // Calculam numarul de octeti folositi
    usdb += *((int *)(arn + i) + 2) - 3 * sizeof(int);
    // Calculam numarul de octeti rezervati
    rsrb += *((int *)(arn + i) + 2);
    // Mergem la urmatorul bloc
    i = *((int *)(arn + i));
  }

  if (blocks == 0) {
    fragm = 0;
  } else {
    fragm = (int)(((count_free(arn, N) - 1) * 1.0 / blocks) * 100);
  }

  printf("%d blocks (%d bytes) used\n", blocks , usdb);
  printf("%d%% efficiency\n", (int)(((usdb * 1.0) / rsrb) * 100));
  printf("%d%% fragmentation\n", fragm);
}

void show_allocations(unsigned char *arn, int N) {
  int i;

  // Primii 4 octeti sunt mereu ocupati
  printf ("OCCUPIED %d bytes\n", sizeof(int));

  if (*(int *)arn == 0) {
    // O arena goala are primii 4 octeti ocupati si restul liberi
    printf ("FREE %d bytes\n", N - sizeof(int));
  } else {
    i = *((int *)arn);

    if (i - sizeof(int) > 0) {
      printf("FREE %d bytes\n", i - sizeof(int));
    }

    do {
      // Afisam nr de octeti ocupati pt fiecare bloc
      printf("OCCUPIED %d bytes\n", *((int *)(arn + i) + 2));
      /*
       * Daca intre 2 blocuri sau intre sfarsitul arenei si ultimul bloc
       * gasim spatiu liber afisam FREE
       */
      if (*((int *)(arn + i)) == 0) {
        if (N - *((int *)(arn + i) + 2) - i > 0) {
          printf("FREE %d bytes\n", N - *((int *)(arn + i) + 2) - i);
        }
      } else {
        if (*((int *)(arn + i)) - *((int *)(arn + i) + 2) - i > 0) {
          printf("FREE %d bytes\n", *((int *)(arn + i)) - *((int *)(arn + i)
          + 2) - i);
        }
      }
      i = *((int *)(arn + i));
    } while (i != 0);
  }
}

void cmd_execute() {
  char cmd[CMD_SIZE];
  int N;
  unsigned char *arn;

  while (1) {
    scanf("%s", cmd);

    if (!strcmp(cmd, "INITIALIZE")) {
      scanf("%d", &N);
      arn = initialize(N);
    } else if (!strcmp(cmd, "FINALIZE")) {
      free(arn);
      exit(0);
    } else if (!strcmp(cmd, "DUMP")) {
      dump(arn, N);
    } else if (!strcmp(cmd, "ALLOC")) {
      int size;
      scanf("%d", &size);
      printf("%d\n", alloc(arn, size, N));
    } else if (!strcmp(cmd, "FREE")) {
      int idx;
      scanf("%d", &idx);
      myfree(arn, N, idx);
    } else if (!strcmp(cmd, "FILL")) {
      int idx, size, val;
      scanf("%d %d %d", &idx, &size, &val);
      fill(arn, idx, size, val, N);
    } else if (!strcmp(cmd, "SHOW")) {
      char show[SHOW_SIZE];
      scanf("%s", show);
      if (!strcmp(show, "FREE")) {
        show_free(arn, N);
      } else if (!strcmp(show, "USAGE")) {
        show_usage(arn, N);
      } else if (!strcmp(show, "ALLOCATIONS")) {
        show_allocations(arn, N);
      }
    }
  }
}

int main() {
    cmd_execute();

    return 0;
}
