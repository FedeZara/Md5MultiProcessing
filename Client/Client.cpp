// Client.cpp: definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include "global.h"
#include <openssl/md5.h>

void main(int argc, char *argv[])
{
	HANDLE *pipe;
	unsigned char buffer[16], fingerprint[16];
	int i, n;
	MD5_CTX context;
	// attesa connessione pipe da parte del processo server
	WaitNamedPipe("\\\\.\\pipe\\my_pipe", NMPWAIT_WAIT_FOREVER);
	// connessione alla pipe con accesso in lettura
	pipe = CreateFile("\\\\.\\pipe\\my_pipe", GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		printf("Errore apertura named-pipe\r\n");
		return;
	}
	// inizializzazione MD5
	MD5Init(&context);
	// estrazione dalla pipe dei dati a blocchi di 16 byte
	// e aggiornamento MD5
	while (ReadFile(pipe, buffer, 16, &n, NULL) != 0)
		MD5Update(&context, buffer, n);
	CloseHandle(pipe); // chiusura pipe
					   // visualizzazione in esadecimale del fingerprint
	MD5Final(fingerprint, &context);
	for (i = 0; i<16; i++)
		printf("%02X", fingerprint[i]);
	printf("\r\n");
	return;
}
