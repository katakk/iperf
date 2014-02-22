##iperf2.0.5 p27 那月ちゃんセーフティモード

* 速度測定用のツール。`iperf 2.0.5` のローカルコピー   
* パッチ当て済 `27`まで完了 https://github.com/katakk/iperf/issues/3  
* コンパイルは `$ make  ` インストールは ` $ cp ./src/iperf  /usr/local/bin/iperf ` きゅん    
* `BSD`ライセンス
* 不要ファイル削除
* VC++2005でビルド通したきゅん
* プロジェ`iperf.vcproj`, `iperf.sln` バイナリ `iperf.exe` 

####使い方

* サーバ(送信側) `iperf -c 10.0.0.1 -w256k -i1 -t60` 
* クライアント(受信側) `iperf -s -w256k -i1`
* でデーターやりとりして速度をみるきゅん   

 



