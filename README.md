#iperf 2.0.5
##那月ちゃんセーフティモード

速度測定用のツール。`iperf 2.0.5` のローカルコピー   

####simplizeした不要ファイル削除

オリジナル欲しい人は`remotes/origin/original` きゅん   
コンパイルは `$ make  ` インストールは ` $ cp ./src/iperf /bin/  ` きゅん    

####VC++2005でビルド通したきゅん

    iperf.exe

####使い方

サーバ(送信側) `iperf -c 10.0.0.1 -w256k -i1 -t60`   と  
クライアント(受信側) `iperf -s -w256k -i1` でデーターやりとりして速度をみるきゅん   

#### パッチあて状況

https://github.com/katakk/iperf/issues/3
*`27`まで完了  http://sourceforge.net/p/iperf/patches/   
*`git checkout 1e78b56b434e4c979ee0564725be131f44047e63` してパッチ当てるきゅん   
* `23`    Android build scripts        `2012-06-08`  バイナリは一緒に入れといたきゅん     
 



