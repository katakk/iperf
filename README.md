##iperf 2.0.5 my local copy
速度測定用のツール。`iperf 2.0.5` のローカルコピー  

###コンパイル

    $ make  
    $ cp ./src/iperf /bin/  

###使い方

    $ iperf -c 10.0.0.1 -w256k -i1 -t60 -----> iperf -s -w256k -i1   

##TODO

###simplize

    automakeとか不要ファイル削除  

###rewrite by c

###ToS


 