
DEBUGFS=`grep debugfs /proc/mounts | awk '{ print $2; }'`
TRACEDIR=$DEBUGFS/tracing
TRACED_PIDS_FILE=/tmp/.traced_pids

PINCPU=0
while getopts :p:h option
do
	case "$option" in
		p)
			PINCPU=1;;
		h|\?)
			echo "Usage: $0 [-p pin_cpu]"
			exit 0;;
	esac
done


function trace_event() {
	echo $1 >> $TRACEDIR/set_event
}
