mkfifo pipe
(cat pipe | while IFS= read -r -n1 char; do
  ./toolbox ls
  sleep 0.05
done) &
exec > pipe
