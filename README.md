http://chriswu.me/blog/writing-hello-world-in-fcgi-with-c-plus-plus/

sudo apt-get install libfcgi-dev spawn-fcgi nginx curl

g++ -Wall main_v1.cpp -lfcgi++ -lfcgi -o hello_world

spawn-fcgi -p 8000 -n hello_world



ab -n 2000 -c 10 http://localhost:81/
httperf --client=0/1 --server=localhost --port=81 --uri=/ --send-buffer=4096 --recv-buffer=16384 --num-conns=10 --num-calls=2000