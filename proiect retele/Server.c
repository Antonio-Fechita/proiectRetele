/* servTcpConc.c - Exemplu de server TCP concurent
    Asteapta un nume de la clienti; intoarce clientului sirul
    "Hello nume".
    */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

/* portul folosit */
#define PORT 2024 //modificat fata de exemplul dat
// #define PORT 2728
/* codul de eroare returnat de anumite apeluri */
extern int errno;

void prefizeazaLungime(char x[])
{
	char lungime[500];
	sprintf(lungime,"%d",strlen(x));
	strcat(lungime,"/");
	strcat(lungime,x);
	strcpy(x,lungime);
}

int scriePrefixat(int fd,char buff[])
{
	prefizeazaLungime(buff);
	return write(fd,buff,strlen(buff));
}

int citestePrefixat(int fd,char buff[])
{
	char x;
	int k=0;
	int lungime = 0;
	do
	{
		read(fd,&x,1);
		lungime = lungime*10 + (x - 48);
	}while(x!='/');
	
	int r = read(fd,buff,lungime);
	buff[r]=NULL;
	//printf("PREF: %s\n",buff);
	return r;
}

int alegeRandomProblema(char problema[500],int nrProbleme,char listaProbleme[10005]) //pune in bufferul "problema" textul unei probleme alese la intamplare
{
  srand(time(NULL));
  int indexProblemaSelectata = rand()%nrProbleme + 1; //alegem la intamplare numarul unei probleme din lista problemelor
  int n=strlen(listaProbleme);
  int k=0;
  int problemaCurenta=0; //tine evidenta problemei pe care o citim la un moment dat
  for(int i=0;i<n;i++)
  {
      if(listaProbleme[i]=='#') //problemele din fisier sunt separate prin simbolul '#'
      problemaCurenta++;
      if(problemaCurenta==indexProblemaSelectata && listaProbleme[i]!='#') //daca citim problema aleasa din lista o salvam in bufferul problema
      {
          problema[k]=listaProbleme[i];
          k++;
      }
      else if(problemaCurenta>indexProblemaSelectata) //daca am terminat de citit problema aleasa ne putem opri din citit lista de probleme
	  {
		  problema[k]=NULL;
		  break;
	  }
      
  }
  return indexProblemaSelectata;  //returnam un identificator al problemei selectate
}

int obtineListaProbleme(char listaProbleme[10005])
{
    int fd = open("Probleme",O_RDONLY);
    int r;
    char subsir[1005];
    do{
    r=read(fd,subsir,1000);
    if(r==-1)
    {
      printf("Eroare la read()");
      exit(2);
    }
    subsir[r]=NULL;
    strcat(listaProbleme,subsir);

  }while(r>0);

  int n=strlen(listaProbleme);
  int nrProbleme=0;

  for(int i=0;i<n;i++)
  if(listaProbleme[i]=='#')
  nrProbleme++;
  
  nrProbleme--;
  return nrProbleme;
}

void stergePunctajeVechi(int nrPunctaje)
{
	char numeFisier[30];
	for(int i=1;i<=nrPunctaje;i++)
	{
		sprintf(numeFisier,"Punctaj%d",i);
		if((access(numeFisier,F_OK))!=-1)
		{
			remove(numeFisier);
		}
			 	
	}
}


int citesteFisier(int fd, char buff[])
{

	
	int nrCaractere=0;
	buff[0]=NULL;
	char subsir[1005];
		int r;
		int k=0;
		do{
			k++;
    		r=read(fd,subsir,1000);
    		if(r==-1)
    		{
     		 printf("Eroare la read()");
    		  exit(2);
   			}
			nrCaractere=nrCaractere+r;
   			subsir[r]=NULL;
    		strcat(buff,subsir);

  		}while(r>0);

	return nrCaractere;
}

