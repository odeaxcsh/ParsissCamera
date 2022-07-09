from io import TextIOWrapper
import os
import argparse

files = [
    'Accurect-Pointer.txt',
    'Endonasal-RII.txt',
    'HeadBand-Reference.txt',
    'Navigation-Pointer.txt',
    'Registration-Pointer.txt'
]

def readFromOriginalFormat(file: TextIOWrapper):
    lines = file.readlines()
    for i, line in enumerate(lines):
        if line.startswith('Num Markers:'):
            numMarkers = int(line.split(':')[1].strip())
        if line.startswith('Marker Positions'):
            break
    
    data = lines[i+2:i+numMarkers+2]
    data = [float(x) for line in data  for x in line.split()]
    return data, numMarkers


def writeToNewFormat(data: list, numMarkers: int, file: TextIOWrapper):
    file.write(str(numMarkers) + '\n')
    for i in range(0, numMarkers*3, 3):
        file.write('{} {} {}\n'.format(data[i], data[i+1], data[i+2]))
    file.write('\n')


if __name__ == '__main__':
    argparser = argparse.ArgumentParser()
    argparser.add_argument('-d', '--directory', help='root directory of the files', required=True)

    args = argparser.parse_args()

    if not os.path.exists(args.directory):
        os.mkdir(f'{args.directory}/converted/')

    for file in files:
        with open(f'{args.directory}/{file}', 'r') as f:
            data, numMarkers = readFromOriginalFormat(f)
            
        with open(f'{args.directory}/converted/{file}', 'w') as f:
            writeToNewFormat(data, numMarkers, f)
            print(f'{file} converted')
    
    