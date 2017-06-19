#!/bin/bash

# get the SRTM data files and convert them for splat use

# License: Public domain / CC-0

# Takes one parameter: the continent to retrieve.  Valid values:
#
# Africa
# Australia
# Eurasia
# Islands
# North_America
# South_America

# path to topgraphy datafiles
TOPOFILEDIR=./
# local hgt file archive
HGTFILEDIR=raw-data


INDEXFILE=`mktemp`
FILELIST=`mktemp`

#URLs from where to fetch the tiles
SRTM3URL="http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/"

#higher res data, currently not implemented
#SRTM1URL="http://e4ftl01.cr.usgs.gov/SRTM/SRTMGL1.003/2000.02.11/" 

#Default options:
CONTINENT=unknown


# Check if all prerequisites are installed
if [ ! -x `which wget` ]; then
	echo "error: not found in path: wget"
	exit 1
fi

if [ ! -x `which unzip` ]; then
	echo "error: not found in path: unzip"
	exit 1
fi


helptext() {
cat <<EOF
Usage: $0 -c CONTINENT|-r [-d] [-h]
-h      display this helptext
-c CONTINENT
        specify the continent to download 
        Valid options are:
        North_America, South_America, Africa,
        Eurasia, Australia, Islands
EOF
}

#Extract commandline options, see helptext for explanation
while getopts ":c:h" opt; do
  case $opt in
    h)
      helptext
      exit 1
      ;;
    c)
      CONTINENT=$OPTARG
      case $CONTINENT in
	North_America|South_America|Africa|Eurasia|Australia|Islands)
		echo "Continent set to $CONTINENT"
		;;
	*)
		echo "Invalid continent: $CONTINENT"
		helptext
		exit 1
		;;
      esac
      INDEXURL=${SRTM3URL}${CONTINENT}/
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      helptext
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      helptext
      exit 1
      ;;
  esac
done


# Start to download tiles:
echo "getting index.. from $INDEXURL"
wget -q -O - $INDEXURL | \
sed -r -e '/hgt.zip/!d; s/.* ([NSWE0-9]+\.?hgt\.zip).*$/\1/;' \
> $FILELIST

#cp $FILELIST ./filelist

mkdir -p $HGTFILEDIR
mkdir -p $TOPOFILEDIR

echo "retrieving files..."
FILECOUNT=`wc -l $FILELIST`


echo "Starting processing of ${FILECOUNT} Files"

for FILE in $(cat $FILELIST); do
    echo "Downloading: ${FILE}"

    #TODO maybe check if file already exists in TOPODIR
    
    #download the tile
    wget -P $HGTFILEDIR -nv -N $INDEXURL$FILE
    
    #direct conversion
    echo "Unzip $FILE and then delete zip"
    unzip -o $HGTFILEDIR/$FILE -d $TOPOFILEDIR
    rm $HGTFILEDIR/$FILE
    
    # TODO comment for prod:
    #break;
done

#delete tempfiles
#rm $INDEXFILE
#rm $FILELIST

echo "Complete.  The files in $HGTFILEDIR may be removed." 
