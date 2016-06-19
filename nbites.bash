export NBITES_DIR=<~/nbites>`  
export AL_DIR=<path-to-naoqi-sdk>`  
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path-to-nbites-dir>/ext/lib`  
export PATH=$nbites_dir/ext/bin:$PATH`

#Paste into .bashrc for git branch in the prompt!
#Plus colors: username in green, @ in default, hostname in blue, pwd in purple, $ and remaining text in default.
#Looks good if you're using black background, white/gray text.
#If you're in a git repository, branch appears in cyan.
export PS1='\[\033[01;32m\]\u\[\033[00m\]@\[\033[01;34m\]\h\[\033[00m\]:\[\033[01;35m\]\w\[\033[36m\]`git branch 2>/dev/null|cut -f2 -d\* -s`\[\033[00m\]$\[\033[00m\] '