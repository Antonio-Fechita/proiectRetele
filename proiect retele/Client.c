#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

void conversieTimp(int secunde,char timp[])
{
  int ore = secunde/3600;
  int minute = (secunde-ore*3600)/60;
  secunde = (secunde-ore*3600-minute*60);

  if(ore!=0)
  sprintf(timp,"%dh %dm %ds",ore,minute,secunde);
  else if(minute!=0)
  sprintf(timp,"%dm %ds",minute,secunde);
  else if(secunde!=0)
  sprintf(timp,"%ds",secunde);

}



void citesteDeLaTastatura(char buff[])
{
  char x;
  int k=0;
  while(x = getchar())
  {   
    if(x == '\n')
       {
         buff[k]=NULL;
         break;
       }
    else
        buff[k++]=x;
  }
}

void prefizeazaLungime(char x[])
{
	char lungime[20005];
	sprintf(lungime,"%d",strlen(x));
	strcat(lungime,"/");
	strcat(lungime,x);
	strcpy(x,lungime);
}



int scriePrefixat(int fd,char buff[])
{
	prefizeazaLungime(buff);
	//printf("scriePrefixat-> ");
	return write(fd,buff,strlen(buff));
}

int citestePrefixat(int fd,char buff[])
{
	char x;
	int k=0;
	int lungime = 0;
  int r;
	do
	{
		r=read(fd,&x,1);
    if(r==-1)
    perror("Eroare la read in citestePrefixat() ");
		lungime = lungime*10 + (x - 48);
	}while(x!='/');
	
	r = read(fd,buff,lungime);
  if(r==-1)
    perror("Eroare la read in citestePrefixat() ");
	buff[r]=NULL;
	return r;
}

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  char username[50];
  int usernameOK;

  //
  do
  {
    printf("Va rugam introduceti un nume de utilizator!\n>");
    usernameOK=1;
    citesteDeLaTastatura(username);
    if(strlen(username)<3)
    {
    printf("Numele de utilizator trebuie sa aiba mai mult de 3 caractere!\n");
    usernameOK=0;
    }
    else if(strlen(username)>30)
    {
    printf("Limita numelui de utlizator este de 30 de caractere!\n");
    usernameOK=0;
    }
    for(int i=0;i<strlen(username);i++)
    if(!isalpha(username[i]))
    {
      printf("Numele de utilizator trebuie sa contina doar litere!\n");
      usernameOK=0;
      break;
    }

  } while (!usernameOK);


  printf("INFORMATII UTILE\n\n-------------------------------\n Olimpiada consta din o problema cu un numar prestabilit de instante de testat.\nPunctajul final se calculeaza in felul urmator:\n Pentru orice solutie care nu compileaza se acorda 0 puncte.\n Pentru o solutie care compileaza se acorda puncte pentru fiecare instanta testata atata timp cat se incadreaza intr-o limita de timp iar rezultatul este corect.\nPentru o solutie corecta care se incadreaza in timp se pot acorda puncte suplimentare pentru o viteza de executie mai buna.\n\nSolutia problemei se va scrie in fisierul \"solutie.c\". Alterarea fisierului prin redenumire sau stergere va avea ca rezultat un punctaj nul.\n Este important ca utilizatorul sa salveze fisierul inainte de terminarea timpului pus la dispozitie rezolvarii pentru a-si inregistra cu succes solutia!\nDaca exista in prezent o olimpiada in desfasurare utilizatorul va fi pus in asteptare pentru urmatoarea sesiune\n-------------------------------\n\n");
  printf("Asteptam eliberearea serverului...\n");

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if ((connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr))) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }


  int fd;
  char subsir[1005];
  char codSursa[20005];
  codSursa[0]=NULL;
  int nrCaractere = 0,r;
  int fisierAlterat=0;
  int msg[1000];
  int timpAsteptare;


  
  /*se primeste enuntul problemei sau un mesaj care ne anunta ca olimpiada este deja in desfasurare*/
  char problema[500];
  while(1)
  {
  citestePrefixat(sd,problema);
  if(strncmp(problema,"Utilizatori asteptati pentru inceperea concursului: ",52)==0)
  printf("%s",problema);
  else break;
  }
  
  scriePrefixat(sd,username);
  char timpRamas[15];
  printf("Toti utilizatorii s-au conectat! Olimpiada incepe in\n");
  for(int i=15;i>=1;i--)
  {
    conversieTimp(i,timpRamas);
    printf("%s\n",timpRamas);
    sleep(1);
  }

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s\n",problema);
  printf ("Scrieti solutia problemei in fisierul \"solutie.c\" \n");

  //facem un proces copil care sa deschida automat editorul userului
  int pid=fork();

  fd = open("solutie.c",O_WRONLY|O_TRUNC|O_CREAT,0777);
  if(fd == -1)
  {
    printf("Eroare la open\n");
    exit(3);
  }
  close(fd);

  if(pid==0)
  {
    execlp("gedit","gedit","solutie.c",NULL); //deschide editorul clientului
  }

  char secundeChar[10];
  int secunde;
  citestePrefixat(sd,secundeChar);
  secunde=atoi(secundeChar);

  for(int i=1;i<=secunde;i++)
  {
    conversieTimp(secunde-i+1,timpRamas);
    printf("Timp ramas: %s\n",timpRamas);
    sleep(1);
  }


  if((access("solutie.c",F_OK))==-1)
  {
    strcpy(codSursa,".");
    fisierAlterat=1;
  }
  else
  {
    fd = open("solutie.c",O_RDWR);
    if(fd == -1)
    {
      printf("Eroare la open\n");
      exit(3);
    }


    do{
      r=read(fd,subsir,1000);
      if(r==-1)
      {
        printf("Eroare la read()");
        exit(2);
      }
      nrCaractere=nrCaractere+r;
      subsir[r]=NULL;
      strcat(codSursa,subsir);

    }while(r>0);



  printf("Aceasta este varianta finala trimisa catre server: \n%s\n",codSursa);
  }
  

  if(strlen(codSursa)==0) //da eroare daca nu are niciun caracter, asa ca asiguram existenta cel putin a unui caracter, rezultatul fiind acelasi de cod care nu compileaza
  strcpy(codSursa,".");
/* trimiterea mesajului la server */
   if (scriePrefixat(sd, codSursa) <= 0)
     {
       perror ("[client]Eroare la write() spre server.\n");
       return errno;
     }

     if(fisierAlterat==0)
     printf("Solutia dumneavoastra a fost inregistrata! Asteptam calcularea punctajelor...\n");
     else
     printf("Fisierul solutie.c a fost alterat! :(\n");
   /* citirea raspunsului dat de server 
      (apel blocant pina cind serverul raspunde) */
   if (citestePrefixat (sd, msg) < 0)
     {
       perror ("[client]Eroare la read() de la server.\n");
       return errno;
     }
   /* afisam mesajul primit */
   printf ("\n%s\n", msg);

  // /* inchidem conexiunea, am terminat */
  
  close (sd);
  
}