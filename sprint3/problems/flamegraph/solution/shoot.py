import argparse
import subprocess
import time
import random
import shlex

RANDOM_LIMIT = 1000
SEED = 123456789
random.seed(SEED)

AMMUNITION = [
    'localhost:8080/api/v1/maps/map1',
    'localhost:8080/api/v1/maps'
]

SHOOT_COUNT = 100
COOLDOWN = 0.1


def start_server():
    parser = argparse.ArgumentParser()
    parser.add_argument('server', type=str)
    return parser.parse_args().server


def run(command, output=None):
    process = subprocess.Popen(shlex.split(command), stdout=output, stderr=subprocess.DEVNULL)
    return process


def stop(process, wait=False):
    if process.poll() is None and wait:
        process.wait()
    process.terminate()


def shoot(ammo):
    hit = run('curl ' + ammo, output=subprocess.DEVNULL)
    time.sleep(COOLDOWN)
    stop(hit, wait=True)
    
def perf(pid):
	return "perf record -p " + str(pid) + " -o perf.data -gs"

def make_shots():
    for _ in range(SHOOT_COUNT):
        ammo_number = random.randrange(RANDOM_LIMIT) % len(AMMUNITION)
        shoot(AMMUNITION[ammo_number])
    print('Shooting complete')



server = run(start_server(), subprocess.DEVNULL)
perf_record = run(perf(server.pid))
make_shots()
perf_record.send_signal(signal.SIGINT)
stop(server)
time.sleep(1)
# https://sky.pro/media/ispolzovanie-operatora-with-v-python/ от сюда
with open("graph.svg", "w") as file:
    script = subprocess.Popen(shlex.split("perf script -i perf.data"), stdout=subprocess.PIPE)
    stackcollapse = subprocess.Popen(shlex.split("./FlameGraph/stackcollapse-perf.pl"), stdin=script.stdout, stdout=subprocess.PIPE)
    output = subprocess.Popen(shlex.split("./FlameGraph/flamegraph.pl"), stdin=stackcollapse.stdout, stdout=file)
    stop(script, True)
    stop(stackcollapse, True)
    stop(output, True)
print('Job done')
