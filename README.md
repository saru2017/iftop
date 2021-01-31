# README for iftop-cygwin

Forked for porting to cygwin.
This project is not completed.
I'will use libwpcap (WpdPack_4_1_2) as libpcap.


# コンパイルしているファイル

```
iftop_SOURCES = addr_hash.c edline.c hash.c iftop.c ns_hash.c \
                 options.c resolver.c screenfilter.c serv_hash.c \
                 sorted_list.c threadprof.c ui.c util.c \
		 addrs_ioctl.c addrs_dlpi.c dlcommon.c \
		 stringmap.c cfgfile.c vector.c
```

# ライブラリ

```
LIBS = -lpcap -lm  -lncursesw -lpthread
```



## オブジェクトファイル生成

[o] addr_hash.c
[o] edline.c
[o] hash.c
[o] iftop.c <pcap.h>が見つからないと怒られるので後回し
[o] ns_hash.c
[o] options.c
[o] resolver.c
[o] screenfilter.c
[o] serv_hash.c
[o] sorted_list.c
[o] threadprof.c
[o] ui.c
[o] util.c
[o] addrs_ioctl.c
[o] addrs_dlpi.c
[o] dlcommon.c
[o] stringmap.c
[o] cfgfile.c
[o] vector.c


## リンク

```
gcc -o iftop addr_hash.o edline.o hash.o iftop.o ns_hash.o options.o resolver.o screenfilter.o serv_hash.o sorted_list.o threadprof.o ui.o util.o addrs_ioctl.o addrs_dlpi.o dlcommon.o stringmap.o cfgfile.o vector.o
```

↓であと一息な感じ

```
 gcc -o iftop addr_hash.o edline.o hash.o iftop.o ns_hash.o options.o resolver.o screenfilter.o serv_hash.o sorted_list.o threadprof.o ui.o util.o addrs_ioctl.o addrs_dlpi.o dlcommon.o stringmap.o cfgfile.o vector.o -lncursesw -lm -lpthread
```


# 履歴

config/config.guessで止まる。
まずはLinuxとかと誤解させてconfigureを通るようにしたい。

Q. ac_buidとかのacってなんだ？！
acはautoconfの略だ。
これはautoconfで作られている。



