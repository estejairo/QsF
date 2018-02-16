# echo tuple2tree($raiz/tuplas/E1up-05.1mm.root)

raiz=/Users/pablinho/SILAB/PET/QsF
echo $raiz
rm -f tuplas2arboles.cpp

n=0
echo --- $n ---

echo "{" | tee -a tuplas2arboles.cpp

for pp in $(ls $raiz/tuplas)
do

#echo "directorio=$raiz/data/$p1/$p2"
#echo "LeCroy2Root(\"$raiz/data/$p1/$p2/\",\"$raiz/tuplas/$p1-$p2.root\")" >> List.txt
#    ./read.py -i ../data/S1up/05.1mm -o ./tuplaOr.root
echo "tuple2tree(\"$raiz/tuplas/$pp\");" | tee -a tuplas2arboles.cpp

((n++))
echo --- $n ---
done


echo "}" | tee -a tuplas2arboles.cpp
