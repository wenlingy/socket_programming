#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
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


#define BACKLOG 10 // the maximum pending connections in the queue
#define MAX_C 4096
#define TCPPORT "25933" // tcp port to connect client
#define UDPPORT "24933" // AWS
#define UDPPORT_serverA "21933" //serverA udp port
#define UDPPORT_serverB "22933" //serverB udp port
#define UDPPORT_serverC "23933" //serverC udp port

//vector<char> buffer(MAX_C);
vector<long signed int> buf1(MAX_C);



struct sockaddr_storage clt_addr;


void phase1_fcn(int aws_tcp_soc_num, int aws_udp_soc_num){
	int tempSockDescp=aws_tcp_soc_num;
	int iRecLen=-1;
	int iSenLen=-1;
	vector<signed long int> nums_int; // Create vector to hold our num
	// stringstream ss;
	// string msg = "Wenling!";
	// if ((iSenLen= send(tempSockDescp,msg.data(), msg.length(), 0)) == -1)
	// {
	// 	perror("AWS: phase1 can't sent success/failure msg\n");
	// }
	// connection success information sending ends
	
	/* Receive the data information from the client*/
	
	//  packet
	if ((iRecLen= recv(tempSockDescp, buf1.data(), buf1.size()*sizeof(long signed int), 0)) == -1)
	{
		perror("AWS:recv error\n");
		exit(1);
	}
	else
	{
		
		signed long int function_name_count = buf1[0] ;
		signed long int count =buf1[1];
		buf1.erase(buf1.begin()+1);
		//cout << "count converted to integer: "<< count<< endl;
		vector<signed long int>::iterator tmp;
		for(tmp= buf1.begin(); tmp<=buf1.begin() + count; tmp++)
		{
			nums_int.push_back(*tmp);
		}	

		cout<<"The AWS has received "<<nums_int.size() - 1<<" numbers from the client using TCP over port "<<TCPPORT<<endl;
	}
	

	
	
	     //set udp for AWS as server to receive from back_server_A
		int	clt_soc_num_udp_server_A=-1;
		int	clt_soc_num_udp_server_B=-1;
		int	clt_soc_num_udp_server_C=-1;
		struct addrinfo hints, *clt_s_a, *clt_s_b, *clt_s_c; //remote addr
		
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;//IPv4
		hints.ai_socktype = SOCK_DGRAM;//UDP

		


		//AWS as a UDP client to send data to back server A
		if ((getaddrinfo("nunki.usc.edu", UDPPORT_serverA, &hints, &clt_s_a)) != 0) 
		{
			perror("AWS: get udp addr info fail\n");
			exit(1);
		}
		// else
		// {
		// 	cout<<" AWS successfully get Back Server A addrinfo"<<endl;
		// }

		if ((clt_soc_num_udp_server_A = socket(clt_s_a->ai_family, clt_s_a->ai_socktype, clt_s_a->ai_protocol)) == -1)
		{
			perror("AWS:creat udp socket fail\n");
			exit(1);
		}
		// else
		// {
		// 	cout<<"AWS successfully set Socket for Back Server A addrinfo"<<endl;
		// }
		
		
		//AWS as a UDP client to send data to back server B
		if ((getaddrinfo("nunki.usc.edu", UDPPORT_serverB, &hints, &clt_s_b)) != 0) 
		{
			perror("AWS: get udp addr info fail\n");
			exit(1);
		}
		// else
		// {
		// 	cout<<"AWS successfully get Back Server B addrinfo"<<endl;
		// }


		if ((clt_soc_num_udp_server_B = socket(clt_s_b->ai_family, clt_s_b->ai_socktype, clt_s_b->ai_protocol)) == -1)
		{
			perror("AWS:creat udp socket fail\n");
			exit(1);
		}
		// else
		// {
		// 	cout<<"AWS successfully get Socket for Back Server B addrinfo"<<endl;
		// }
		
		//AWS as a UDP client to send data to back server C
		if ((getaddrinfo("nunki.usc.edu", UDPPORT_serverC, &hints, &clt_s_c)) != 0) 
		{
			perror("AWS: get udp addr info fail\n");
			exit(1);
		}
		// else
		// {
		// 	cout<<"AWS successfully get Back Server C addrinfo"<<endl;
		// }


		if ((clt_soc_num_udp_server_C = socket(clt_s_c->ai_family, clt_s_c->ai_socktype, clt_s_c->ai_protocol)) == -1)
		{
			perror("AWS:creat udp socket fail\n");
			exit(1);
		}
		// else
		// {
		// 	cout<<"AWS successfully get Socket for Back Server C addrinfo"<<endl;
		// }
	
//send nums divided into 3 parts: A,B,C
	
		//sent nums to serverA
		
		int iRec=-1;
		int iSend=-1;
		int nums_server=(nums_int.size())/3;
		
		//save the part of data to vetor
		
		vector<signed long int> datas_serverA;
		vector<signed long int>::iterator firstA;
		for(firstA= nums_int.begin(); firstA<nums_int.begin()+ nums_server+1; firstA++)
		{
			datas_serverA.push_back (*firstA);
		}		
		
		if((iSend= sendto(clt_soc_num_udp_server_A, datas_serverA.data(), datas_serverA.size()*sizeof(signed long int), 0, clt_s_a->ai_addr, clt_s_a->ai_addrlen))== -1)
		{
			perror("AWS: send data information to serverA has error\n");
			exit(1);
		}
        else
        {
            //for(firstA= datas_serverA.begin(); firstA<datas_serverA.end(); firstA++){
			//cout<<*firstA<<" ";
            //}
			cout<<"The AWS has sent "<<iSend/sizeof(signed long int) - 1 <<" numbers to Backend-Server A"<<endl;
		}	
		
		//sent nums to serverB
		
		vector<signed long int> datas_serverB;
		vector<signed long int>::iterator firstB;
		datas_serverB.push_back(datas_serverA.front());
		for(firstB= nums_int.begin()+nums_server+1; firstB<nums_int.begin()+ 2*nums_server+1; firstB++)
		{
			datas_serverB.push_back (*firstB);
		}	
		if((iSend = sendto(clt_soc_num_udp_server_B, datas_serverB.data(), datas_serverB.size()*sizeof(signed long int), 0, clt_s_b->ai_addr, clt_s_b->ai_addrlen)) == -1)
		{
			perror("AWS: send data information to serverB has error\n");
			exit(1);
		}
		else{
			cout<<"The AWS has sent "<<iSend/sizeof(signed long int) - 1 <<" numbers to Backend-Server B"<<endl;;
		}

		

		//send nums to serverC
		vector<signed long int> datas_serverC;
		vector<signed long int>::iterator firstC;
		datas_serverC.push_back(datas_serverA.front());
		for(firstC= nums_int.begin()+ 2*nums_server+1; firstC<nums_int.end(); firstC++)
		{
			datas_serverC.push_back (*firstC);
		}	
		if((iSend = sendto(clt_soc_num_udp_server_C, datas_serverC.data(), datas_serverC.size()*sizeof(signed long int), 0, clt_s_c->ai_addr, clt_s_c->ai_addrlen)) == -1)
		{
			perror("AWS: send data information to serverC has error\n");
			exit(1);
		}    
		else{
			cout<<"The AWS has sent "<<iSend/sizeof(signed long int) - 1 <<" numbers to Backend-Server C"<<endl;
		}
		

		//to receive the result from backend server A, B, C
		vector<signed long int> buffer_result(MAX_C);
		vector<signed long int> result;

		struct sockaddr_storage clt_addr;  // to store the addrinfo about server A B C
		socklen_t addr_len;
		addr_len = sizeof(clt_addr);
		int ready_to_send = 0;
		while(1){
			if((iRec= recvfrom(aws_udp_soc_num, buffer_result.data(), sizeof(signed long int)*buffer_result.size() , 0, (struct sockaddr *)&clt_addr, &addr_len))== -1){
            	perror("AWS: receive result from serverABC has error\n");
            	exit(1);
			}
			else{
				int server_number=0;
				string server_port_udp;
				string server_udp;
				string function;

				//which backend server
				if(buffer_result.front()==1){
					server_port_udp=UDPPORT_serverA;
					server_udp="Backend-ServerA";
				}
				else if(buffer_result.front()==2){
					server_port_udp=UDPPORT_serverB;
					server_udp="Backend-ServerB";
				}
				else if(buffer_result.front()==3){
					server_port_udp=UDPPORT_serverC;
					server_udp="Backend-ServerC";
				}

				//which function
				if(datas_serverA.front()==1){
					function="MAX";
				}
				else if(datas_serverA.front()==2){
					function="MIN";
				}
				else if(datas_serverA.front()==3){
					function="SUM";
				}
				else if(datas_serverA.front()==4){
					function="SOS";
				}
				buffer_result.erase (buffer_result.begin());
				result.push_back(buffer_result.front());
				cout<<"The AWS received reduction result of "<<function<<" from "<<server_udp<<" using UDP over port "<<server_port_udp<<" and it is "<<result.back()<<endl;

				ready_to_send++;
				//cout <<"ready to send: "<< ready_to_send << endl;


				if(ready_to_send==3){
					//cout<<"total results number is "<<result.size()<<endl;
					//for(firstA= result.begin(); firstA<result.end(); firstA++)
					// {
					// 	cout<<*firstA<<endl;
					// }		
					ready_to_send = 0;
					//calculate function
					if(datas_serverA.front()==1){ //max
						vector<signed long int>::iterator max_pos=max_element(result.begin(),result.end());
						cout<<"The AWS has successfully finished the reduction MAX:"<<*max_pos<<endl;
						//send result to client
						int max= *max_pos;
						if ((iSenLen= send(tempSockDescp, &max, sizeof(max), 0)) == -1){
							perror("AWS cannot sent result success/failure msg\n");
							}
					}
					else if(datas_serverA.front()==2){ //min
						vector<signed long int>::iterator min_pos=min_element(result.begin(),result.end());
						cout<<"The AWS has successfully finished the reduction MIN:"<<*min_pos<<endl;
						//send result to client
						int min= *min_pos;
						if ((iSenLen= send(tempSockDescp, &min, sizeof(min), 0)) == -1){
							perror("AWS cannot sent result success/failure msg\n");
							}
					}
					else if(datas_serverA.front()==3){ //sum
						int sum=0;
						for(vector<signed long int>::iterator i=result.begin(); i!=result.end(); i++){
							sum+=*i;
						}
						cout<<"The AWS has successfully finished the reduction SUM:"<<sum<<endl;
						//send result to client
						if ((iSenLen= send(tempSockDescp,&sum, sizeof(sum), 0)) == -1){
							perror("AWS cannot sent result success/failure msg\n");
							}
					}
					else if(datas_serverA.front()==4){ //sos
						int sos=0;
						for(vector<signed long int>::iterator j=result.begin(); j!=result.end(); j++){
							sos+=*j;
						}
						cout<<"The AWS has successfully finished the reduction SOS:"<<sos<<endl;
						//send result to client
						if ((iSenLen= send(tempSockDescp,&sos, sizeof(sos), 0)) == -1){
							perror("AWS cannot sent result success/failure msg\n");
							}
						else{
							cout<<"The AWS has successfully finished sending the reduction value to client."<<endl;
						}
					}
					return;
				}
			}
		}
}



