seq 0 9 | while read num; do echo $num | nc localhost 12345; sleep 1; done
seq 0 9 | xargs -I {} sh -c 'echo {} | nc localhost 12345; sleep 1'
for i in {0..9}; do echo $i | nc localhost 12345; sleep 1; done


seq 0 9 | nc localhost 12345

seq 0 9 | nc localhost 12345 & \
seq 0 9 | nc localhost 12345 & \
wait
