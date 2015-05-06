
DEBUGFS=`grep debugfs /proc/mounts | awk '{ print $2; }'`
TRACEDIR=$DEBUGFS/tracing

TTRACER_DIR=/tmp/trace_result
TTRACER_EXCLUSIVE_LOCK=$TTRACER_DIR/.lock
TRACED_PIDS_FILE=$TTRACER_DIR/traced_pids
OUTDIR=$TTRACER_DIR/$(date +"%Y%m%d-%H:%M:%S")

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
