&> pcc_client.elg
./app/pccclient send 127.0.0.1 9000 --pcc-rate-control=bp -pyhelper=loaded_client -pypath=/home/ashrov/Documents/Research/Aurora/PCC-RL/src/udt-plugins/testing/ --history-len=10 --pcc-utility-calc=linear --model-path=/home/ashrov/Documents/Research/Aurora/PCC-RL/src/gym/saved_model &> pcc_client.elg &
