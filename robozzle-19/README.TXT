# robozzle

goal collect all the stars *

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
.   erase

ARROWS  move cursor
TAB     switch CODE/RUN
SPACE   if in RUN MODE step
ESC     quit program

./game                      run game       
./rle L00.TXT E00.TXT       run length encode L00.TXT output to E00.TXT
./rld E00.TXT D00.TXT       run length decode E00.TXT output to D00.TXT

make        build binaries
make enc    encode levels
make dec    decode levels
make clean  clean all

LEVEL FORMAT:
<width>,<height>,<player_x>,<player_y>,<player_direction>

player direction:
0 up
1 right
2 down
3 left

player level cell OCTAL:
421
?00 BLACK
?01 RED
?10 GREEN
?11 BLUE
0?? no star
1?? star

COMPRESSED LEVEL FORMAT:
<width>,<height>,<player_x>,<player_y>,<player_direction>,<compressed level data>

compressed level data format:
S:=<hex><data>S|<data>S|<empty>
<hex>:=0..9|A..F
<data>:=G..N
