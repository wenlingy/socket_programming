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


using namespace std;

#define SERVERPORT "25933"  //server well-known port
#define MAX_C 4096 



int main(int argc, char* argv[])
{
	sleep(1);// waiting for AWS initializing
	
//SetTCP

//set up client connectionc
    struct addrinfo hints, *cl;
	string function_name = argv[1];
    int cl_skt=0;
    memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	if (( getaddrinfo("nunki.usc.edu", SERVERPORT, &hints, &cl)) != 0)
	{
		perror("Client: getaddrinfo failure\n");
		exit(0);
	}
	
	if ((cl_skt = socket(cl->ai_family, cl->ai_socktype, cl->ai_protocol)) == -1)
	{
		perror("Client: Socket Error\n");
		exit(0);
	}


	if ((connect(cl_skt, cl->ai_addr, cl->ai_addrlen)) == -1)
	{
		perror("Client: Connect error");
		exit(0);
	}
	else{
		cout<<"The client is up and running."<<endl;	
	}
	
	// receiving data information to AWS for testing
	int iSend=0;
	int iRec=0;
	// vector<char> rec_buf(MAX_C);

	// if(( iRec= recv(cl_skt, rec_buf.data(), rec_buf.size()*sizeof(int), 0)) == -1)
	// {
	// 	perror("Client: recv failure has error\n");
	// 	exit(1);
	// }
	// else
	// {
	// 	cout<<"The client is now connected to AWS "<<endl;
	// 	for(int i =0 ; i< rec_buf.size(); i++){
	// 		cout<< rec_buf[i];
	// 	}
	// 	cout << "\n";	
	// }

	// Receive the connection confirmation from the server.
    // end of set up connection

	
	/*Sending data information*/
	/*Read data information file*/
	ifstream infile;
	string line;
	string nums_str;
	vector<signed long int> nums_int; // Create vector to hold our num
	signed long int count = 0;
	infile.open("nums.csv");
	
	if (infile.is_open())
	{
		while(getline(infile,line))
		{
			nums_str.append(line);
			nums_str.append(" ");
			count++;
		}
		stringstream tmp;
		tmp<<count;
		nums_str.insert(0,tmp.str()+" ");
		nums_str.insert(0,function_name+" "); //insert function_name

	}


	int pos1 = nums_str.find(" ");
		
	function_name = nums_str.substr(0,pos1);
    //cout << "function found: "<< function_name<< endl;
	string function_type_list[] = {"MAX", "MIN", "SUM", "SOS"};
	signed long int function_name_count;
	if(function_name =="max"){
		function_name_count=1;
	}
	else if(function_name =="min"){
		function_name_count=2;
	}
	else if(function_name =="sum"){
		function_name_count=3;
	}	
	else if(function_name =="sos"){
		function_name_count=4;
	}
		
	// find count's space
	int pos2 = nums_str.find(" ", pos1+1);
		
	string count_str = nums_str.substr(pos1+1, pos2 - pos1 -1);
	//cout << "count found: "<< count_str<< endl;
		
	// convert count_str into integer count_int
	istringstream count_tmp(count_str);
	count_tmp >> count;
	//cout << "count converted to integer: "<< count<< endl;
		
	// push back function name
	nums_int.push_back(function_name_count);

	// push back total number count
	nums_int.push_back(count);
	istringstream iss;
	iss.str(nums_str.substr(pos2+1,nums_str.length() - pos2 - 1)); //data information
		
	//push all vals
	for(int n=0; n<count;n++){
		signed long int val;
		iss >> val;
		nums_int.push_back(val);
		//cout << val << " ";
	}


	if((iSend = send(cl_skt, nums_int.data(), nums_int.size()*sizeof(signed long int), 0)) == -1)
	{
		perror("The client: send data information has error\n");
		exit(1);
	}
	else{
		cout<<"The client has sent the reduction type "<<function_type_list[function_name_count-1] <<" to AWS."<<endl;
		cout<<"The client has sent "<<count<<" numbers to AWS"<<endl;
	}
	infile.close();

	vector<signed long int> result_buf(MAX_C);
	//Waiting for the results from AWS
	if((iRec= recv(cl_skt, result_buf.data(), result_buf.size()*sizeof(signed long int), 0)) == -1)
	{
		perror("The client: recv failure or success has error\n");
		exit(1);
	}
	else
	{
		//print the result received from aws
		cout<<"The client has received reduction "<<function_type_list[function_name_count-1]<<": "<<result_buf.front()<<endl;
		close(cl_skt);
	}
	return 0;
}
