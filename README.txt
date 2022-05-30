hello

ini program socket simple HTTP client buat contoh belajar aja

gw bikin untuk kasih contoh buat Fernanda terkait error handling dan
short writes handling yang bener dalam TCP socket, kasus na HTTP client
simple, cuma hit `GET / HTTP/1.1` doang.

klo mo baca history message na, cek link ini

Link: https://t.me/GNUWeeb/609588
---

cara compile: 

  clang -Wall -Wextra -O3 http.c -o http

clang boleh diganti pake gcc

cara pake:

  ./http 127.0.0.1 8000

  127.0.0.1 itu address server na
  8000 itu port na, sesuaiin aja

---

License: GNU GPL v2

tq

-- Viro
