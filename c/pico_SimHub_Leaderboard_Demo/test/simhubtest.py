import serial
import time
import random

# Commands from SimHub:
# Simulate with python script
	# >python simhubtest.py
# Identify driver:
	# !PD;<car number>;<driver name>;<time>
# Examples:
	# !PD;99;27;Jimmy McRae;0.000

# Notify new lap time:
	# !PL;<lap number>;<lap time>
# Examples:
	# !PL;1;34:35.369
	# !PL;2;32:31.467

# Update rankings:
	# Start ranking update: !PS 
	# Rank ahead: !PA;<places ahead>;<car number>;<driver name>;<time difference>
	# Rank behind: !PB;<places ahead>;<car number>;<driver name>;<time difference>
	# Commit ranking update: !PU
# Examples:
	# !PS
	# !PA;4;2;Russell Brooks;-03.436
	# !PA;3;3;Jim Clark;-03.464
	# !PA;2;4;Jackie Stewart;-02.441
	# !PA;1;5;Stirling Moss;-01.122
	# !PB;1;6;Mike Hawthorn;+00.101
	# !PB;2;7;Nigel Mansell;+00.339
	# !PB;3;8;Graham Hill;+01.044
	# !PB;4;9;James Hunt;+01.504
	# !PU

# Set to non-zero to print commands sent to the pico.
debugprint = 0
# Set to non-zero to print the updated information.
infoprint = 1

driverlist = [{"number":1, "name":"Tony Pond", "car":"TR7"},
              {"number":2, "name":"Russell Brooks", "car":"RS2000"},
              {"number":3, "name":"Jim Clark", "car":"Lotus"},
              {"number":4, "name":"Jackie Stewart", "car":"Stewart"},
              {"number":5, "name":"Stirling Moss", "car":"Lotus"},
              {"number":6, "name":"Mike Hawthorn", "car":"Jaguar"},
              {"number":7, "name":"Nigel Mansell", "car":"McLaren"},
              {"number":8, "name":"Graham Hill", "car":"BRM"},
              {"number":9, "name":"James Hunt", "car":"Hesketh"},
              {"number":10, "name":"John Surtees", "car":"Ferrari"},
              {"number":11, "name":"Tony Brookes", "car":"BRM"},
              {"number":12, "name":"David Coulthard", "car":"Williams"},
              {"number":13, "name":"Jenson Button", "car":"BAR"},
              {"number":14, "name":"Jason Plato", "car":"Vauxhall"},
              {"number":15, "name":"Will Hoy", "car":"BMW"},
              {"number":16, "name":"Andy Rouse", "car":"Rover"},
              ]
# Number of before/ahead on the display.
displaybehind = 5
displayahead = 5

# Connect to the pico on a Virtual COM Port.
serialPort = serial.Serial(
    port="COM18", baudrate=115200, bytesize=8, rtscts=True, timeout=2, stopbits=serial.STOPBITS_ONE
)
# check which COM Port was really used.
if (infoprint): print('Using ' + serialPort.name)

# Start at lap 1.
lap = 1

# Send an empty command to reset the device.
serialPort.write(b'\r\n')

# Send the driver information string.
driverstring = f'!PD;99;27;Jimmy McRae; 0.000'
serialPort.write(driverstring.encode(encoding='utf-8', errors='replace'))
serialPort.write(b'\r\n')
if (infoprint): print('I am Jimmy McRae')
if (debugprint): print('-> ' + driverstring) # For debug

laptm = 0
lapts = 0
laptms = 0

while 1:
    
    # Swap some race positions around.
    rnd1 = random.randint(0, len(driverlist) - 1)
    rnd2 = random.randint(0, len(driverlist) - 1)
    
    if (rnd1 > rnd2):
        rndt = rnd2;
        rnd2 = rnd1;
        rnd1 = rndt;

    tmp = driverlist[rnd1];
    i = rnd1
    while i < rnd2:
        driverlist[i] = driverlist[i+1];
        i+=1
    driverlist[rnd2] = tmp;

    # Wait for a second before updating anything.
    time.sleep(1)

    timestring = f'!PL;{lap};{laptm:02d}:{lapts:02d}.{laptms:03d}'
    serialPort.write(timestring.encode(encoding='utf-8', errors='replace'))
    serialPort.write(b'\r\n')
    if (infoprint): print(f'Lap {lap} {laptm:02d}:{lapts:02d}.{laptms:03d}')
    if (debugprint): print('-> ' + timestring)

    # Lap time update for next pass.
    if (random.randint(0,100) > 75):
        laptm = random.randint(0,59)
        lapts = random.randint(0,59)
        laptms = random.randint(0,999)
        lap += 1

    updatestring = f'!PS'
    serialPort.write(updatestring.encode(encoding='utf-8', errors='replace'))
    serialPort.write(b'\r\n')
    if (infoprint): print('Rankings...')
    if (debugprint): print('-> ' + updatestring)
    
    # Ahead rankings.
    i = displayahead
    ts = 0
    tms = 0
    while i > 0:
        ts += random.randint(0,1)
        tms += random.randint(0,999)
        if (tms > 1000):
            ts += 1
            tms -= 1000
        rankstring = f'!PA;{i - 1};{driverlist[displayahead - i - 1]["number"]};{driverlist[displayahead - i - 1]["name"]};-{ts:02d}.{tms:03d}'
        serialPort.write(rankstring.encode(encoding='utf-8', errors='replace'))
        serialPort.write(b'\r\n')
        if (infoprint): print(f'Rank -{i - 1} {driverlist[displayahead - i - 1]["number"]};{driverlist[displayahead - i - 1]["name"]};-{ts:02d}.{tms:03d}')
        if (debugprint): print('-> ' + rankstring)
        i -= 1

    # Behind rankings.
    i = 0
    ts = 0
    tms = 0
    while i < displaybehind:
        ts += random.randint(0,1)
        tms += random.randint(0,999)
        if (tms > 1000):
            ts += 1
            tms -= 1000
        rankstring = f'!PB;{i};{driverlist[i + displaybehind]["number"]};{driverlist[i + displaybehind]["name"]};+{ts:02d}.{tms:03d}'
        serialPort.write(rankstring.encode(encoding='utf-8', errors='replace'))
        serialPort.write(b'\r\n')
        if (infoprint): print(f'Rank +{i} {driverlist[i + displaybehind]["number"]};{driverlist[i + displaybehind]["name"]};+{ts:02d}.{tms:03d}')
        if (debugprint): print('-> ' + rankstring)
        i += 1

    updatestring = f'!PU'
    serialPort.write(updatestring.encode(encoding='utf-8', errors='replace'))
    serialPort.write(b'\r\n')
    if (infoprint): print('...Update')
    if (debugprint): print('-> ' + updatestring)
    print()