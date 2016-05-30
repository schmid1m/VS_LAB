package require Expect
package require udp

#######################################
# Configuration Setup

# server IDs and IP addresses
#	00	"141.47.69.0" 
#	01	"141.47.69.1" 
#	02	"141.47.69.2" 
#	03	"141.47.69.3" 
#	04	"141.47.69.4" 
#	05	"141.47.69.5" 
#	06	"141.47.69.6" 
#	07	"141.47.69.7" 
#	08	"141.47.69.8" 
#	09	"141.47.69.9" 
#	10	"141.47.69.10" 
#	11	"141.47.69.11" 
#	12	"141.47.69.12" 
#	13	"141.47.69.13" 
#	14	"141.47.69.14" 
#	15	"141.47.69.15" 
#	16	"141.47.69.16" 
#	17	"141.47.69.17" 
#	18	"141.47.69.18" 
array set servers {
	04	"141.47.69.4" 
	10	"141.47.69.10" 
	16	"141.47.69.16" 
}

# client IDs and IP addresses
# (IP addresses are not used so far, but they need to be configured anyway;
# clients are assumed to run on the local machine)
array set clients {
	0	"141.47.75.250" 
	1	"141.47.75.250" 
}

# broadcast IP address
set broadcast_ip "141.47.95.255"

# server port (same value for all servers!)
set server_port 11111

# machine types {arm|x64}
set server_type arm
set client_type x64

# login credentials
set server_login root
set server_pwd   vslab

# client/server executable names
set server_exe vslabd
set client_exe vslabc

# server working directory
set server_workdir /tmp

# client working directory prefix;
# the real directory path is composed of the path defined here and the client ID, e.g. /tmp/vslabc0
#set client_workdir /tmp/vslabc
set client_workdir /tmp/vslabc

# default client priority
set default_client_priority 0

# uncoded text file size in bytes
set textfile_size 20000

# file names for uncoded, coded and descrambled text
set textfile_uncoded "text.txt"
set textfile_coded "text.bin"
set textfile_descrambled "text.out"

# data structure is
#    char priority;                      
#    char version;                       
#    char mode;                         
#    char type/func;                      
#    short int length;                   
#    short int reserved;   
#    short int CID
#    short int reserved
#    short int wordcount              
# Adapt procedures ReadResponse and RequestServerStats if data structure is changed!

set server_status_request_priority	0
set server_status_request_version	14
set server_status_request_mode	1
set server_status_request_type	99
set server_status_request_length 0
set server_status_request_reserved 0


set server_status_response_type	100
set server_status_response_mode	2
set server_status_response_version	14


#######################################
# Internal definitions

# initial debug mode
set debug 0

# timeout and local echo settings for expect
set timeout 2
log_user 0

# timeout for ping check in seconds
set ping_wait_secs 1

# status check interval in milliseconds
set status_check_interval 100

# server prompt (regexp)
set server_prompt ".+# "

# test script working directory
set testscript_workdir [pwd]

# textscrambler
set textscrambler_cmd "$testscript_workdir/textscrambler"

# full program names for dtach
set dtach_cmd_server "dtach_$server_type"
set dtach_cmd_client "$testscript_workdir/dtach_$client_type"

# socket file name for dtach
set dtach_sockfile "vslab_dtach_sockfile"

# ps command (process name must be listed and process id must be in the first column)
switch $server_type {
	"arm"	{set ps_cmd_server "ps"}
	"x64"	{set ps_cmd_server "ps -e"}
} 

# max number of unresponded requests before extended server checks start
set no_server_response_max_count 5

