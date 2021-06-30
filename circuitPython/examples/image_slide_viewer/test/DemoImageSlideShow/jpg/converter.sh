rm -rf compressed; mkdir -p compressed; for i in *.jpg; do j=`basename "$i"`; k="${j%.*}"; echo $k; echo y | ffmpeg -i "$i" -qscale:v 11 -compression_level 100 compressed/"$k"".jpg" ; done
