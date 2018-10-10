/**********************************************************************/
/*                                                                    */
/*         File csqm.c:  Central Server Queueing Network Model        */
/*                                                                    */
/*  This is the queueing network simulation program of Figure 2.2     */
/*  (with a call to "reportf()" added).                               */
/*                                                                    */
/**********************************************************************/

#include "smpl.h"
#include <stdlib.h>
#include <math.h>

#define n0 3 // jobs (editores) class 2 intranet 
#define n1 3 // jobs (redatores) class 1 intranet
#define n2 3 // jobs (secretarias) class 0 intranet  */
#define n3 9 // jobs (jornalistas) class 0 internet  */

#define nt n0+n1+n2+n3     /* total no. of tasks */

 
// mudança na quantidade de disco e no tempo de resposta do cpu
struct token
  {
    int cls;         /* task's class (& priority) */
    int un;          /* disk for current IO req.  */
    float ts;         /* tour start time stamp     */
    float pac;      /*representa pacotes*/
    int loc;             /*variavel que indica o local*/
    int esc;             /*variavel que indica se os discos ja foram acessados*/
    int def_rec;             /*variavel que indica qual recurso (imp ou disk) sera utilizado*/
    int sel_imp;             /*Varivavel que seleciona a impressora*/
    int setor;               /*Variavel que seleciona o setor*/
  } task[nt+1];

int
  disk[4],        /* disk facility descriptors */
  cpu,               /* cpu facility descriptor   */
  nts = 10000,         /* no. of tours to simulate  */
  link[5],             /*Links*/
  aux,                 //Variavel que guarda os valores gerados randomicamente
  impressora[3];             /*Impressora*/
  
  
float
  tc[3] = {4.5, 4.0, 3.5},  /* classe 0,1,2 tempo de cpu  */
  td = 29.0,   //Tempo de disco
  sd = 8.0,   /* disk time mean, std. dev. */
  timp[3] = {0.0, 35.0, 34.0}, //Tempo de ultilizacao das impressoras
  sdimp[3] = {0.0, 11.0, 10.0};//DP de impressoras
  
