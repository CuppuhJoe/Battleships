# WORKS CITED:
# https://stackoverflow.com/questions/8512462/looping-through-all-files-in-a-directory
# 
for file in *; do 
    if [ -f $file ]
    then 
        # echo "$file"
        if [[ $file == *.cpp ]]; then
			#handle C++
            echo "g++ -o "${file%.*}.ai" $file"
            g++ -std=c++11 -Wall -O3 -o "${file%.*}.ai" $file
            mv ${file%.*}.ai ../AI_Executables/"${file%.*}.ai"
		fi
		if [[ $file == *.go ]]; then
			#handle GoLang
            echo "go build -o ${file%.*}.ai $file"
            go build -o ${file%.*}.ai $file
            mv ${file%.*}.ai ../AI_Executables/"${file%.*}.ai"
		fi
		if [[ $file == *.py ]]; then
			#handle python
            echo "compiling python3 $file to ${file%.*}.ai"
            cp ./$file ./${file%.*}.ai
            mv ${file%.*}.ai ../AI_Executables/"${file%.*}.ai"
		fi
    fi 
done