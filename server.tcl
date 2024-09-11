#!/bin/tclsh

load build/debug/libhttp_parser.so



proc handle {chan addr port} {

    set result [parse_request -channel $chan]


    puts "------------------------"
    puts "result $result"
    puts "------------------------"

    puts "::> [dict get $result method]"
    puts "::> [dict get $result path]"
    puts "::> [dict get $result body]"
    puts "::> [dict size [dict get $result headers]]"
    puts "::> .."


    puts $chan "HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/plain\n"
    if {[dict get $result path] == "/echo"} {
        puts $chan [dict get $result body]
    } else {
        #while {[gets $chan] ne ""} {}
        puts $chan "Hello World!"
    }
    close $chan

}

proc accept {socket addr port} {
  chan configure $socket -blocking 0 -buffering line
  chan event $socket readable [list handle $socket $addr $port]
}


socket -server accept 5151
vwait forever
