./run.sh --id 1 --hosts ../example/hosts --output ../example/output/1.output ../example/configs/fifo-broadcast.config --mode deliver

./run.sh --id 1 --hosts ../example/hosts --output ../example/output/1.output ../example/configs/fifo-broadcast.config --mode send

./run.sh --id 1 --hosts ../example/hosts --output ../example/output/1.output ../example/configs/fifo-broadcast.config --mode all

./run.sh --id 2 --hosts ../example/hosts --output ../example/output/2.output ../example/configs/fifo-broadcast.config --mode deliver

./run.sh --id 2 --hosts ../example/hosts --output ../example/output/2.output ../example/configs/fifo-broadcast.config --mode send

./run.sh --id 3 --hosts ../example/hosts --output ../example/output/3.output ../example/configs/fifo-broadcast.config --mode all