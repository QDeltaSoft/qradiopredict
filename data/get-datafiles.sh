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
TOPOFILEDIR=splat-datafiles/sdf/
# local hgt file archive
HGTFILEDIR=splat-datafiles/hgtzip/

#SRTM2SDF_HD=srtm2sdf-hd
#SRTM2SDF=srtm2sdf

#INDEXFILE=`mktemp`
#FILELIST=`mktemp`

INDEXFILE=./indexfile.txt
FILELIST=./filelist.txt

#URLs from where to fetch the tiles
SRTM3URL="http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/"
SRTM1URL="http://e4ftl01.cr.usgs.gov/SRTM/SRTMGL1.003/2000.02.11/" 


#Default options:
CONTINENT=unknown


# Check if all prerequisites are installed
if [ ! -x `which readlink` ]; then
	echo "error: not found in path: readlink"
	exit 1
fi

if [ ! -x `which wget` ]; then
	echo "error: not found in path: wget"
	exit 1
fi

if [ ! -x `which unzip` ]; then
	echo "error: not found in path: unzip"
	exit 1
fi

if [ ! -x `which bzip2` ]; then
	echo "error: not found in path: bzip2"
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
-r      Download High Resolution SRTM data for use
        with splat-hd.
        The whole world will be downloaded, no
        separation between continents!     

EOF
}

#Extract commandline options, see helptext for explanation
while getopts ":dc:rhx:y:" opt; do
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
    r)
      USE_HIGHRES=true
      #unfortunately there is no listing per continent
      INDEXURL=$SRTM1URL
      echo "HIGH RESOLUTION: Using $SRTM2SDF_HD instead of srtm2sdf"
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

if [ "$USE_HIGHRES" = false ] && [ "$CONTINENT" = unknown ]; then
    echo "no arguments given"
    helptext
    exit 1
fi


# Start to download tiles:
echo "getting index.. from $INDEXURL"
wget -q -O - $INDEXURL > $INDEXFILE
	
if [ "$USE_HIGHRES" = true ]
then  
   #random magic stolen from the internet
   grep -F '.hgt.zip<' $INDEXFILE | sed -e 's@.*href="@@g' -e 's/">.*//g' > $FILELIST
else
    wget -q -O - $INDEXURL | \
	sed -r -e '/hgt.zip/!d; s/.* ([NSWE0-9]+\.?hgt\.zip).*$/\1/;' \
	> $FILELIST
fi

#cp $FILELIST ./filelist

mkdir -p $HGTFILEDIR
mkdir -p $TOPOFILEDIR

echo "retrieving files..."
FILECOUNT=`wc -l $FILELIST`
echo "Starting processing of ${FILECOUNT} Files"

#convert to absolute path because srtm2sdf does not accept output path arguments
HGTREALPATH=`readlink -f $HGTFILEDIR`
TOPOREALPATH=`readlink -f $TOPOFILEDIR`
PWD=`pwd`

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