main()
  {
    int 
    i, //job que esta sendo tratano no instante t
    j, // represena a classe que esta sendo tratano no instante t
    event, // representa o evento
    n_intra[3], //vetor de links de entrada da intranet
    n_inter[1]; //vetor de link de entrada da internet
	
	float 
    t, //Variável que recebe tempo de execução
    s_intra[3], //Vetor links de sainda da intranet
    s_inter[1], //Vetor links de sainda da internet
    velRede = 2000000.0, //Velocidade de rede estupulada
    tempo_rede; //Variável que recebe calculo do tempo da rede
	
	struct token *p; //Ponteiro de acesso à estruruda de informações sobre os jobs
    
    n_intra[0] = n_intra[1] = n_intra[2] = 0; //iniciando os vetores link de entrada da intranet
    s_intra[0] = s_intra[1] = s_intra[2] = 0.0; //iniciando vetores link de saida da intranet
    n_inter[0] = 0; //iniciando vetor link de entrada internet
    s_inter[0] = 0.0; //iniciando vetor link de Saida internet
     
    for (i = 1; i <= n0; i++) //definindo classe e local para os tres pirmeiros jobs (Editores)
        {
         task[i].cls = 2;//Classe 2
         task[i].loc = 1; //Local == intranet
        }
    for (i = n0+1; i <= n0+n1; i++) //definindo classe e local para os jobs entre 4 e 6 (Redatores)
        {
        task[i].cls = 1;  //Classe 1
        task[i].loc = 1; //Local == intranet
        }
    
    for (i = n0+n1+1; i <= n0+n1+n2; i++) //definindo classe e local para os jobs entre 7 e 9 (Secretários)
        {
        task[i].cls = 0;//Classe 0
        task[i].loc = 1;  //Local == intranet
         }
    for (i = n0+n1+n2+1; i <= nt; i++) //definindo classe e local para os jobs entre 10 e 18 (Jornalistas)
        {
         task[i].cls = 0; //classe 0
         task[i].loc = 2; //Local == internet
         }
    for (i = 1; i <= nt; i++){//Defiine os setores para os jobs
        if(i==1 || i==4 || i==7 || i==10 || i==11 || i ==12) then task[i].setor = 1; //jobs para setor 1
        else if(i==2 || i==5 || i==8 || i==13 || i==14) then task[i].setor = 2; //jobs para setor 2
        else task[i].setor = 3; //jobs para setor 3
    }
         
    smpl(0,"Empresa ISTUE - Revista Agenda Capital"); //Nome da empresa na simulação
    
    cpu = facility("CPU",1); // recurso de CPU
    
    link[1] = facility("Link Ent Intra",1); //link de entrada da intranet
    link[2] = facility("Link Sai Intra",1);//link de Saida da intranet
    link[3] = facility("Link Ent Inter",1);//link de entrada da internet
    link[4] = facility("Link Sai Inter",1);//link de saida da internet
      
    disk[1] = facility("Disco-1",1); //disco 1
    disk[2] = facility("Disco-2",1); //disco 2
    disk[3] = facility("Disco-3",1); //disco 3
        
    impressora[1] = facility("Impressora-1(IMPA)",1); //Criando recurso impressora 1
    impressora[2] = facility("Impressora-2(IMPB)",1); //Criando recurso impressora 2
     
    stream(5);//Semente (Diretamente ligado a numeros randomicos )
    
    // agendamento intranet/internet:
    schedule(1,0.15,1);//Agendando job 1 para inicio de ciclo intranet
    schedule(2,0.15,10);//Agendando job 10 para inicio de ciclo internet
    schedule(1,0.15,2); //Agendando job 2 para inicio de ciclo intranet
    schedule(2,0.15,11);//Agendando job 11 para inicio de ciclo internet
    schedule(1,0.15,3);//Agendando job 3 para inicio de ciclo intranet
    schedule(2,0.15,12);//Agendando job 12 para inicio de ciclo internet
    schedule(1,0.15,4);//Agendando job 4 para inicio de ciclo intranet
    schedule(2,0.15,13);//Agendando job 13 para inicio de ciclo internet
    schedule(1,0.15,5);//Agendando job 5 para inicio de ciclo intranet
    schedule(2,0.15,14);//Agendando job 14 para inicio de ciclo internet
    schedule(1,0.15,6);//Agendando job 6 para inicio de ciclo intranet
    schedule(2,0.15,15);//Agendando job 15 para inicio de ciclo internet
    schedule(1,0.15,7);//Agendando job 7 para inicio de ciclo intranet
    schedule(2,0.15,16);//Agendando job 16 para inicio de ciclo internet
    schedule(1,0.15,8);//Agendando job 8 para inicio de ciclo intranet
    schedule(2,0.15,17);//Agendando job 17 para inicio de ciclo internet
    schedule(1,0.15,9);//Agendando job 9 para inicio de ciclo intranet
    schedule(2,0.15,18);//Agendando job 18 para inicio de ciclo internet
       
    while (nts) //roda ate zero o numero de simulações
      {
        
        cause(&event,&i); //Direciona job para case
		p=&task[i];// ponteiro esta apontando para a estrutura de atributos do job
        
        switch(event)
          {
            case 1:  /* begin tour intranet*/
            
              p->esc=0;//indicando que o job não passou por nenhum recurso
              p->ts=stime();//guardanto tempo inicial de simulação
              
              do{ p->pac = expntl(420.0);//Gerando tamanho do pacote 
                } while (p->pac < 360.0 || p->pac > 480.0); //tamanho do pacote com variação para mais ou menos 60 em relação ao tamanho médio
              
              aux = irandom(1,10);//Sorteio de 1 a 10 para calculo percentual
              
              if(aux >= 1 && aux <= 6) p->def_rec = 1;//def_rec = 1 para impressora
                     else p->def_rec = 2;//def_rec= 2 para discos
                     
			  schedule(3,0.0,i);//passando para o proximo case
            break;
            
            case 2:  /* begin tour internet*/
              p->esc=0;//indicando que o job não passou por nenhum recurso
              p->ts=stime();//guardanto tempo inicial de simulação
              aux = irandom(1,10);//Sorteio de 1 a 10 para calculo percentual
              
              if(aux >= 1 && aux <= 6) p->def_rec = 1;//def_rec = 1 para impressora
                     else p->def_rec = 2;//def_rec= 2 para discos

              do{ p->pac = expntl(420.0);//Gerando tamanho do pacote 
                } while (p->pac < 360.0 || p->pac > 480.0);//tamanho do pacote com variação para mais ou menos 60 em relação ao tamanho médio
              
              schedule(4,0.0,i);//passando para o proximo case
            break;
            
            case 3: /* criando link de entrada intranet*/
                 j=p->cls; //capturando de qual classe pertence o job atual
                 tempo_rede = (p->pac * 8000.0) / velRede; //realizando cálculo de rede
                 
                 if (j == 1) then tempo_rede = tempo_rede*1.05;//caso o job pertenca a classe 1 acrecenta-se o delay de 105% do tempo de rede
                 if (j == 0) then tempo_rede = tempo_rede*1.10;//caso o job pertenca a classe 1 acrecenta-se o delay de 110% do tempo de rede
                 
                 if (request(link[1],i,0)!= 1) then schedule(5,tempo_rede,i); //requisitando link de rede e passando para proximo case com o delay da classe
            break;
            
            case 4: /* criando link de entrada internet*/
                 j=p->cls;//capturando de qual classe pertence o job atual
                 tempo_rede = (p->pac * 8000.0) / velRede;//realizando cálculo de rede
                 
                 if (j == 1) then tempo_rede = tempo_rede*1.05;//caso o job pertenca a classe 1 acrecenta-se o delay de 105% do tempo de rede
                 if (j == 0) then tempo_rede = tempo_rede*1.10;//caso o job pertenca a classe 1 acrecenta-se o delay de 110% do tempo de rede
                                 
                 if (request(link[3],i,0)!= 1) then schedule(6,tempo_rede,i);//requisitando link de rede e passando para proximo case com o delay da classe
            break;
            
            case 5: //Liberando o link de entrada intranet
              release(link[1],i); //Liberando o link de entrada intranet
              schedule(7,0.0,i); //passando para proximo case de requisição de cpu
            break;
            
            case 6: //Liberando o link de entrada internet
              release(link[3],i); //Liberando o link de entrada internet
              schedule(7,0.0,i); //passando para proximo case de requisição de cpu
            break;
                        
            case 7:  /* request cpu */
              j=p->cls; //capturando de qual classe pertence o job atual
              if (preempt(cpu,i,j)!= 1) then schedule(8,tc[j],i); //requisitando a cpu com o tempo de prioridade referente a classe atual 
            break;
              
            case 8:  /* release cpu, select disk */
              release(cpu,i); //Liberando cpu
              			  
		      if (p->esc == 0) then{//Caso nao tenha escolhido nenhum recurso
                 if (p->def_rec == 2) //Caso o recurso seja disco
                  {              
                    if( i == 1 || i == 2 || i == 3) then p->un = irandom(1,3); //selecionar disco para editores
                    else if(i == 4 || i == 7 || i == 10 || i == 11 || i == 12) then p->un = 1; // disco para o setor 1
                    else if(i == 5 || i == 8 || i == 12 || i == 13 ) then p->un = 2; //Disco para o setor 2
                    else p->un = 3; //Disco para o setor 3
                    schedule(9,0.0,i);//Agenda para selecionar Disco
                  }
                  else //Condição de impressora
                  {
                  	schedule(11,0.0,i);//Agenda para selecionar impressora
                  }
                           
                 }
              //caso já tenha acessado algum componente (impressora ou disco), direcionar para os links de saida
		      else if(p->loc == 1) then schedule(13,0.0,i); //direcionar para o link de intranet
		           else schedule(14,0.0,i);//direcionar para o link de internet
		           
            break;
              
            case 9:  /* request disk */
              if (i >= 1 && i <= 3 ) p->un=irandom(1,2,3); // sorteia disco entre 1,2 e 3 para Class 0
			  
              else{// selecionar determinados discos para as demais classes
			       if (p->setor == 1) then p->un = 1; //dados do setor 1 para o disco 1
			       if (p->setor == 2) then p->un = 2; //dados do setor 2 para o disco 2
			       if (p->setor == 3) then p->un = 3; //dados do setor 3 para o disco 3
              }              
              if (request(disk[p->un],i,0)!= 1) then schedule(10,erlang(td,sd),i); //requisitando disco com o tempo de acordo com cada um
            break;
              
            case 10:  /* libera disco, end tour */
              release(disk[p->un],i); //liberando disco
              p->esc=1; //sentando que o sistema já realizou alguma operação 
              schedule(7,0.0,i); //voltando para o case da CPU
            break;
            
            case 11:  /* request Impressora */
              if (p->setor == 3 || p->loc == 2) then p->sel_imp = 2;//direciona usuarios da internet (jornalistas) para impressora 2
              else p->sel_imp = 1; //demais usuários para impressora 1
              
              if (request(impressora[p->sel_imp],i,0)!= 1) then schedule(12,erlang(timp[p->sel_imp],sdimp[p->sel_imp]),i); //requisitando impressora com o tempo referente a cada uma
            break;
              
            case 12:  /* libera Impressora, end tour */
              release(impressora[p->sel_imp],i); //liberando recurso
              p->esc=1;//sentando que o sistema já realizou alguma operação 
              schedule(7,0.0,i);//voltando para o case da CPU
            break;
            
            case 13: //criando link 2 Intranet
                 j=p->cls;//capturando de qual classe pertence o job atual
                 tempo_rede = (p->pac * 8000.0) / velRede;//realizando cálculo de rede
                 
                 if (j == 1) then tempo_rede = tempo_rede*1.05;//caso o job pertenca a classe 1 acrecenta-se o delay de 105% do tempo de rede
                 if (j == 0) then tempo_rede = tempo_rede*1.10;//caso o job pertenca a classe 1 acrecenta-se o delay de 110% do tempo de rede
                 if (request(link[2],i,0)!= 1) then schedule(15,tempo_rede,i);//requisitando link de rede e passando para proximo case com o delay da classe
            break;
            
            case 14: //criando link 4 Internet
                 j=p->cls;//capturando de qual classe pertence o job atual
                 tempo_rede = (p->pac * 8000.0) / velRede;//realizando cálculo de rede
                 
                 if (j == 1) then tempo_rede = tempo_rede*1.05;//caso o job pertenca a classe 1 acrecenta-se o delay de 105% do tempo de rede
                 if (j == 0) then tempo_rede = tempo_rede*1.10;//caso o job pertenca a classe 1 acrecenta-se o delay de 110% do tempo de rede
                if (request(link[4],i,0)!= 1) then schedule(16,tempo_rede,i);//requisitando link de rede e passando para proximo case com o delay da classe
            break;
            
            case 15://liberando link2 intranet e fazendo calculo do tempo
              release(link[2],i); //liberando link de intranet
              j=p->cls;//capturando de qual classe pertence o job atual
              t=stime();//capturando tempo atual do sistema e armazenando na variavel de tempo 
			  s_intra[j]+=t-p->ts; //somando tempo de simulação em relação ao uso do link de intranet
			  p->ts=t; //armazenando tempo de simulação do job
			  n_intra[j]++; //somando o numero de requisições que foram feitas no link de intranet
              nts--;//decrescentando o numero de simulações
              schedule(1,0.0,i);//voltando para o case de intranet
            break;
            
            case 16://liberando link4 internet e fazendo calculo do tempo
              release(link[4],i);//liberando link de internet
              j=p->cls;//capturando de qual classe pertence o job atual
              t=stime(); //capturando tempo atual do sistema e armazenando na variavel de tempo 
			  s_inter[j]+=t-p->ts; //somando tempo de simulação em relação ao uso do link de internet
			  p->ts=t; //armazenando tempo de simulação do job
			  n_inter[j]++; //somando o numero de requisições que foram feitas no link de internet
              nts--;//decrescentando o numero de simulações
              schedule(2,0.0,i);//voltando para o case de internet
            break;
          }
      }
    reportf(); //imprimir resultados na tela 
    printf("\n\n");
    printf("\n Tempo de ciclo da classe 0 Intranet (secretarios) = %8.2f ms",s_intra[0]/n_intra[0]);//tempo de simulação de rede referente aos secretários
    printf("\n Tempo de ciclo da classe 1 Intranet (Redatores) = %8.2f ms",s_intra[1]/n_intra[1]);//tempo de simulação de rede referente aos Redatores
    printf("\n Tempo de ciclo da classe 2 Intranet (editores) = %8.2f ms",s_intra[2]/n_intra[2]);//tempo de simulação de rede referente aos Editores
    printf("\n Tempo de ciclo da classe 0 Internet (jornalistas) = %8.2f ms",s_inter[0]/n_inter[0]);//tempo de simulação de rede referente aos Jornalistas
    printf("\n\n");
    system("pause");
  }
