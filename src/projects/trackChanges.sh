for d in */ ; do cd "$d"; echo "$d"; git status --porcelain ; cd ..; done
