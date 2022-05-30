// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Alviro Iskandar Setiawan <alviro.iskandar@gnuweeb.org>
 *
 * ini program socket simple HTTP client buat contoh belajar aja
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>

/*
 * Cara jalanin program kek begini
 *
 * ./prog [target server] [target port]
 *
 * Misal:
 *
 *   ./http 127.0.0.1 8000
 *
 */
int main(int argc, char *argv[])
{

	static const char http_payload[] = "GET / HTTP/1.1\r\n\r\n";
	struct sockaddr_in addr;
	char buf[4096];
	const char *p;
	ssize_t ret;
	size_t len;
	int tcp_fd;
	int err;

	/*
	 * Kita harus pastiin programnya dieksekusi dengan bener, ni
	 * program butuh 3 argument
	 *
	 * argv[0] = ./http
	 * argv[1] = [target server addr]
	 * argv[2] = [target server port]
	 */
	if (argc != 3) {
		printf("Invalid argument!\n");
		printf("Usage: %s [server addr] [server port]\n", argv[0]);
		printf("Example: %s 127.0.0.1 8000\n", argv[0]);
		return EINVAL;
	}


	/*
	 * ni kita bikin TCP socket fd dulu
	 *
	 * AF_INET maksud na pake ipv4
	 * SOCK_STREAM itu TCP
	 */
	tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_fd < 0) {
		err = errno;
		perror("socket");
		return err;
	}

	/*
	 * siapin target HTTP server na
	 *
	 * terdiri dari:
	 * - sin_family (jenis IP na, AF_INET tu IPv4)
	 * - sin_port to port na
	 * - sin_addr tu address na
	 *
	 */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t)atoi(argv[2]));
	addr.sin_addr.s_addr = inet_addr(argv[1]);


	/*
	 * klo udah, connect ke HTTP server tujuan
	 */
	err = connect(tcp_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (err) {
		err = errno;
		perror("connect");
		close(tcp_fd); /* jan lupa close tcp_fd, biar ga fd leak */
		return err;
	}

	/*
	 * kita kirim payload na ke server
	 */
	p = http_payload;
	len = strlen(http_payload);


write_again:
	ret = write(tcp_fd, p, len);
	if (ret <= 0) {

		if (ret == 0) {
			/*
			 * oggghh...
			 */
			puts("server down while we're sending data");
			close(tcp_fd);
			return ENETDOWN;
		}

		err = errno;

		/*
		 * klo kita dpt EINTR, berarti write na
		 * kena interrupt, bisa diulang lagi
		 */
		if (err == EINTR)
			goto write_again;

		/*
		 * gagal write, error.
		 */
		perror("write");
		close(tcp_fd);
		return err;
	}

	len -= (size_t)ret;
	if (len > 0) {
		/*
		 * kita dpt short write disini, write lagi sisa na
		 */
		p += (size_t)ret;
		goto write_again;
	}


	/*
	 * oc, kita uda berhasil kirim http request na, sekarang
	 * kita baca http response
	 */
read_again:
	ret = read(tcp_fd, buf, sizeof(buf) - 1);
	if (ret < 0) {
		ret = errno;
		perror("read");
		close(tcp_fd);
		return ret;
	}

	if (ret == 0) {
		/*
		 * uda EOF, selesai ni, tutup program na, success
		 */
		close(tcp_fd);
		return 0;
	}

	/* set null terminating biar printf na safe */
	buf[ret] = '\0';
	printf("%s", buf);
	goto read_again;
}

