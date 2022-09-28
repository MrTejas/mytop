bluetoothctl paired-devices | cut -f2 -d' '|
# while read -r uuid
# do

uuid=$1F:98:81:A1:FF:22
info=`bluetoothctl info $uuid`
# echo "$info" | grep "Name"


if echo "$info" | grep -q "Connected: yes"; then
    echo "We have to disconnect"
    bluetoothctl disconnect 1F:98:81:A1:FF:22
    # echo "Disconnected Successfully..."
    # echo "$info" | grep "Name"
else
    # bluetoothctl disconnect "$info"
    echo "We have to connect" 
    bluetoothctl connect 1F:98:81:A1:FF:22
    # echo "Connected Successfully..."
    # echo "$info" | grep "Name"
fi

# done
