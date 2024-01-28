import random

Block_num = 300
Size_range = 100

upper_bound = 0.6
lower_bound = 1.75

with open('sample.in', 'w') as file:
    file.write("{} {}\n". format(upper_bound, lower_bound))
    
    for count in range(Block_num):
        file.write("b{} {} {}".format(count, random.randint(10, Size_range), random.randint(10, Size_range)))
        if (count != Block_num - 1):
            file.write("\n")

