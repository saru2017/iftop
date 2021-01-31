gcc -c addr_hash.c -o addr_hash.o
gcc -c addrs_dlpi.c -o addrs_dlpi.o
gcc -c addrs_ioctl.c -o addrs_iotcl.o
gcc -c cfgfile.c -o cfgfile.o
gcc -c counter_hash.c -o counter_hash.o
gcc -c dlcommon.c -o dlcommon.o
gcc -c edline.c -o edline.o
gcc -c hash.c -o hash.o
gcc -c iftop.c -o iftop.o -I./include
gcc -c ns_hash.c -o ns_hash.o
gcc -c options.c -o options.o
gcc -c resolver.c -o resolver.o
gcc -c screenfilter.c -o screenfilter.o
gcc -c serv_hash.c -o serv_hash.o
gcc -c sorted_list.c -o sorted_list.o
gcc -c stringmap.c -o stringmap.o
gcc -c threadprof.c -o threadprof.o
gcc -c tui.c -o tui.o
gcc -c ui.c -o ui.o
gcc -c ui_common.c -o ui_common.o
gcc -c util.c -o util.o
gcc -c vector.c -o vector.o


gcc -o iftop addr_hash.o addrs_dlpi.o addrs_iotcl.o cfgfile.o counter_hash.o dlcommon.o edline.o hash.o iftop.o ns_hash.o options.o resolver.o screenfilter.o serv_hash.o sorted_list.o stringmap.o threadprof.o tui.o ui.o ui_common.o util.o vector.o Packet.lib wpcap.lib -lm -lncursesw -lpthread 

