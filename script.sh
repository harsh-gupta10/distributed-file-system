#!/bin/bash 
rm -r D1
rm -r D2
# rm -r ~/D3
mkdir D1
mkdir D2
# # # mkdir ~/D3

cp header.h D1
cp header.h D2
# # cp header.h ~/D3
cp Storage_Server_F.c D1
cp Storage_Server_F.c D2
# # cp Storage_Server_F.c ~/D3


# # cp client.c  ~/
# # cp header.h ~/

gcc naming_Server.c bookkeeping.c Triefinal.c LRUcaching.c -o nm

gcc D1/Storage_Server_F.c  -o D1/ss
gcc D2/Storage_Server_F.c  -o D2/ss
# gcc ~/D3/Storage_Server_F.c  -o ~/D3/ss



gcc client.c -o c
# # gcc ~/client.c -o ~/c

# # # gnome-terminal --working-directory="~/" & # c
# # # gnome-terminal --working-directory="./" & #nm
# # # gnome-terminal --working-directory="./" & #c
# # # gnome-terminal --working-directory="./D1" & #ss
# # # gnome-terminal --working-directory="./D2" & #ss
# # # gnome-terminal --working-directory="~/D3" & #ss
# # # gnome-terminal --working-directory="./" & #aise hi


gnome-terminal &
gnome-terminal &
# # gnome-terminal &
cd D1
gnome-terminal &
cd -
cd D2
gnome-terminal &
cd -
# # # cd ~/
# # # gnome-terminal &
# # cd -
# # # cd ~/D3
# # # gnome-terminal &
# # cd -