void SetTCPandSetUDP()
{
  // set tcp 
  
    int status;
    struct addrinfo hints_tcp;
    struct addrinfo *aws_tcp_s;  //will point to the result
	int sockfd;
	
	socklen_t addr_size;
	
	
	// AWS set TCP
	memset(&hints_tcp, 0, sizeof hints_tcp); // make sure the struct is empty
	hints_tcp.ai_family = AF_INET;// family type of socket
	hints_tcp.ai_socktype = SOCK_STREAM;// socket type
	hints_tcp.ai_flags = AI_PASSIVE;//fill in IP


	if ((status = getaddrinfo("nunki.usc.edu", TCPPORT, &hints_tcp, &aws_tcp_s)) != 0)
	{
		perror("AWS server: getaddrinfo failure\n");
		exit(1);
	}
	
	if ((sockfd = socket(aws_tcp_s->ai_family, aws_tcp_s->ai_socktype, aws_tcp_s->ai_protocol)) == -1)
	{
		perror("AWS server: socket Error\n");
		exit(1);
	}
	 
	if ((bind(sockfd, aws_tcp_s->ai_addr, aws_tcp_s->ai_addrlen)) == -1)
	{
		perror("AWS server: bind Error occurred:");
		exit(1);
	}

	if (( listen(sockfd, BACKLOG)) == -1)
	{
		perror("AWS server: listen Error occurred:");
		exit(1);
	}
	else{
		cout<<"The AWS is up and running."<<endl;
	}	
	freeaddrinfo(aws_tcp_s);
	
	
	//set AWS as a UDP server
	int aws_udp_soc_num=-1;
	struct addrinfo hints_udp , *aws_udp_s; //remote addr	
	
	memset(&hints_udp, 0, sizeof(hints_udp));
	hints_udp.ai_family = AF_INET;//IPv4
	hints_udp.ai_socktype = SOCK_DGRAM;//UDP
	
	if((getaddrinfo("nunki.usc.edu", UDPPORT, &hints_udp, &aws_udp_s))!= 0)
	{
		perror("AWS: get udp addr info fail\n");
		exit(1);
	}

	if((aws_udp_soc_num = socket(aws_udp_s->ai_family, aws_udp_s->ai_socktype, aws_udp_s->ai_protocol)) == -1)
	{
		perror("AWS:creat udp socket fail\n");
		exit(1);
	}
	if((bind(aws_udp_soc_num, aws_udp_s->ai_addr, aws_udp_s->ai_addrlen))== -1){
            perror("AWS:bind fail\n");
            exit(1);
	}
	// else{
	// 	cout<<"aws is up and running through udp binding success."<<endl;
	// }
	freeaddrinfo(aws_udp_s);	

	int new_fd=-1;
	addr_size = sizeof(clt_addr);
	int pid; //process id
	while(1)
	{
		if ((new_fd = accept(sockfd, (struct sockaddr *)&clt_addr, &addr_size))== -1)
		{
			perror("AWS server: Accept ERROR\n");
			exit(1);
		}
		
		if((pid = fork()) == -1)
		{
			close(new_fd);
			continue;
		}
		else if(pid >0)
		{
			close(new_fd);
		}
		else if(pid == 0)
		{
			phase1_fcn(new_fd,aws_udp_soc_num);
			close(new_fd);
			exit(0);
		}
	}
}      

int main()
{
	SetTCPandSetUDP();
	return 0;
}