int config(int* anonim, int* timpAsteptare, int nrInstante[], int timpRezolvare[], int timpSolutie[][4]){
  char str[10000];
  char ant[20];
  int fd = open("Configurare",O_RDONLY);
  int problema=0;
  int nrUseri;
  citesteFisier(fd,str);
  char *token;
  token = strtok (str,": \n");
  strcpy(ant,token);
  while (token != NULL)
  {
    if(strcmp(token,"alocat")==0)
    problema=problema+1;
    else if(strcmp(ant,"utilizatori")==0)
    nrUseri=atoi(token);
    else if(strcmp(ant,"rezolvarii")==0)
    timpRezolvare[problema]=atoi(token);
    else if(strcmp(ant,"verificat")==0)
    nrInstante[problema]=atoi(token);
	else if(strcmp(ant,"utilizatorilor")==0)
	*timpAsteptare=atoi(token);
	else if(strcmp(ant,"Anonim")==0)
	{
		if(strcmp(token,"nu")==0)
		{
		*anonim=0;
		}
		else
		{
		*anonim=1;
		}
	}
    else if(strcmp(ant,"rulare")==0)
    {
      timpSolutie[problema][0]=atoi(token);
      token = strtok (NULL, ": \n");
      timpSolutie[problema][1]=atoi(token);
      token = strtok (NULL, ": \n");
      timpSolutie[problema][2]=atoi(token);
	  token = strtok (NULL, ": \n");
      timpSolutie[problema][3]=atoi(token);
    }

    strcpy(ant,token);
    token = strtok (NULL, ": \n");
  }

  return nrUseri;
}

void desparteString(char a[], char b[]) //desparte stringul a la aparitia simbolului / si pune jumatatea din dreapta in stringul b pastrand in a jumatatea din stanga
{
	int n=strlen(a);
	int ok=0;
	int k=0;
	for(int i=0;i<n;i++)
	{
		if(a[i]=='/')
		{
			a[i]=NULL;
			ok=1;
		}
		else if(ok==1)
		{
			b[k++]=a[i];
		}

	}
	b[k]=NULL;
}

float calculeazaPunctaj(int userID,int idProblema,int nrInstanteDeTestat,char numeExecutabil[],time_t limitaSuperioara,time_t t1, time_t t2, time_t t3)
{
	float punctaj=0;
	char caleInput[100];
	char caleOutput[100];
	char numeFisierOutputUser[50];
	char outputCorect[10000];
	char outputUser[10000];
	//
	printf("t1= %ld, t2= %ld, t3= %ld, limita= %ld, userID= %d, idProblema= %d, nrInstante= %d\n",t1,t2,t3,limitaSuperioara,userID,idProblema,nrInstanteDeTestat);
	char temp[100];
	sprintf(temp,"pct%d",userID);
	int incorecte=0;
	int depasite=0;
	int rapide=0;
	int medii=0;
	int acceptabile=0;
	//
	sprintf(numeFisierOutputUser,"Output%d",userID);
	for(int i=1;i<=nrInstanteDeTestat;i++)
	{
		sprintf(caleInput,"Input-Output/%d/%d/",idProblema,i);
		strcpy(caleOutput,caleInput);
		strcat(caleInput,"Input");
		strcat(caleOutput,"Output");

		close(0);
		close(1);

		int fd_i = open(caleInput,O_RDONLY);
		int fd_o_user = open(numeFisierOutputUser,O_RDWR|O_TRUNC|O_CREAT,0777);
		int fd_o = open(caleOutput,O_RDONLY);

		citesteFisier(fd_o,outputCorect);
		outputCorect[strlen(outputCorect)-1]=NULL; //stergem ultimul caracter care este un new line
		//ruleaza program

		char comandaRulareExecutabil[50];
		strcpy(comandaRulareExecutabil,"./");
		strcat(comandaRulareExecutabil,numeExecutabil);
		time_t start = time(NULL);
		time_t timpRulareProgram;
		int timpDepasit=0;
		int pid3=fork(); //copil care va rula solutia compilata cu succes a utilizatorului
		if(pid3==0)
    	{
      		execlp(comandaRulareExecutabil,NULL); //ceva de felul ./test
    	}				
    	do
    	{
      		timpRulareProgram = time(NULL) - start;
      		if(timpRulareProgram > limitaSuperioara) //daca s-a depasit limita oprim programul deoarece ar putea sa nu se mai termine
      		{
       			timpDepasit=1;
				depasite=depasite+1;
        		kill(pid3,SIGKILL);
        		break;
      		}
   		}while(waitpid(pid3,NULL,WNOHANG)==0);

					  
		//sleep(10);
    	if(timpDepasit==0) //daca programul utilizatorului s-a incadrat in timp
   		{
      	//Nu s-a depasit limita de timp
		  //printf("timpdepasit=0");

		//   fd_o_user=open(numeFisierOutputUser,O_RDONLY);
			int inainteDeLseek=lseek(fd_o_user,0,SEEK_CUR);
			int dupaLseek=lseek(fd_o_user,0,SEEK_SET);
			int caractere=citesteFisier(fd_o_user,outputUser);
					  close(fd_o_user);
		  int fde=open(temp,O_WRONLY|O_APPEND); //probabil 1
			 printf("output user: \n[[%s]]\ninainte de lseek: %d, dupa lseek: %d,\n output corect: \n[[%s]]\n",outputUser,inainteDeLseek,dupaLseek,outputCorect);
			 
      		if(strcmp(outputUser,outputCorect)==0)  //daca outputul userului este corect ii vom acorda un numar de puncte
			{
				printf("Timp rulare program: %d\n",timpRulareProgram);
				punctaj=punctaj+5;
				if(timpRulareProgram <= t1)
				{
				punctaj=punctaj+5;
				rapide=rapide+1;
				}
				else if(timpRulareProgram <= t2)
				{
				punctaj=punctaj+2.5;
				medii=medii+1;
				}
				else
				{
				acceptabile=acceptabile+1;
				}
			}
			else //output incorect
			{
				incorecte=incorecte+1;
			}
    	} /*if(timpDepasit==0)*/
		//altfel nu primeste puncte pentru aceasta instanta testata
		// else
		close(fd_o_user);
	}
	punctaj=(punctaj*100)/(10 * nrInstanteDeTestat); //normalizam punctajele in functie de cate instante de testat avem
	printf("punctaj: %f\nrapide: %d, medii: %d,acceptabile: %d, incorecte: %d, depasite: %d\n\n-----------\n\n",punctaj,rapide,medii,acceptabile,incorecte,depasite);

	return punctaj;
}

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

