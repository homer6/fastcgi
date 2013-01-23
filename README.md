Install
-------

 sudo apt-get install libfcgi-dev spawn-fcgi nginx curl


Running the service
-------------------

 sudo mv /etc/nginx/nginx.conf /etc/nginx/old_nginx.conf.bk
 
 sudo ln -s `pwd`/nginx.con /etc/nginx/nginx.conf
 
 sudo /etc/init.d/nginx restart
 
 sudo /etc/init.d/

 ./make
 
 ./start

 (remove the -n switch from the start executable if you want it to run in the background)


Benchmarks/Testing
------------------

 curl --data "hello" http://localhost:81/

 ab -n 2000 -c 10 http://localhost:81/

 httperf --client=0/1 --server=localhost --port=81 --uri=/ --send-buffer=4096 --recv-buffer=16384 --num-conns=10 --num-calls=2000



Code from this article
----------------------

 http://chriswu.me/blog/writing-hello-world-in-fcgi-with-c-plus-plus/
