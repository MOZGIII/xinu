# Xinu makefile
DIRS = kernel chardevs diskdevs miscdevs shell utils serial net netapp \
        arp dgram icmp ip snmp udp tcp tcpd tcl

LIBS = xinu.a network.a serial.a tcl.a

all:	$(DIRS)

lib:
	@make ARG=lib

objs:
	@make ARG=objs

.PHONY: $(DIRS) clean

clean:
	cd bin && rm -rf $(LIBS)
	@make -k ARG=clean

$(DIRS):

	cd $@ && make $(ARG)