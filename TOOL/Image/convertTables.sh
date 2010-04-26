
files=`find ~/robocup/nao-man/tables/ -name "*.mtb"`
for file in $files
do
    #echo $file
    newFile=~/Desktop/newtables/$file
    touch $newFile
    java ConvertTable $file $newFile
done