# bitmaps generated with "bitmap" command
# set status_bitmap {
# 	#define status_width 15
# 	#define status_height 16
# 	static unsigned char status_bits[] = {
# 	   0xf0, 0x07, 0xf8, 0x0f, 0xfc, 0x1f, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0x1c,
# 	   0x00, 0x0e, 0x00, 0x07, 0x80, 0x03, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01,
# 	   0x00, 0x00, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01};
# }
set onoff_bitmap {
	#define onoff_width 15
	#define onoff_height 16
	static unsigned char onoff_bits[] = {
	   0xc0, 0x01, 0xc0, 0x01, 0xd8, 0x0d, 0xdc, 0x1d, 0xde, 0x3d, 0xcf, 0x79,
	   0xc7, 0x71, 0xc7, 0x71, 0xc7, 0x71, 0xc7, 0x71, 0x07, 0x70, 0x0f, 0x78,
	   0x1e, 0x3c, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07};
}
# set terminal_bitmap {
# 	#define terminal_width 15
# 	#define terminal_height 16
# 	static unsigned char terminal_bits[] = {
# 	0x00, 0x00, 0xff, 0x7f, 0x01, 0x40, 0xfd, 0x5f, 0x05, 0x50, 0x05, 0x50,
# 	0x05, 0x50, 0x05, 0x50, 0x05, 0x50, 0x05, 0x50, 0x05, 0x50, 0xfd, 0x5f,
# 	0x01, 0x40, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00};
# }


#######################################
# Initialization

# fonts and images
set fnt(big) [font create -family Courier -size 18 -weight bold]
set fnt(small) [font create -family Courier -size 8 -weight normal]
set img(onoff) [image create bitmap -data $onoff_bitmap]
# set img(status) [image create bitmap -data $status_bitmap]
# set img(terminal) [image create bitmap -data $terminal_bitmap]

# sort and count server/client IDs
foreach server_id [array names servers] {
	lappend server_ids $server_id
}
set server_ids [lsort $server_ids]
set nservers [array size servers]

foreach client_id [array names clients] {
	lappend client_ids $client_id
}
set client_ids [lsort $client_ids]
set nclients [array size clients]

# initialize storage for client ids
foreach server_id $server_ids {
	set ::server_status($server_id,cid) -2
}

# initialize storage for scramble counts
foreach server_id $server_ids {
	foreach client_id $client_ids {
		set ::scramble_count($server_id,$client_id) {}
	}
}

# initialize counters for unresponded server status requests
foreach server_id $server_ids {
	set no_server_response_count($server_id) 0
}

# initialize client priorities
foreach client_id [array names clients] {
	set client_priority($client_id) $default_client_priority
}

# initialize size of uncoded textfile (real value will be set later)
set textfile_realsize $textfile_size

#######################################
# Server networking, checks and controls

proc OpenServerChannels {} {
	# opens sockets for UDP communication with servers
	foreach sid $::server_ids {
		set ::schannel($sid) [udp_open]
		fconfigure $::schannel($sid) -remote [list $::servers($sid) $::server_port]
		fconfigure $::schannel($sid) -buffering none -translation binary
		fileevent $::schannel($sid) readable [list ReadResponse $::schannel($sid) $sid]
	}
}

proc CloseServerChannels {} {
	# closes server sockets
	foreach sid $::server_ids {
		close $::schannel($sid)
	}
}

proc ReadResponse {channel id} {
	# reads and processes server response
	set response [read $channel]
	binary scan $response ccccSSSSI priority version mode type length reserved1 cid reserved2 scrcount
	if {$::debug} {UpdateStatusLog "Response from $id: t=$type m=$mode v=$version p=$priority c=$cid d=$scrcount"}
	if {(	($type == $::server_status_response_type) \
		&&	($mode == $::server_status_response_mode) \
		&&	($version == $::server_status_response_version)	) } {
		set ::server_status($id,type) $type
		set ::server_status($id,mode) $mode
		set ::server_status($id,version) $version
		set ::server_status($id,priority) $priority
		set ::server_status($id,fid) $cid
		if {($::server_status($id,cid) != $cid) && ($::server_status($id,cid) != -2)} {
			# client id has changed, old client id is valid
			if {$cid == -1} {
				# server now unlocked -> replace last entry for previously locked client in counter list
				set ::scramble_count($id,$::server_status($id,cid)) [lreplace $::scramble_count($id,$::server_status($id,cid)) end end $scrcount]
			} else {
				# server changed lock from one to another client -> append entry to counter list of new client id
				lappend ::scramble_count($id,$cid) $scrcount
			}
		} elseif {$cid >= 0} {
			# client id has not changed, but client is locked-> replace last counter entry in list
			set ::scramble_count($id,$cid) [lreplace $::scramble_count($id,$cid) end end $scrcount]
		}
		set ::server_status($id,cid) $cid
		
		# clear no response count for server
		set ::no_server_response_count($id) 0
	}
}

