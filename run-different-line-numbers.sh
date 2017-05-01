#!/bin/bash
topic=$1
fsa_address=$2
rhyme_address=$3
rhyme_inf=$4
encourage_add=$5
linenumber=$6 
server_add="localhost"

related_add="$(echo $topic | ./client $server_add 50001| ./client $server_add 50002)"
rhyme_log="data/rhyme_log_"$RANDOM
rhyme_add="$(echo $related_add  $encourage_add $rhyme_log $(($linenumber/2)) | ./client $server_add 50003 )" 
#head -20 $rhyme_add
fsa_uncomp="data/complete_fsa_without_rhyme-"$linenumber"-auto.fsa"
reorder_rhyme_file="data/reorder_rhyme_"$RANDOM
./reorder_rhymes <$rhyme_add > $reorder_rhyme_file
cat $fsa_uncomp > $fsa_address
./make_complete_fsa_with_rhyme-different_line_number <  $reorder_rhyme_file >> $fsa_address
cut -d ' ' -f2 $reorder_rhyme_file | tr '\n' ' ' | awk '{ for (i=NF; i>1; i--) printf("%s ",$i); print $1; }' | tr '_' ' ' > $rhyme_address


echo "##Rhyme Words"> $rhyme_inf
cut -d ' ' -f2 $reorder_rhyme_file >> $rhyme_inf
echo "" >> $rhyme_inf
echo "##Exact Rhyme Candidates">>$rhyme_inf
cat $rhyme_log>>$rhyme_inf 
#echo "##Rhyme info">> $rhyme_inf
#cat $encourage_add >> $rhyme_inf



# > data/rhyme_file
#echo $topic |./find_related_words > data/related_words
#./find_rhyme_words < data/related_words > data/rhyme_file
#./make_complete_fsa_with_rhyme < data/rhyme_file > $address


