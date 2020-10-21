#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	char c[1024 * 1024];
	fd_set readfds;
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);

	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	FD_ZERO(&readfds);
	for (;;) {
		ssize_t rd = read(STDIN_FILENO, c, sizeof(c));
		if (rd < 0) {
			if (errno == EAGAIN ||
			    errno == EWOULDBLOCK) {
				FD_SET(STDIN_FILENO, &readfds);
				select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
			} else {
				return -1;
			}
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
				else
					return 1;
			}
		}
	}
	return 0;
}
