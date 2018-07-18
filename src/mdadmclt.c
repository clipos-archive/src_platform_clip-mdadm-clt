// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2007-2018 ANSSI. All Rights Reserved.

/**
 * mdadmclt.c
 *
 * @brief mdadmclt connects to /var/run/mdadm and sends a command request to a mdadm server. Command can be sdb or sdc.
 * @see mdadm
 *
 **/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

static int usage (const char *s);

static int usage(const char *s)
{
	fprintf(stderr, "Usage: %s action\n\
where action can have the following values :\n\
\tsdc\tformats sdc and synchronizes with sda.\n\
\tsdb\tformats sdb and synchronizes with sda.\n\
WARNING: sdb should be called when PC has been rebooted \
and then new disk inserted\n", s);
	return -1;
}

int main(int argc, const char *argv[])
{
	int s;
	char c;
	struct sockaddr_un sau;
	char command = 0;

	if (argc != 2)
		exit(usage(argv[0]));

	if (!strcmp (argv[1], "sdb"))
		command = 'B';
	else if (!strcmp (argv[1], "sdc"))
		command = 'C';
	else
		exit(usage(argv[0]));

	sau.sun_family = AF_UNIX;
	snprintf(sau.sun_path, sizeof(sau.sun_path), 
		"%s", MDADMSOCKET);

	s = socket(PF_UNIX, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		return -1;
	}
	
	if (connect(s, (struct sockaddr *)&sau, sizeof(struct sockaddr_un)) < 0) {
		perror("connect");
		close(s);
		return -1;
	}

	if (write(s,&command,1) != 1) {
		perror("write");
		close(s);
		return -1;
	}

	if (read(s, &c, 1) != 1) {
		perror("read");
		close(s);
		return -1;
	}

	close(s);
	if (c == 'Y') {
		printf("Action successfully completed\n");
		return EXIT_SUCCESS;
	} else {
		printf("Error during the execution\n");
		return EXIT_FAILURE;
	}
}
