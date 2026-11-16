#include "kernel/types.h"
#include "user/user.h"


void sieve(int read_fd) {
	int p[2];
	int pid = -1;
	int prime;
	int num;
	int r = read(read_fd, &prime, sizeof(prime));

	if (r == 0) {
		close(read_fd);
		return;
	}

	if (r != sizeof(prime)) {
		fprintf(2, "read failed\n");
		exit(1);
	}

	fprintf(1, "prime %d\n", prime);

	while ((r = read(read_fd, &num, sizeof(num)))== sizeof(num)){
		if (num % prime == 0) {
			continue;
		}
		
		if (pid == -1) {
			if (pipe(p) < 0) {
				fprintf(2, "pipe failed");
				exit(1);
			}

			pid = fork();

			if (pid < 0) {
				fprintf(2, "fork failed\n");
				exit(2);
			}

			if (pid == 0 ) {
				close(read_fd);
				close(p[1]);
				sieve(p[0]);
				exit(0);
			}
			close(p[0]);
		}
		write(p[1], &num, sizeof(num));
	}

	if (r != 0) {
		fprintf(2, "read failed\n");
		exit(1);
	}
		
	close(read_fd);
		
	if (pid != -1) {
		close(p[1]);
		wait(0);
	}

	exit(0);
}

int
main () {
	int p[2];
	if (pipe(p) < 0) {
		fprintf(2, "pipe failed\n");
		exit(1);
	}

	int pid = fork();

	if (pid < 0) {
		fprintf(2, "fork failed");
		exit(1);
	}

	if (pid == 0) {
		close(p[1]);
		sieve(p[0]);
		exit(0);
	}

	close(p[0]);
	for (int n = 2; n <= 35; ++n) {
		write(p[1], &n, sizeof(n));
	}
	close(p[1]);
	wait(0);

	exit(0);
}
