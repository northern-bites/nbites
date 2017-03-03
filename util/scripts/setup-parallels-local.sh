FILE="/etc/hosts"

declare -a output=("#nbites robot addresses" "169.254.75.9 wash.local"
"169.254.75.10 river.local" "169.254.75.11 jayne.local" 
"169.254.75.12 simon.local" "169.254.75.13 inara.local"
"169.254.75.14 kaylee.local" "169.254.75.15 vera.local"
"169.254.75.16 mal.local" "169.254.75.17 zoe.local"
"169.254.75.18 blt.local" "169.254.75.19 elektra.local"
"169.254.75.20 wasp.local" "169.254.75.21 buzz.local"
"169.254.75.22 batman.local" "169.254.75.23 shehulk.local"
"192.168.110.30 starlord.local" )

echo
echo "Checking to see if the file exists"

if [ -f "$FILE" ]
then
echo
echo "$FILE found."
echo
echo "Starting to copy the names:"
else
echo "$FILE not found. Please make sure that the file path is correct then try again."
exit 1
fi
for i in "${output[@]}"
do
    echo "$i" | sudo tee -a $FILE
done

echo "Transfer complete!"