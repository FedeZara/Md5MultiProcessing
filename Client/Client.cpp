// Client.cpp: definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <openssl/md5.h>

int main(int argc, char *argv[])
{
	HANDLE pipe;
	unsigned char buffer[16], fingerprint[16];
	int i;
	DWORD n;
	MD5_CTX context;
	// attesa connessione pipe da parte del processo server
	WaitNamedPipe(TEXT("\\\\.\\pipe\\my_pipe"), NMPWAIT_WAIT_FOREVER);
	// connessione alla pipe con accesso in lettura
	pipe = CreateFile(TEXT("\\\\.\\pipe\\my_pipe"), GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// controllo se ci sono stati errori nella connessione alla pipe
	if (pipe == INVALID_HANDLE_VALUE)
	{
		printf("Errore apertura named-pipe\r\n");
		return -1;
	}
	// inizializzazione MD5
	MD5_Init(&context);
	// estrazione dalla pipe dei dati a blocchi di 16 byte
	// e aggiornamento MD5
	while (ReadFile(pipe, buffer, 16, &n, NULL) != 0)
		MD5_Update(&context, buffer, n);
	CloseHandle(pipe); // chiusura pipe
					   // visualizzazione in esadecimale del fingerprint
	MD5_Final(fingerprint, &context);
	// ciclio di stampa
	for (i = 0; i<16; i++)
		printf("%02X", fingerprint[i]);
	printf("\r\n");
	return 0;
}
