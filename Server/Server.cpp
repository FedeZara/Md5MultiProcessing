// Server.cpp: definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <direct.h>  
#include <comdef.h>

// struttura per informazioni sulla finestra di esecuzione
STARTUPINFO startup_window;
// struttura per informazioni sul processo e il thread principale
PROCESS_INFORMATION child_process;
int main(int argc, char *argv[])
{
	//creazione delle variabili
	FILE *file;//Puntatore a file
	unsigned char buffer[16];//buffer di lettere
	int n;
	DWORD i;
	HANDLE pipe;						//dichiaro una nuova variabile di tipo HANDLE
	if (argc != 2)
	{
		printf("Uso: %s file\r\n", argv[0]);
		return -1;
	}
	// creazione pipe
	pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\my_pipe"),
		PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 1024, 0, 0, NULL);

	// controllo se ci sono stati errori nella connessione alla pipe
	if (pipe == INVALID_HANDLE_VALUE)
	{
		printf("Errore creazione named-pipe\r\n");
		return -1;											//ritorno -1 in caso di errore
	}
	// apertura in lettura file
	fopen_s(&file, argv[1], "rb");
	if (file == NULL)
	{
		printf("Errore apertura file \"%s\"\r\n", argv[1]);
		CloseHandle(pipe); // chiusura pipe
		return -1;
	}

	WCHAR path[80];

	GetModuleFileName(NULL, path, MAX_PATH);
	
	wchar_t *substr_file;
	substr_file = wcsstr(path, L"\\Server.exe");
	wcscpy(substr_file, L"\\Client.exe");

	// creazione processo client
	if (CreateProcess(path, NULL, NULL, NULL, TRUE, 0, NULL,
		NULL, &startup_window, &child_process) == 0)	{
		printf("Errore nella creazione processo client\r\n");
		CloseHandle(pipe); // chiusura pipe
		fclose(file); // chiusura file
		return -1;
	}
	// attesa connessione processo client alla pipe
	if (ConnectNamedPipe(pipe, NULL) == 0)
	{
		printf("Errore connessione named-pipe\r\n");
		CloseHandle(pipe); // chiusura pipe
		fclose(file); // chiusura file
		return -1;
	}
	// lettura da file di blocchi di dati di 16 byte
	// e inserimento nella pipe
	while ((n = fread(buffer, 1, 16, file)) > 0)
		WriteFile(pipe, buffer, n, &i, NULL);
	fclose(file); // chiusura file
	CloseHandle(pipe); // chiusura pipe
					   // attesa visualizzazione risultato da parte del processo figlio
	Sleep(1000); // attesa 1s
	return 0;
}