##iperf 2.0.5 my local copy
速度測定用のツール。`iperf 2.0.5` のローカルコピー  

###コンパイル

    $ make  

###インストール

    $ cp ./src/iperf /bin/  

###使い方

送信側 `iperf -c 10.0.0.1 -w256k -i1 -t60`   
受信側 `iperf -s -w256k -i1`   

##TOOD
###simplize

* automakeとか不要ファイル削除  

* 消しまっくたのでオリジナル欲しい人は`remotes/origin/original`   

### パッチあて状況

http://sourceforge.net/p/iperf/patches/
    $ git checkout 1e78b56b434e4c979ee0564725be131f44047e63   
してパッチ当てる

####not yet patchd

* `27` Patch to allow burst traffic in UDP mode                          2013-04-17  2013-04-17  
* `24` Option --priority allows user to set SO_PRIORITY                  2012-09-19  2012-09-19  
* `20` Eliminate some compiler warnings                                  2011-12-08  2011-12-08  
* `19` Update usage and comments to reflect new default buf size         2011-12-08  2011-12-08  
* `18` iperf TCP client cannot detect write errors                       2011-12-08  2011-12-08  

####patchd

* `26` Support specifying interface for Multicast                        2013-01-11  2013-01-11  
* `25` IPv6 traffic class support                                        2012-10-02  2012-10-02  
* `22` Fix iperf to work with -B -P (updated 2.0.5)                      2012-01-29  2012-01-29  
* `17` Broken delay_loop, can't handle 1s, breaks -b for low values      2011-10-14  2011-10-14  
* `15` A new compat/​delay.cpp                                           2010-07-17  2010-07-17  
* `14` Reduce reporter CPU usage.                                        2010-07-17  2010-07-17  
* `13` Iperf-2.0.4 port to win32 and visual studio 2003                  2009-12-23  2009-12-23  

* `10` log individual UDP packet loss back to the client                 2009-01-28  2009-01-28  
* ` 8` poisson traffic and measure delay patch                           2008-11-05  2008-11-05  
* ` 9` Fix iperf to work with -B -P                                      2009-01-03  2009-01-03  
* ` 5` Fix for iperf -d extra connections (bug# 1903811)                 2008-04-25  2008-04-25  

以下はバイナリ提供とか範囲外 バイナリは一緒に入れといた  

* `16`    Jperf usability fixes                                          2010-10-03  2010-10-03  
* `21`    Windows binary 2.0.5                                           2012-01-06  2012-01-06  
* `23`    Android build scripts                                          2012-06-08  2013-07-10  

##まだやってない けど入れたい機能とか

####カーネルモード動作   

####rewrite by c   

####ToS  (win)  

