#Script to setup git for Level 1 Noobs
echo "Enter your name"
read FULL_NAME
echo "Enter your email"
read EMAIL

echo "Setting Global Constants for Git..."
git config --global user.name $FULL_NAME
git config --global user.email $EMAIL