proc RequestServerStats {sid} {
	# sends status request to server
	puts -nonewline $::schannel($sid) [binary format c4S2 [list $::server_status_request_priority $::server_status_request_version $::server_status_request_mode $::server_status_request_type] [list $::server_status_request_length $::server_status_request_reserved]]
	# increment no response count for server
	incr ::no_server_response_count($sid)
	
	if {$::debug} {UpdateStatusLog "Status request to $sid: t=$::server_status_request_type m=$::server_status_request_mode v=$::server_status_request_version p=$::server_status_request_priority"}
}

proc CheckServerHost {sid} {
	# pings server host to see if it's alive
  	catch {exec ping -c 1 -w $::ping_wait_secs $::servers($sid)} ping_result
  	return [regexp "1 received" $ping_result]
}

proc CheckServerExe {sid} {
	# checks for presence of server executable
	set exe_available 0
	spawn ssh -o "StrictHostKeyChecking no" -l $::server_login $::servers($sid)
	expect "password: "
	exp_send "$::server_pwd\n"
	expect -re $::server_prompt
	exp_send "echo $::server_workdir/*\n"
	expect -re $::server_prompt
	if [regexp "Welcome" $expect_out(buffer)] {
		exp_send "echo $::server_workdir/*\n"
		expect -re $::server_prompt
	}
	if [regexp "$::server_workdir/$::server_exe" $expect_out(buffer)] {
		set exe_available 1
	} else {
		set exe_available 0
	}
	exp_send "exit\n"

	close -i $spawn_id
	wait -i $spawn_id

	return $exe_available
}

proc CheckServerProcess {sid} {
	# checks for running server process
	set server_running 0
	spawn ssh -o "StrictHostKeyChecking no" -l $::server_login $::servers($sid)
	expect "password: "
	exp_send "$::server_pwd\n"
	expect -re $::server_prompt
	exp_send "$::ps_cmd_server\n"
	expect -re $::server_prompt
	if [regexp "Welcome" $expect_out(buffer)] {
		exp_send "$::ps_cmd_server\n"
		expect -re $::server_prompt
	}
	if [regexp {$::server_exe[[:space:]]} $expect_out(buffer)] {
		set server_running 1
	} else {
		set server_running 0
	}
	# kill all running ps processes
	exp_send "for p in \$($::ps_cmd_server | grep '$::ps_cmd_server' | grep -v grep | awk '{print \$1}'); do kill -9 \$p; done\n"
	expect -re $::server_prompt
	exp_send "exit\n"

	close -i $spawn_id
	wait -i $spawn_id

	return $server_running
}

proc CheckServer {sid} {
	# check server status
	if {$::no_server_response_count($sid) <= $::no_server_response_max_count} {
		# $::no_server_response_max_count or less open requests -> assuming a running server
		RequestServerStats $sid
		set ::server_status($sid,process) "running"
	} else {
		# more than $::no_server_response_max_count open requests -> what's the problem?
		#	- server host online?
		if [CheckServerHost $sid] {
			#	- server executable available?
			if [CheckServerExe $sid] {
				#	- server process running?
				if [CheckServerProcess $sid] {
					set ::server_status($sid,process) "running"
					RequestServerStats $sid
				} else {
					set ::server_status($sid,process) "not_running"
				}
			} else {
				set ::server_status($sid,process) "exe_not_available"
			}
		} else {
			set ::server_status($sid,process) "dead"
		}
	}

	if {$::debug} {UpdateStatusLog "Server ID $sid status: $::server_status($sid,process) : $::no_server_response_count($sid)"}
}

