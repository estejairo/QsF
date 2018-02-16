
raiz=/Users/pablinho/SILAB/PET/QsF
echo $raiz

n=0
echo --- $n ---

for p1 in $(ls $raiz/data)
do

for p2 in `ls $raiz/data/$p1`
do

dir=$raiz/data/$p1/$p2
echo "directorio=$dir"
echo ultimo evento registrado por canal
eve1=`ls $dir/C1*.trc | tail -n 1`
eve2=`ls $dir/C2*.trc | tail -n 1`
eve3=`ls $dir/C3*.trc | tail -n 1`
echo $eve1
echo $eve2
echo $eve3

num1=${eve1#*_};num1=${num1%.trc};echo $num1;
num2=${eve2#*_};num2=${num2%.trc};echo $num2;
num3=${eve3#*_};num3=${num3%.trc};echo $num3;

if [[ $num1 = $num2 && $num1 = $num3 ]];
then echo iguales;
else echo -------------------------------------- diferentes - - - -;
ar=($num1 $num2 $num3)
max=$(echo "${ar[*]}" | sort -nr | head -n 1)

fi


((n++))
echo --- $n ---

done

done
