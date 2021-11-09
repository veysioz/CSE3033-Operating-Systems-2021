#! /bin/bash

#project1

#Abbas Göktuğ Yılmaz 150115061
#Nurefşan Yücel 150119802
#Veysi Öz 150116005

#My program - 5

#Calculates the sum via co of numbers by exchanging consecutive digits
#program created on macos

#program first takes a number 
#loop condition checks if number has more than 1 digit 
#finds right most digit with mod 
#then reduce a digit from number by dividing number to 10 
#then finds second digit from right 
#then changes digits places by multiplying by 10 and summing with second right digit 
#loop continous untill 1 digit left
#then prints sum

num=$1

sum=0

while [ $num -gt 9 ] #number should have at least 2 digit
do
	mod=$((num % 10)) #for most right digit
	num=$((num / 10)) #to reduce a digit
	mod2=$((num % 10)) #for right digit after most right digit
	temp=$((10 * mod + mod2)) #to change digits place
	sum=$((sum + temp)) #add each digit to sum
done

echo $sum