proc StartStopServer {sid} {
	# starts or stops server
	if {$::server_status($sid,process) == "not_running"} {
		if {$::debug} {UpdateStatusLog "Starting server ID $sid..."}

		# clear scramble counts
		foreach client_id $::client_ids {
			set ::scramble_count($sid,$client_id) {}
		}

		# remove SSH key from known_hosts to avoid errors due to newly generated SSH keys on Zedboard startup
		catch {exec ssh-keygen -R $::servers($sid)} ssh_keygen_result
		if {$::debug} {UpdateStatusLog "SSH key of server ID $sid removed."}

		# copy dtach to server
		spawn scp -o "StrictHostKeyChecking no" $::testscript_workdir/$::dtach_cmd_server $::server_login@$::servers($sid):$::server_workdir
		expect "password: "
		exp_send "$::server_pwd\n"
		expect "100%"
		if {$::debug} {UpdateStatusLog "$::testscript_workdir/$::dtach_cmd_server copied to $::server_login@$::servers($sid):$::server_workdir : $expect_out(buffer)" }
		close -i $spawn_id
		wait -i $spawn_id
		
		# start server
		spawn ssh -o "StrictHostKeyChecking no" -l $::server_login $::servers($sid)
		expect "password: "
		exp_send "$::server_pwd\n"
		expect -re $::server_prompt
		exp_send "rm $::server_workdir/$::dtach_sockfile\n"
		expect -re $::server_prompt
		exp_send "rm $::server_workdir/$::dtach_sockfile\n"
		expect -re $::server_prompt
		exp_send "$::server_workdir/$::dtach_cmd_server -n $::server_workdir/$::dtach_sockfile $::server_workdir/$::server_exe 1\n"
		expect -re $::server_prompt
		exp_send "exit\n"
		close -i $spawn_id
		wait -i $spawn_id
		
		if {$::debug} {UpdateStatusLog "Server ID $sid started.: $::server_workdir/$::dtach_cmd_server -n $::server_workdir/$::dtach_sockfile $::server_workdir/$::server_exe"}

		set ::no_server_response_count($sid) 0

		
	} elseif {$::server_status($sid,process) == "running"} {
		if {$::debug} {UpdateStatusLog "Stopping server ID $sid..."}

		# kill server process
		spawn ssh -o "StrictHostKeyChecking no" -l $::server_login $::servers($sid)
		expect "password: "
		exp_send "$::server_pwd\n"
		expect -re $::server_prompt
		exp_send "kill -9 \$($::ps_cmd_server |grep $::dtach_cmd_server | grep -v grep | awk '{print \$1}')\n"
		expect -re $::server_prompt
		exp_send "rm $::server_workdir/$::dtach_sockfile\n"
		expect -re $::server_prompt
		exp_send "rm $::server_workdir/$::dtach_sockfile\n"
		expect -re $::server_prompt
		exp_send "exit\n"
		close -i $spawn_id
		wait -i $spawn_id
		if {$::debug} {UpdateStatusLog "Server ID $sid stopped."}

		set ::no_server_response_count($sid) [expr $::no_server_response_max_count + 1]

	}
}

# proc AttachServerOutput {sid} {
# 	# attaches to server output
# 	spawn ssh -o "StrictHostKeyChecking no" -l $::server_login $::servers($sid)
# 	expect "password: "
# 	exp_send "$::server_pwd\n"
# 	expect -re $::server_prompt
# 	# attach to running process
# 	exp_send "$::server_workdir/$::dtach_cmd_server -a $::server_workdir/$::dtach_sockfile\n"
# 	set ::server_detach($sid) 0
# 	while {!$::server_detach($sid)} {
# 		set expect_out(buffer) ""
# 		expect -re "."
# 		if [regexp {[[:graph:][:space:]]} $expect_out(buffer)] {
# 			.so_$sid.f.log insert end $expect_out(buffer)
# 			.so_$sid.f.log see end
# 		}
# 	}
# 	# send detach command
# 	exp_send "\x1c"
# 	expect -re ".+~# "
# 
# 	exp_send "exit\n"
# 	wait -i $spawn_id
# 
# 	set ::server_detach($sid) 0
# 
# }


#######################################
# Client checks and controls

proc CheckClientExe {cid} {
	# checks for presence of client executable
	return [expr [catch {exec ls $::client_workdir$cid/$::client_exe}] == 0]
}

proc CheckClientProcess {cid} {
	# checks for running client process
	return [expr [catch {exec ps -ef | grep $::client_workdir$cid/$::client_exe | grep -v grep}] == 0]
}

proc CheckClientDescrambledFileOK {cid} {
	# checks for presence and correctness of descrambled file
	return [expr [catch {exec diff -q ./text.txt $::client_workdir$cid/$::textfile_descrambled}] == 0]
}

