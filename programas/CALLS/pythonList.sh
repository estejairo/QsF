
raiz=/Users/pablinho/SILAB/PET/QsF
echo $raiz
rm List.txt

n=0
echo --- $n ---

for p1 in $(ls $raiz/data)
do

for p2 in `ls $raiz/data/$p1`
do

#echo "directorio=$raiz/data/$p1/$p2"
#echo "LeCroy2Root(\"$raiz/data/$p1/$p2/\",\"$raiz/tuplas/$p1-$p2.root\")" >> List.txt
#    ./read.py -i ../data/S1up/05.1mm -o ./tuplaOr.root
echo "$raiz/Lecroy_util/read.py -i $raiz/data/$p1/$p2/ -o $raiz/tuplas/$p1-$p2.root" | tee -a List.txt

((n++))
echo --- $n ---

done

done