void anuntaUserStareParticipanti(int fd[], int nrUseriActivi, int nrUseriAsteptati, int timpAsteptare)
{
	
	char rasp[100];
	sprintf(rasp,"Utilizatori asteptati pentru inceperea concursului: %d/%d\n",nrUseriActivi,nrUseriAsteptati);
	for(int i=1;i<=nrUseriActivi;i++)
	{
		int pid=fork();
		if(pid==0)
		{
		scriePrefixat(fd[i],rasp);
		exit(1);
		}
	}
}



int main ()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    char msg[20005];		//mesajul primit de la client
    char msgrasp[20005]=" ";        //mesaj de raspuns pentru client
	char problema[500];
	char listaProbleme[10005];
    int sd;			//descriptorul de socket
	int nrProbleme;
  	int nrInstante[50];
  	int timpRezolvare[50];
  	int timpSolutie[50][4];
	int timpAsteptare;
	int anonim=0;
	int nrClienti=config(&anonim,&timpAsteptare,nrInstante,timpRezolvare,timpSolutie);
	int idProblema;
	
	
	nrProbleme=obtineListaProbleme(listaProbleme);
    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	perror ("[server]Eroare la socket().\n");
    	return errno;
    }

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
    	perror ("[server]Eroare la bind().\n");
    	return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 20) == -1)
    {
    	perror ("[server]Eroare la listen().\n");
    	return errno;
    }

	int idClient=0;
    /* servim in mod concurent clientii... */
    while (1)
    {
    	int client[30];
    	int length = sizeof (from);

    	printf ("[server]Asteptam la portul %d...\n",PORT);
    	fflush (stdout);

		
    	/* acceptam un client (stare blocanta pina la realizarea conexiunii) */
    	client[idClient+1] = accept (sd, (struct sockaddr *) &from, &length);

		if(idClient==nrClienti) //un nou client solicita sa se conecteze, trebuie sa astepte urmatoarea sesiune daca a inceput deja una
		{						
			while(waitpid(-1,NULL,WNOHANG)!=-1); //asteapta pana cand o sesiune este libera
			idClient=0;
		}						
		idClient++;
		anuntaUserStareParticipanti(client,idClient,nrClienti,timpAsteptare);

    	/* eroare la acceptarea conexiunii de la un client */
    	if (client[idClient] < 0)
    	{
    		perror ("[server]Eroare la accept().\n");
    		continue;
    	}

		if(idClient==nrClienti) //daca s-au adunat toti clientii
		{
		idProblema=alegeRandomProblema(problema,nrProbleme,listaProbleme);
		stergePunctajeVechi(nrClienti);
		for(int i=1;i<=nrClienti;i++){ //lansam cate un proces copil pentru fiecare client
    	int pid;
    	if ((pid = fork()) == -1) {
    		close(client[i]);
    		continue;
    	} else if (pid > 0) {
    		// parinte
    		close(client[i]);
    		while(waitpid(-1,NULL,WNOHANG));
    		continue;
    	} else if (pid == 0) {
    		// copil
    		close(sd);
			
			/* se trimite problema de programare utilizatorului */
			scriePrefixat(client[i],problema);

			/*citim numele de utilizator*/
			 char userName[50];
			 citestePrefixat(client[i],userName);

			char timpRezolvareChar[10];
			sprintf(timpRezolvareChar,"%d",timpRezolvare[idProblema]);
			printf("TIMP REZOLVARE: %s\n",timpRezolvareChar);
			scriePrefixat(client[i],timpRezolvareChar);



    		/* s-a realizat conexiunea, se astepta mesajul */
    		bzero (msg, 20005);
    		printf ("[server]Asteptam mesajul...\n");
    		fflush (stdout);

    		/* citirea mesajului */
    		if (citestePrefixat (client[i], msg) < 0)
    		{
    			perror ("[server]Eroare la read() de la client.\n");
    			close (client[i]);	/* inchidem conexiunea cu clientul */
    			continue;		/* continuam sa ascultam */
    		}

    		printf ("[server]Mesajul a fost receptionat...%s\n", msg);

    		/*pregatim mesajul de raspuns */
    		bzero(msgrasp,20005);
    		sprintf(msgrasp,"Acesta este id-ul tau: %d\n",i);
    		//strcat(msgrasp,msg);
			//---

			//pregatim fisier pentru codul sursa al utilizatorului		
			char numeFisierCodSursa[50];
			sprintf(numeFisierCodSursa,"userInput%d.c",i);
			int fd = open(numeFisierCodSursa, O_RDWR | O_TRUNC | O_CREAT, 0777); 
			write(fd,msg,strlen(msg)); //scriem in noul fisier codul sursa primit de la client
			char numeExecutabil[50];
			sprintf(numeExecutabil,"userInput%d",i); //

			//compilam codul intr-un alt proces copil
			int pid1;
			if ((pid1 = fork()) == -1) {
				perror("Eroare la fork()");
    			
			}
			 else if (pid1 == 0) {
    		// copil
			close(client[i]);

			execlp("gcc","gcc",numeFisierCodSursa,"-o",numeExecutabil,NULL);
			exit(0);
			}
    			// parinte 
				wait(NULL); //asteptam finalizarea compilarii efectuata de copilul procesului curent

				//codul a fost compilat daca a fost sintactic corect, urmeaza sa fie rulat in urmatorul proces copil

				int pid2; //////////////////////////////
				if ((pid2 = fork()) == -1) 
				{
    				perror("Eroare la al doilea fork()\n");
    				exit(-1);
    			}  
				else if (pid2 == 0) 
				{
    		     // copil
				 //verifica daca exista fisierul de executat

				float punctaj;
				char numeFisierPunctaj[30];
				char numeFisierPunctajNou[30];
				sprintf(numeFisierPunctaj,"Punctaj%dX",i); //vom redenumi mai tarziu fisierul din Punctaj[id]X in Punctaj[id] pentru a asigura faptul ca atunci cand extragem punctajul unui alt utilizator procesul care il scrie a terminat sa il scrie in fisier in totalitate
				sprintf(numeFisierPunctajNou,"Punctaj%d",i);



				 	int a;
				 	if((a=access(numeExecutabil,F_OK))==-1)
				 	{
						printf("Codul sursa nu a fost compilat cu succes!\n");
						punctaj=0;
						remove(numeFisierCodSursa);
				 	}
				 	else
					{
						 printf("Codul sursa a fost compilat cu succes!\n");
						 //urmeaza pasul de rulare
						 
						punctaj = calculeazaPunctaj(i,idProblema,nrInstante[idProblema],numeExecutabil,timpSolutie[idProblema][3],timpSolutie[idProblema][0],timpSolutie[idProblema][1],timpSolutie[idProblema][2]);
					}

						int fd_p=open(numeFisierPunctaj,O_WRONLY|O_CREAT,0777);
						char punctajChar[50];
						sprintf(punctajChar,"%f/%s",punctaj,userName);
						write(fd_p,punctajChar,strlen(punctajChar));


						close(fd_p);

						// printf("3: numeFisierPunctaj: [%s], numeFisierPunctajNou: [%s]\n",numeFisierPunctaj,numeFisierPunctajNou);

						// close(1);
						// int fd_d=open("pct2",O_WRONLY|O_APPEND);
						// printf("4: numeFisierPunctaj: [%s], numeFisierPunctajNou: [%s]\n",numeFisierPunctaj,numeFisierPunctajNou);
						if(rename(numeFisierPunctaj,numeFisierPunctajNou)==0)
						printf("Redenumirea a avut succes\n");
						else
						printf("Redenumirea a esuat : \n");
						// printf("5: numeFisierPunctaj: [%s], numeFisierPunctajNou: [%s]\n",numeFisierPunctaj,numeFisierPunctajNou);
						printf("Dupa rename\n");

						 //ruleazaProgramUser(numeExecutabil,timpFolositDeProgram,input,output);

						 remove(numeFisierCodSursa);
						 remove(numeExecutabil); //asiguram faptul ca un utilizator dintr-o sesiune viitoare avand acelasi id ca un alt utilizator dintr-o sesiune anterioara nu va fi punctat pentru solutia dintr-o sesiune anterioara
					 

					 
					exit(2);
			    }
    			// parinte (copilul primului parinte)
    			wait(NULL); //asteptam terminarea pasului de rulare a executabilului

			//extragem toate punctajele userilor din fisierele scrise de fiecare proces dedicat unui user


			float punctaje[100];
			char userNames[100][50];
			char numeFisierPunctaj[30];
			char punctajChar[10];
			float punctajPersonal;
			int fd_p;
			float aux;
			char auxC[50];

			for(int j=1;j<=nrClienti;j++)
			{
				sprintf(numeFisierPunctaj,"Punctaj%d",j);
				printf("cautam in fisierul: %s\n",numeFisierPunctaj);
				while(access(numeFisierPunctaj,F_OK)==-1);  //asteptam pana cand devine disponibil punctajul unui anumit user
				fd_p = open(numeFisierPunctaj,O_RDONLY);
				// int r=read(fd_p,punctajChar,10); //punctajul maxim ar trebui sa fie 100.000000
				citesteFisier(fd_p,punctajChar);
				desparteString(punctajChar,userNames[j]);
				punctaje[j]=atof(punctajChar);
			}

			punctajPersonal=punctaje[i]; //salvam punctajul personal al userului
			for(int j=1;j<=nrClienti-1;j++) //sortam punctajele descrescator
			for(int z=j+1;z<=nrClienti;z++)
			if(punctaje[j]<punctaje[z])
			{
				aux=punctaje[j];
				punctaje[j]=punctaje[z];
				punctaje[z]=aux;

				strcpy(auxC,userNames[j]);
				strcpy(userNames[j],userNames[z]);
				strcpy(userNames[z],auxC);
			}

			strcpy(msgrasp,"Rezultate:\n");
			for(int j=1;j<=nrClienti;j++)
			{
				if(anonim==1)
				sprintf(punctajChar,"%d: %f\n",j,punctaje[j]);
				else
				sprintf(punctajChar,"%d: %s: %f\n",j,userNames[j],punctaje[j]);
				strcat(msgrasp,punctajChar);
			}
			sprintf(punctajChar,"\nPunctaj personal: %f",punctajPersonal);
			strcat(msgrasp,punctajChar);


    		printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);


    		/* returnam mesajul clientului */
    		if (scriePrefixat (client[i], msgrasp) < 0)
    		{
    			perror ("[server]Eroare la write() catre client.\n");
    			continue;		/* continuam sa ascultam */
    		}
    		else
    			printf ("[server]Mesajul a fost trasmis cu succes.\n");
    		
    		/* am terminat cu acest client, inchidem conexiunea */
    		close (client[i]);
    		exit(0);
		}
	  } /*for*/
	 } /*if(idClient==nrClienti)*/
    }				/* while */
}				/* main */