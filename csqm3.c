
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
  aux,
  impressora[3];             /*Impressora*/
  
  
float
  tc[3] = {4.5, 4.0, 3.5},  /* classe 0,1,2 tempo de cpu  */
  //td = 25.0, sd = 7.5,   /* disk time mean, std. dev. */
  td = 29.0, sd = 8.0,   /* disk time mean, std. dev. */
  timp[3] = {0.0, 35.0, 34.0}, sdimp[3] = {0.0, 11.0, 10.0};
  
main()
  {
    int i, j, event, n_intra[3], n_inter[1]; 
	
	float t, s_intra[3], s_inter[1], velRede = 1000000.0, tempo_rede; 
	
	struct token *p;
    
    n_intra[0] = n_intra[1] = n_intra[2] = 0; s_intra[0] = s_intra[1] = s_intra[2] = 0.0;
    n_inter[0] = 0; s_inter[0] = 0.0;
     
    for (i = 1; i <= n0; i++) 
        {
         task[i].cls = 2;//Classe 2
         task[i].loc = 1; //Local == intranet
        }
    for (i = n0+1; i <= n0+n1; i++)
        {
        task[i].cls = 1;  //Classe 1
        task[i].loc = 1; //Local == intranet
        }
    
    for (i = n0+n1+1; i <= n0+n1+n2; i++)
        {
        task[i].cls = 0;//Classe 0
        task[i].loc = 1;  //Local == intranet
         }
    for (i = n0+n1+n2+1; i <= nt; i++)
        {
         task[i].cls = 0; //classe 0
         task[i].loc = 2; //Local == internet
         }
    for (i = 1; i <= nt; i++){//Defiine os setores
        if(i==1 || i==4 || i==7 || i==10 || i==11 || i ==12) then task[i].setor = 1;
        else if(i==2 || i==5 || i==8 || i==13 || i==14) then task[i].setor = 2;
        else task[i].setor = 3; 
    }
         
    smpl(0,"Empresa ISTUE - Revista Agenda Capital");
    
    //trace(2);//Visualizar evento em tempo de simulacao
    
    cpu = facility("CPU",1);
    
    link[1] = facility("Link Ent Intra",1);
    link[2] = facility("Link Sai Intra",1);
    link[3] = facility("Link Ent Inter",1);
    link[4] = facility("Link Sai Inter",1);
      
    disk[1] = facility("Disco-1",1);
    disk[2] = facility("Disco-2",1);
    disk[3] = facility("Disco-3",1);
        
    impressora[1] = facility("Impressora-1",1);
    impressora[2] = facility("Impressora-2",1);
     
    stream(2);//Semente (Diretamente ligado a numeros randomicos )
    // agendamento intranet/internet:
    schedule(1,0.15,1);
    schedule(2,0.15,10);
    schedule(1,0.15,2);
    schedule(2,0.15,11);
    schedule(1,0.15,3);
    schedule(2,0.15,12);
    schedule(1,0.15,4);
    schedule(2,0.15,13);
    schedule(1,0.15,5);
    schedule(2,0.15,14);
    schedule(1,0.15,6);
    schedule(2,0.15,15);
    schedule(1,0.15,7);
    schedule(2,0.15,16);
    schedule(1,0.15,8);
    schedule(2,0.15,17);
    schedule(1,0.15,9);
    schedule(2,0.15,18);
       
    while (nts) //roda ate zero
      {
        
        cause(&event,&i); 
		p=&task[i];
        
        switch(event)
          {
            case 1:  /* begin tour intranet*/
            
              p->esc=0;
              p->ts=stime();
              
              do{ p->pac = expntl(420.0);
                } while (p->pac < 360.0 || p->pac > 480.0);
              
              aux = irandom(1,10);//Sorteio de 1 a 10 para calculo percentual
              
              if(aux >= 1 && aux <= 6) p->def_rec = 1;//def_rec = 1 para impressora
                     else p->def_rec = 2;//def_rec= 2 para discos
                     
			  schedule(3,0.0,i);
            break;
            
            case 2:  /* begin tour internet*/
              p->esc=0;
              p->ts=stime();
              //p->def_rec = 2;
              aux = irandom(1,10);//Sorteio de 1 a 10 para calculo percentual
              
              if(aux >= 1 && aux <= 6) p->def_rec = 1;//def_rec = 1 para impressora
                     else p->def_rec = 2;//def_rec= 2 para discos

              do{ p->pac = expntl(420.0);
                } while (p->pac < 360.0 || p->pac > 480.0);
              
              schedule(4,0.0,i);
            break;
            
            case 3: /* criando link de entrada intranet*/
                 j=p->cls;
                 tempo_rede = (p->pac * 8000.0) / velRede;
                 
                 if (j == 1) then tempo_rede = tempo_rede*1.05;
                 if (j == 0) then tempo_rede = tempo_rede*1.10;
                 
                 if (request(link[1],i,0)!= 1) then schedule(5,tempo_rede,i); 
            break;
            
            case 4: /* criando link de entrada internet*/
                 j=p->cls;
                 tempo_rede = (p->pac * 8000.0) / velRede;
                 
                 if (j == 1) then tempo_rede = tempo_rede*1.05;
                 if (j == 0) then tempo_rede = tempo_rede*1.10;
                                 
                 if (request(link[3],i,0)!= 1) then schedule(6,tempo_rede,i); 
            break;
            
            case 5: //Liberando o link de entrada intranet
              release(link[1],i); 
              schedule(7,0.0,i);
            break;
            
            case 6: //Liberando o link de entrada internet
              release(link[3],i); 
              schedule(7,0.0,i);
            break;
                        
            case 7:  /* request cpu */
              j=p->cls;
              if (preempt(cpu,i,j)!= 1) then schedule(8,tc[j],i);
            break;
              
            case 8:  /* release cpu, select disk */
              release(cpu,i);
              			  
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
                                    
		      else if(p->loc == 1) then schedule(13,0.0,i);
		           else schedule(14,0.0,i);
		           
            break;
              
            case 9:  /* request disk */
              if (i >= 1 && i <= 3 ) p->un=irandom(1,2,3); // sorteia disco entre 1,2 e 3 para Class 0
			  
              else{
			       if (p->setor == 1) then p->un = 1;
			       if (p->setor == 2) then p->un = 2;
			       if (p->setor == 3) then p->un = 3;
              }              
              if (request(disk[p->un],i,0)!= 1) then schedule(10,erlang(td,sd),i);
            break;
              
            case 10:  /* libera disco, end tour */
              release(disk[p->un],i); 
              p->esc=1;
              schedule(7,0.0,i);
              //if(p->loc == 1) then schedule(11,0.0,i);
              //if(p->loc == 2) then schedule(12,0.0,i);
			  //nts--;
            break;
            
            case 11:  /* request Impressora */
              if (p->setor == 3 || p->loc == 2) then p->sel_imp = 2;//direciona usuarios da internet (jornalistas) para impressora 2
              else p->sel_imp = 1;
              
              if (request(impressora[p->sel_imp],i,0)!= 1) then schedule(12,erlang(timp[p->sel_imp],sdimp[p->sel_imp]),i);
            break;
              
            case 12:  /* libera Impressora, end tour */
              release(impressora[p->sel_imp],i); 
              p->esc=1;
              schedule(7,0.0,i);
            break;
            
            case 13: //criando link 2 Intranet
                if (request(link[2],i,0)!= 1) then schedule(15,tempo_rede,i); 
            break;
            
            case 14: //criando link 4 Internet
                if (request(link[4],i,0)!= 1) then schedule(16,tempo_rede,i); 
            break;
            
            case 15://liberando link2 intranet e fazendo calculo do tempo
              release(link[2],i);
              j=p->cls;
              t=stime(); 
			  s_intra[j]+=t-p->ts; //variável nova
			  p->ts=t; 
			  n_intra[j]++; //variável nova
              
              nts--;
              schedule(1,0.0,i);
            break;
            
            case 16://liberando link4 internet e fazendo calculo do tempo
              release(link[4],i);
              j=p->cls;
              t=stime(); 
			  s_inter[j]+=t-p->ts;  //variável nova
			  p->ts=t; 
			  n_inter[j]++;  //variável nova
              
              nts--;
              schedule(2,0.0,i);
            break;
          }
      }
    reportf(); 
    printf("\n\n");
    printf("\n Tempo de ciclo da classe 0 Intranet = %8.2f ms",s_intra[0]/n_intra[0]);
    printf("\n Tempo de ciclo da classe 1 Intranet = %8.2f ms",s_intra[1]/n_intra[1]);
    printf("\n Tempo de ciclo da classe 0 Internet = %8.2f ms",s_inter[0]/n_inter[0]);
    printf("\n Tempo de ciclo da classe 1 Internet = %8.2f ms",s_inter[1]/n_inter[1]);
    printf("\n\n");
    system("pause");
  }


