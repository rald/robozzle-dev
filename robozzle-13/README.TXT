# robozzle

^   forward
<   turn left
>   turn right
R   paint red
G   paint green
B   paint blue
r   on red
g   on green
b   on blue
0-9 function call
X   return

TAB     switch CODE/RUN
SPACE   if in RUN MODE step
ESC     quit program


./game                      run game       
./rle L00.TXT E00.TXT       run length encode L00.TXT output to E00.TXT
./rld E00.TXT D00.TXT       run length decode E00.TXT output to D00.TXT

make        build binaries
make enc    encode levels
make dec    decode levels