proc CheckClient {cid} {
	# perform all client checks and set client status
	# client executable available?
	if [CheckClientExe $cid] {
		# client process running?
		if [CheckClientProcess $cid] {
			set ::client_status($cid,process) "running"
		} else {
			# client not running -> descrambled file available and correct?
			if [CheckClientDescrambledFileOK $cid] {
				set ::client_status($cid,process) "not_running-descrambled_ok"
			} else {
				set ::client_status($cid,process) "not_running-descrambled_not_ok"
			}
		}
	} else {
		set ::client_status($cid,process) "exe_not_available"
	}

	if {$::debug} {UpdateStatusLog "Client ID $cid status: $::client_status($cid,process)"}
}

proc StartStopClient {cid} {
	# starts or stops client
	if {[regexp "not_running" $::client_status($cid,process)]} {
		if {$::debug} {UpdateStatusLog "Starting client ID $cid..."}

		# clear scramble counts
		foreach server_id $::server_ids {
			set ::scramble_count($server_id,$cid) {}
		}		
		
		# scramble text file and copy it to client working directory
		ScrambleTextFile
		catch {exec cp ./$::textfile_coded $::client_workdir$cid/} cp_result
		if {$::debug} {UpdateStatusLog "Scrambled textfile ./$::textfile_coded copied to $::client_workdir$cid/: $cp_result"}

		# start client
		# vslabc infile outfile client_id priority broadcast_ip
		catch {exec $::dtach_cmd_client -n $::client_workdir$cid/$::dtach_sockfile \
					$::client_workdir$cid/$::client_exe \
					$::client_workdir$cid/$::textfile_coded \
					$::client_workdir$cid/$::textfile_descrambled \
					$cid \
					$::client_priority($cid) \
					$::broadcast_ip} start_client_result
		if {$::debug} {UpdateStatusLog "Client ID $cid started: \
					$::dtach_cmd_client -n $::client_workdir$cid/$::dtach_sockfile \
					$::client_workdir$cid/$::client_exe \
					$::client_workdir$cid/$::textfile_coded \
					$::client_workdir$cid/$::textfile_descrambled \
					$cid \
					$::client_priority($cid) \
					$::broadcast_ip \n\
					$start_client_result"}

	} elseif {$::client_status($cid,process) == "running"} {
		if {$::debug} {UpdateStatusLog "Stopping client ID $cid..."}

		# kill client process
		if {![catch {exec ps -ef | grep $::client_workdir$cid/$::client_exe | grep -v grep | grep -v $::dtach_cmd_client | awk "{print \$2}"} process_number]} {
			if {![catch {exec kill -9 $process_number}]} {
				set kill_result "Process killed."
			} else {
				set kill_result "Could not kill process."
			}
		} else {
			set kill_result "No process to kill."
		}

		if {$::debug} {UpdateStatusLog "Client ID $cid: $kill_result"}
	}
}

#######################################
# Helper functions

proc UpdateStatusLog {statustext} {
	.fstatus.t insert end "[clock format [clock seconds] -format %H:%M:%S] $statustext\n"
	.fstatus.t see end
}

proc RemoveStoredSSHKeys {} {
	# remove stored SSH keys from key storage to avoid errors due to newly generated SSH keys on Zedboard startup
	foreach server_id $::server_ids {
		catch {exec ssh-keygen -R $::servers($server_id)} ssh_keygen_result
		if {$::debug} {UpdateStatusLog "SSH key of server ID $server_id removed."}
	}
}

proc GenerateTextFile {} {
	# generate uncoded text file with about $textfile_size bytes
	catch {exec lorem -p [expr int(round($::textfile_size / 204))] > ./$::textfile_uncoded}
	# read true textfile size
	catch {exec ls -la $::textfile_uncoded | awk "{print \$5}"} ::textfile_realsize
}

proc ScrambleTextFile {} {
	# scramble textfile with random generator polynom
	catch {exec $::textscrambler_cmd $::textfile_uncoded $::textfile_coded [expr int(65535*rand())]} scramble_result
	if {$::debug} {UpdateStatusLog "$scramble_result"}
}

proc GetDescrambledBytes {cid} {
	# cumulates number of descrambled bytes for a client
	set sum 0
	foreach sid $::server_ids {
		if {$::server_status($sid,cid) > -2} {
			set sum [expr $sum + [.fg_$sid$cid.l cget -text]]
		}
	}
	return $sum
}

#######################################
# GUI

# GUI: top-left panel
frame .ftopleft -relief solid -borderwidth 1
grid .ftopleft -row 0 -column 0 -sticky nesw

