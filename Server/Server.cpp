// Server.cpp: definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
// struttura per informazioni sulla finestra di esecuzione
STARTUPINFO startup_window;
// struttura per informazioni sul processo e il thread principale
PROCESS_INFORMATION child_process;
void main(int argc, char *argv[])
{
	FILE *file;
	unsigned char buffer[16];
	int n;
	DWORD i;
	HANDLE pipe;
	if (argc != 2)
	{
		printf("Uso: %s file\r\n", argv[0]);
		return;
	}
	// creazione pipe
	pipe = CreateNamedPipe(LPTSTR("\\\\.\\pipe\\my_pipe"),
		PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 1024, 0, 0, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		printf("Errore creazione named-pipe\r\n");
		return;
	}
	// apertura in lettura file
	file = fopen(argv[1], "rb");
	if (file == NULL)
	{
		printf("Errore apertura file \"%s\"\r\n", argv[1]);
		CloseHandle(pipe); // chiusura pipe
		return;
	}
	// creazione processo client
	if (CreateProcess(LPCTSTR("..\..\Client\Debug\Client.exe"), NULL, NULL, NULL, TRUE, 0, NULL,
		NULL, &startup_window, &child_process) == 0)	{
		printf("Errore nella creazione processo client\r\n");
	CloseHandle(pipe); // chiusura pipe
	fclose(file); // chiusura file
	return;
	}
	// attesa connessione processo client alla pipe
	if (ConnectNamedPipe(pipe, NULL) == 0)
	{
		printf("Errore connessione named-pipe\r\n");
		CloseHandle(pipe); // chiusura pipe
		fclose(file); // chiusura file
		return;
	}
	// lettura da file di blocchi di dati di 16 byte
	// e inserimento nella pipe
	while ((n = fread(buffer, 1, 16, file)) > 0)
		WriteFile(pipe, buffer, n, &i, NULL);
	fclose(file); // chiusura file
	CloseHandle(pipe); // chiusura pipe
					   // attesa visualizzazione risultato da parte del processo figlio
	Sleep(1000); // attesa 1s
	return;
}