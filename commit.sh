#!/bin/bash

projectName="Hex\ Enclave";
projectColor="\E[1m\E[30m\E[46m";

itemName[1]="tom";
attributes[1]="\E[1m\E[97m";

itemName[2]="coords";
attributes[2]="\E[1m\E[34m";

itemName[3]="generator";
attributes[3]="\E[1m\E[93m";

itemName[4]="hex";
attributes[4]="\E[1m\E[94m";


function commitLibrary
{
	 echo -e "${attributes[$1]}${itemName[$1]}\E[0m";
	 git add "${itemName[$1]}.cpp";
	 git add "${itemName[$1]}.hpp";
}

clear
echo -ne "\e[3J"

len=${#itemName[@]}+1

for (( i=0; i<$len; i++ ))
do
	 commitLibrary $i;
done

echo -e "\E[1mbuild.sh\E[0m";
git add build.sh;

echo -e "\E[1mcommit.sh\E[0m";
git add commit.sh;

echo -e "$projectColor""main.cpp\E[0m";
git add main.cpp;

echo -e "\E[1m\E[37m\E[105mCommiting.\E[0m";
git commit;

echo -e "\E[1m\E[97m\E[42m$projectName commited successfully.\E[0m";