button .ftopleft.ex -text "QUIT" -font $fnt(small) -width 1 -command "set quit 1"
#button .ftopleft.bk -text "KILL" -font $fnt(small) -width 1
#button .ftopleft.bc -text "CLR" -font $fnt(small) -width 1
label .ftopleft.l -font $fnt(small) -text "Debug:"
checkbutton .ftopleft.c -variable debug
label .ftopleft.ls -text "Size:"  -font $fnt(small)
label .ftopleft.s -textvariable textfile_realsize -font $fnt(small)
#entry .ftopleft.e -textvariable broadcast_ip -font $fnt(small) -width 12

grid .ftopleft.ex -row 1 -column 1 -columnspan 1 -rowspan 1 -sticky w
#grid .ftopleft.bk -row 1 -column 2 -columnspan 1 -rowspan 1 -sticky w
#grid .ftopleft.bc -row 1 -column 3 -columnspan 1 -rowspan 1 -sticky w
grid .ftopleft.l -row 2 -column 1 -columnspan 2 -rowspan 1 -sticky w
grid .ftopleft.c -row 2 -column 3 -columnspan 1 -rowspan 1 -sticky w
grid .ftopleft.ls -row 3 -column 1 -columnspan 1 -rowspan 1 -sticky w
grid .ftopleft.s -row 3 -column 2 -columnspan 2 -rowspan 1 -sticky w
#grid .ftopleft.e -row 3 -column 1 -columnspan 6 -rowspan 1 -sticky w

# GUI: server panel
set i 1
foreach server_id $server_ids {
	frame .fs_$server_id -relief solid -borderwidth 1
	grid .fs_$server_id -row 0 -column $i -sticky nesw

	label .fs_$server_id.l -font $fnt(big) -text $server_id
	button .fs_$server_id.b -image $img(onoff) -command "StartStopServer $server_id"
	#button .fs_$server_id.t -image $img(terminal) -command "ShowServerOutput $server_id"
	#button .fs_$server_id.s -image $img(status)  -command "RequestServerStats $server_id"
	label .fs_$server_id.i -textvariable servers($server_id) -font $fnt(small) -width 12
	grid .fs_$server_id.l -row 1 -column 1 -columnspan 1 -rowspan 1
	grid .fs_$server_id.b -row 1 -column 2 -columnspan 1 -rowspan 1
	#grid .fs_$server_id.t -row 1 -column 3 -columnspan 1 -rowspan 1
	#grid .fs_$server_id.s -row 1 -column 4 -columnspan 1 -rowspan 1
	grid .fs_$server_id.i -row 2 -column 1 -columnspan 2 -rowspan 1 -sticky ew
	incr i
}

# GUI: client panel
set i 1
foreach client_id $client_ids {
	frame .fc_$client_id -relief solid -borderwidth 1
	grid .fc_$client_id -row $i -column 0 -sticky nesw

	label .fc_$client_id.l -font $fnt(big) -text $client_id
	button .fc_$client_id.b -image $img(onoff) -command "StartStopClient $client_id"
	entry .fc_$client_id.e -textvariable client_priority($client_id) -font $fnt(small) -width 2
	#button .fc_$client_id.s -image $img(status)
	ttk::progressbar .fc_$client_id.p -length 12 -maximum $::textfile_realsize
	label .fc_$client_id.i -textvariable clients($client_id) -font $fnt(small) -width 12
	grid .fc_$client_id.l -row 1 -column 1 -columnspan 1 -rowspan 1
	grid .fc_$client_id.b -row 1 -column 2 -columnspan 1 -rowspan 1
	grid .fc_$client_id.e -row 1 -column 3 -columnspan 1 -rowspan 1
	grid .fc_$client_id.p -row 2 -column 1 -columnspan 3 -rowspan 1 -sticky ew
	grid .fc_$client_id.i -row 3 -column 1 -columnspan 3 -rowspan 1 -sticky ew
	incr i
}

# GUI: grid
set row 0
foreach client_id $client_ids {
	set column 0
	foreach server_id $server_ids {
		frame .fg_$server_id$client_id -relief solid -borderwidth 1
		grid .fg_$server_id$client_id -row [expr $row + 1] -column [expr $column + 1] -sticky nesw
		
		label .fg_$server_id$client_id.l -font $fnt(big) -text "0"
		grid .fg_$server_id$client_id.l -row 1 -column 1 -columnspan 1 -rowspan 1 -sticky nesw
		
		incr column
	}
	incr row
}

