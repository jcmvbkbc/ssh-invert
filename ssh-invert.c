#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	char c[1024 * 1024];
	fd_set readfds;

	FD_ZERO(&readfds);
	for (;;) {
		ssize_t rd = read(STDIN_FILENO, c, sizeof(c));
		if (rd < 0) {
			FD_SET(STDIN_FILENO, &readfds);
			select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
		} else if (rd == 0) {
			return 0;
		} else {
			int i;
			for (i = 0; i < rd; ++i)
				c[i] ^= 0x5a;
			for (i = 0; i < rd; ) {
				ssize_t wr = write(STDOUT_FILENO, c + i, rd - i);
				if (wr == 0)
					return 0;
				else if (wr > 0)
					i += wr;
			}
		}
	}
	return 0;
}
