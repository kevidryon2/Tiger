if test -z $1
then
	echo "Usage: $0 [directory] [size]"
	echo "Mounts a directory to a ramfs; if the [size] argument is omitted, the default size of 2G will be used"
	exit 1
fi

if test $(whoami) == "root"
then
	echo Mounting...
	if test -z $2; then
		mount --verbose -o ramfs size=2g -t ramfs $1
		if test $? -eq 0; then
			echo "Mounted successfully."
			exit 0
		else
			echo "Can't mount; see error above for info."
			exit 1
		fi
	fi
else
	echo "This script must be executed as root."
	exit 1
fi
