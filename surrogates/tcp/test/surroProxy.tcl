#!/usr/bin/tclsh

#===============================================
# proxy to allow simulation
# of sluggish network scenerios
#===============================================

set this "proxy"

package require Fctclx

set MASK_MISC	1

#================================================
#	proxyServer - entry point
#================================================
proc proxyServer { port } {
global mysock

set mysock(main) [socket -server proxyAccept $port]

};# end proxyServer

#================================================
#	proxyAccept - entry point
#================================================
proc proxyAccept { sock addr port } {
global mysock

#puts "Accept $sock from $addr port $port"

set mysock(addr,$sock) [list $addr $port]

fileevent $sock readable [list proxyDo $sock $mysock(client) ]
fileevent $mysock(client) readable [list proxyDo $mysock(client) $sock ]
fconfigure $sock -translation binary
fconfigure $mysock(client) -translation binary

};# end proxyAccept


#================================================
#	proxyDo - entry point
#================================================
proc proxyDo { insock outsock } {
global mysock

set myhdr [read $insock 12 ]
binary scan $myhdr i1i1i1 token nbytes ybytes

#puts stdout [format "proxyDo token=%d nbytes=%d ybytes=%d len=%d"\
# $token $nbytes $ybytes [string length $myhdr]]

if { $nbytes > 0 } {
	set mybuf [read $insock $nbytes ]
	puts -nonewline $outsock $myhdr$mybuf
	flush $outsock
	}

if { $token == 5 } {
	close $insock
	close $outsock
	}

};# end proxyDo

#================================================
#	main - entry point
#================================================
set fn main
set logMask 0xff
set loggerName ""
set myName  PROXY

set state flag
foreach arg $argv {
	switch -- $state {
		flag {
			switch -glob -- $arg {
				-N	{ set state name }
				-p	{ set state port }
				-m	{ set state mask }
				-l	{ set state logger }
				default { error "unknown flag $arg"}
			}
		}
		
		name {
			set myName $arg
			set state flag
		}

		port {
			set  myport $arg
			set state flag
		}

		mask {
			set logMask $arg
			set state flag
		}

		logger {
			set loggerName $arg
			set state flag
		}
	};# end switch state
};# end foreach

set myslot [name_attach $myName 2048]
catch {puts stdout [format "myName=<%s> myslot=%d" $myName $myslot]}

if { $loggerName == "" } {
	set logger -1
} else {
	set logger [name_locate "LOGGER"]
}

set myFifo [ format "%s/%s" $env(FIFO_PATH) $myslot ]
set recvid [ open $myFifo {RDWR}]

#logit $logger $this $fn $MASK_MISC $logMask [format "myName=<%s> myslot=%s" $myName $myslot]
#logit $logger $this $fn $MASK_MISC $logMask [format "loggerName=<%s> id=%d" $loggerName $logger]
#logit $logger $this $fn $MASK_MISC $logMask [format "logMask=%X" $logMask]
#logit $logger $this $fn $MASK_MISC $logMask [format "myFifo=<%s> fd=<%s>" $myFifo $recvid]

set mysock(client) [ socket 127.0.0.1 8001 ]

proxyServer $myport

vwait forever

name_detach

puts stdout {done}

exit
