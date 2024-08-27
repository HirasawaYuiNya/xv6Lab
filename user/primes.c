#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/param.h"

int lpipe_first_data(int lpipe[2], int *dst)
{
  if (read(lpipe[0], dst, sizeof(int)) == sizeof(int)) {
    printf("prime %d\n", *dst);
    return 0;
  }
  return -1;
}

void traversal(int lpipe[2])
{
	close(lpipe[1]);
	int first;

	if (lpipe_first_data(lpipe, &first) == 0) {
		int p[2];
		pipe(p);
		
		int data;

		while (read(lpipe[0], &data, sizeof(int)) == sizeof(int)) {
			if (data % first)
				write(p[1], &data, sizeof(int));
		}
		close(lpipe[0]);
		close(p[1]);
		
		if (fork() == 0)
			traversal(p);
		else {
			close(p[0]);
			wait(0);
		}
	}
	exit(0);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Parameter missing\n");
		exit(0);
	}

	int nums = atoi(argv[1]);

	int p[2];
	pipe(p);

	for (int i = 2; i <= nums; i++) {
		write(p[1], &i, sizeof(int));
	}

	if (fork() == 0) {
		traversal(p);
	} else {
		close(p[1]);
		close(p[0]);
		wait(0);
	}
	exit(0);
}
