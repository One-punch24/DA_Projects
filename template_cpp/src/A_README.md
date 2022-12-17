# Build the application:
./build.sh

./run.sh --id 1 --hosts ../example/hosts --output ../example/output/1.output ../example/configs/lattice-agreement-1.config

./run.sh --id 2 --hosts ../example/hosts --output ../example/output/2.output ../example/configs/lattice-agreement-2.config

./run.sh --id 3 --hosts ../example/hosts --output ../example/output/3.output ../example/configs/lattice-agreement-3.config

# Debug
./run.sh --id 1 --hosts ../example/hosts --output ../example/output/1.output ../example/configs/lattice-agreement-1.config send

./run.sh --id 1 --hosts ../example/hosts --output ../example/output/1.output ../example/configs/lattice-agreement-1.config  deliver

./run.sh --id 2 --hosts ../example/hosts --output ../example/output/2.output ../example/configs/lattice-agreement-2.config send

./run.sh --id 2 --hosts ../example/hosts --output ../example/output/2.output ../example/configs/lattice-agreement-2.config deliver

./run.sh --id 3 --hosts ../example/hosts --output ../example/output/3.output ../example/configs/lattice-agreement-3.config send

./run.sh --id 3 --hosts ../example/hosts --output ../example/output/3.output ../example/configs/lattice-agreement-3.config deliver