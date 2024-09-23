include .env

netdump:
	gcc -g -w -o netdump analizer.c main.c buffer.c command_handler.c status_handler.c utils/raw_array.c utils/string_utils.c -lpcap
clean:
	-rm netdump
run:
	sudo ./netdump
debug:
	-rm netdump
	gcc -g -w -o netdump analizer.c main.c buffer.c command_handler.c status_handler.c utils/raw_array.c utils/string_utils.c -lpcap
	clear
	sudo ./netdump
install:
	mkdir -p $(DESTDIR)/usr/bin
	install -m 0755 netdump $(DESTDIR)/usr/bin/netdump