# GUI: status log
frame .fstatus -relief solid -borderwidth 1
grid .fstatus -row [expr $nclients + 1] -column 0 -columnspan [expr $nservers + 1] -sticky nsew

text .fstatus.t -height 20 -wrap word -font $fnt(small) -yscrollcommand {.fstatus.sy set}
scrollbar .fstatus.sy -command {.fstatus.t yview}
grid .fstatus.t -row 1 -column 1 -sticky nsew
grid .fstatus.sy -row 1 -column 2 -sticky nsw
grid columnconfigure .fstatus 1 -weight 10

# # GUI: output log
# proc ShowServerOutput {sid} {
# 
# 	if {$::server_status($sid,process) == "running"} {
# 		set ::server_detach($sid) 0
# 		toplevel .so_$sid
# 		wm title .so_$sid "Server ID $sid: Output Log"
# 		wm attributes .so_$sid -fullscreen
# 
# 		frame .so_$sid.f
# 		grid .so_$sid.f -row 1 -column 1
# 
# 		text .so_$sid.f.log -width 80 -height 40 -font $::fnt(small) -borderwidth 2 -relief raised -setgrid true -yscrollcommand [list .so_$sid.f.scroll set]
# 		scrollbar .so_$sid.f.scroll -command [list .so_$sid.f.log yview]
# 		button .so_$sid.f.cl -text Close -command "set ::server_detach($sid) 1;	if {$::server_detach($sid) == 0} {destroy .so_$sid}"
# 		grid .so_$sid.f.scroll -row 1 -column 2 -sticky ns
# 		grid .so_$sid.f.log -row 1 -column 1 -sticky nesw
# 		grid .so_$sid.f.cl -row 2 -column 1
# 		
# 		AttachServerOutput $sid
# 	}
# }

# GUI: Key bindings
bind . <q> {set quit 1}


#####################################
# main loop

OpenServerChannels
RemoveStoredSSHKeys
GenerateTextFile
.fc_$client_id.p configure -maximum $::textfile_realsize

set quit 0
while {!$quit} {

	# server status and counters
	foreach server_id $server_ids {
		# check server status
 		CheckServer $server_id
		if {$::server_status($server_id,process) == "dead"} {
			set server_status_color gray
		} elseif {$::server_status($server_id,process) == "exe_not_available"} {
			set server_status_color yellow
		} elseif {$::server_status($server_id,process) == "not_running"} {
			set server_status_color red
		} elseif {$server_status($server_id,cid) == -1} {
			set server_status_color green
		} elseif {$server_status($server_id,cid) >= 0} {
			set server_status_color blue
		} else {
			set server_status_color darkred
		}

		# set button colors according to status
		.fs_$server_id.b configure -background $server_status_color -activebackground $server_status_color

		# set counters
		foreach client_id $client_ids {
			.fg_$server_id$client_id.l configure -text [expr [join $scramble_count($server_id,$client_id) +] + 0]
		}
		
		# update GUI
		update
	
		# quit?
		if $quit break
	}

	# client status
	foreach client_id $client_ids {
		# check client status
		CheckClient $client_id
		if {$::client_status($client_id,process) == "exe_not_available"} {
			set client_status_color yellow
		} elseif {$::client_status($client_id,process) == "not_running-descrambled_not_ok"} {
			set client_status_color red
		} elseif {$::client_status($client_id,process) == "not_running-descrambled_ok"} {
			set client_status_color green
		} elseif {$::client_status($client_id,process) == "running"} {
			set client_status_color blue
		} else {
			set client_status_color darkred
		}

		# set button colors according to status
		.fc_$client_id.b configure -background $client_status_color -activebackground $client_status_color
		
		# set progress bar according to number of descrambled bytes
		.fc_$client_id.p configure -value [GetDescrambledBytes $client_id]
		
		# check priority for non-numeric characters
		regsub -all {[^0-9]} $::client_priority($client_id) "" ::client_priority($client_id)
	}
	
	#update
	after $status_check_interval
}

CloseServerChannels
wait

exit
