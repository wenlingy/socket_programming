#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <vector>
#include <numeric> 

using namespace std;

#define MAX_C 4096
#define UDPPORT "24933"  //server well-known port
#define UDPPORTA "21933" // serverA port
#define UDPPORTB "2233" // serverB port
#define UDPPORTC "23933" // serverC port





//set udp for serverA

void SetUDP()
{
// begin phase2  
	int server_udp_soc_num=-1;
    int	aws_soc_num=-1;
	struct addrinfo hints , *server_udp , *aws_addr; //*hs -> myaddr
	struct sockaddr_storage clt_addr;  // AWS addr
	socklen_t addr_len;
	addr_len = sizeof(clt_addr);


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;//IPv4
	hints.ai_socktype = SOCK_DGRAM;//UDP

	
	// set back server A as UDP server
	if ((getaddrinfo("nunki.usc.edu", UDPPORTA, &hints, &server_udp)) != 0)
	{
		perror("serverA: get addr info fail\n");
		exit(1);
	}
	/*else{
		cout<<" serverA successfully get Back Server A addrinfo"<<endl;
	}*/

	if ((server_udp_soc_num = socket(server_udp->ai_family, server_udp->ai_socktype, server_udp->ai_protocol)) == -1)
	{
		perror("serverA:creat udp soc fail\n");
		exit(1);
	}
	/*else
	{
		cout<<" Back serverA successfully create socket"<<endl;
	}*/
     
	if ((bind(server_udp_soc_num, server_udp->ai_addr, server_udp->ai_addrlen)) == -1)
	{
		
		perror("serverA:bind fail\n");
		exit(1);
	}
	else{
		cout<<"The Server A is up and running using UDP on port "<<UDPPORTA<<"."<<endl;
	}
	
	
	// set back server A as UDP client to send data to AWS
	if ((getaddrinfo("nunki.usc.edu", UDPPORT, &hints, &aws_addr)) != 0)
	{
		perror("serverA: get aws addr info fail\n");
		exit(1);
	}
	/*else{
		cout<< "Back Server A successfully get AWS addrinfo"<<endl;
	}*/
	
	if ((aws_soc_num = socket(aws_addr->ai_family, aws_addr->ai_socktype, aws_addr->ai_protocol)) == -1)
	{
		perror("Back Server A:creat udp socket fail\n");
		exit(1);
	}
	/*else
	{
		cout<<"Back Server A successfully set Socket for AWS addrinfo"<<endl;
	}*/
		
		
	//start to receive nums from AWS
	
	while(1) 
	{
		int iRec=-1;
		int iSend=-1;
		vector<signed long int> rec_buf(MAX_C);
		
		// Wait for AWS to send data 
		//vector<int>::iterator first;
		iRec=recvfrom(server_udp_soc_num, rec_buf.data(), rec_buf.size()*sizeof(signed long int) , 0,(struct sockaddr *)&clt_addr, &addr_len);
	    if(iRec== -1)
		{
			perror(" serverA :recv nums not success\n");
			exit(1);
		}
		else
		{
			//to test received data informaton
			
			//print vector <int>
			// for(first= rec_buf.begin(); first<rec_buf.end(); first++)
			// {
			// cout<<*first<<" ";
			// }
			
			// pirnt function name
			//cout<<"\nfunction "<<rec_buf.front()<<endl;
			
			int iRec_nums=iRec/sizeof(signed long int)-1;
			vector<signed long int> result(MAX_C);

			cout<<"The Server A has received "<<iRec_nums<<" numbers"<<endl;
			
			if(rec_buf.front()==1){  // max
				rec_buf.erase(rec_buf.begin());
				vector<signed long int>::iterator max_pos=max_element(rec_buf.begin(),rec_buf.begin()+iRec_nums);
				cout<<"The Server A has successfully finished the reduction MAX:"<<*max_pos<<endl;
				result.insert(result.begin(),*max_pos);
				result.insert(result.begin(), 1);

                if((iSend= sendto(aws_soc_num, result.data(), result.size()*sizeof(signed long int), 0, aws_addr->ai_addr, aws_addr->ai_addrlen)) == -1)
                {
                	perror("Server A: send result to AWS has error\n");
                	exit(1);
                }
                else{
					//cout<<"The Server A send result, number is: "<<iSend<<endl;
					cout<<"The Server A has successfully finished sending the reduction value to AWS server."<<endl;
				}
			}
			else if(rec_buf.front()==2){ // min
				rec_buf.erase (rec_buf.begin());
				vector<signed long  int>::iterator min_pos=min_element(rec_buf.begin(),rec_buf.begin()+iRec_nums);
				cout<<"The Server A has successfully finished the reduction MIN:"<<*min_pos<<endl;
				result.insert(result.begin(),*min_pos);
				result.insert(result.begin(), 1);

				if((iSend= sendto(aws_soc_num, result.data(), result.size()*sizeof(signed long int), 0, aws_addr->ai_addr, aws_addr->ai_addrlen)) == -1)
                {
                	perror("Server A: send result to AWS has error\n");
                	exit(1);
                }
                else{
                	cout<<"The Server A has successfully finished sending the reduction value to AWS server."<<endl;
                }
			}
			else if(rec_buf.front()==3){ // sum
				rec_buf.erase (rec_buf.begin());
				signed long int sum = accumulate(rec_buf.begin(), rec_buf.begin()+iRec_nums+1, 0);
				cout<<"The Server A has successfully finished the reduction SUM:"<<sum<<endl;
				result.insert(result.begin(),sum);
				result.insert(result.begin(), 1);
				
				if((iSend= sendto(aws_soc_num, result.data(), result.size()*sizeof(signed long int), 0, aws_addr->ai_addr, aws_addr->ai_addrlen)) == -1)
                {
                	perror("Server A: send result to AWS has error\n");
                	exit(1);
                }
                else{
                	cout<<"The Server A has successfully finished sending the reduction value to AWS server."<<endl;
                }
			}
			else if(rec_buf.front()==4){ // sos
				rec_buf.erase (rec_buf.begin());
				signed long int sos = inner_product(rec_buf.begin(), rec_buf.begin()+iRec_nums, rec_buf.begin(), 0 );
				cout<<"The Server A has successfully finished the reduction SOS:"<<sos<<endl;
				result.insert(result.begin(),sos);
				result.insert(result.begin(), 1);

				if((iSend= sendto(aws_soc_num, result.data(), result.size()*sizeof(signed long int), 0, aws_addr->ai_addr, aws_addr->ai_addrlen)) == -1)
                {
                	perror("Server A: send result to AWS has error\n");
                	exit(1);
                }
                else{
                	cout<<"The Server A has successfully finished sending the reduction value to AWS server."<<endl;
                }
			}
		}
	}
}

int main()
{
	SetUDP();
	return 0;
}
