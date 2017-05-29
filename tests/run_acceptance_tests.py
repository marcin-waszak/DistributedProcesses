from subprocess import Popen, PIPE, call, DEVNULL
from colored import fg, bg, attr
from time import sleep
import os

adminExecutable = './bin/admin'
serverExecutable = './bin/server'
workerExecutable = './bin/worker'

def runServer(*args):
    return Popen([serverExecutable,*args], stdout=DEVNULL, stderr=DEVNULL)

def runWorker(*args):
    return Popen([workerExecutable,*args], stdout=DEVNULL, stderr=DEVNULL)

def test0():
    inst = []
    inst.append(runServer('-a', '127.0.0.1'))
    inst.append(runWorker('-a', '127.0.0.1'))
    inst.append(runWorker('-a', '127.0.0.1'))
    inst.append(runWorker('-a', '127.0.0.1'))

    sleep(4)
    p = Popen([adminExecutable, '-a', '127.0.0.1', '-l', '-d'],stdout=PIPE)
    out = p.communicate()
    out = out[0].decode()

    expectedOut='''
0 -> 127.0.0.1
1 -> 127.0.0.1
2 -> 127.0.0.1
'''

    for i in inst:
        i.kill()

    lines1 = [l.strip() for l in out.strip().split('\n')         if l.strip()!='']
    lines2 = [l.strip() for l in expectedOut.strip().split('\n') if l.strip()!='']
    return lines1 == lines2

def test1():
    inst = []
    inst.append(runServer('-a', '::1'))
    inst.append(runWorker('-a', '::1'))
    inst.append(runWorker('-a', '::1'))
    inst.append(runWorker('-a', '::1'))

    sleep(4)
    p = Popen([adminExecutable, '-a', '::1', '-l', '-d'],stdout=PIPE)
    out = p.communicate()
    out = out[0].decode()

    expectedOut='''
0 -> ::1
1 -> ::1
2 -> ::1
'''

    for i in inst:
        i.kill()

    lines1 = [l.strip() for l in out.strip().split('\n')         if l.strip()!='']
    lines2 = [l.strip() for l in expectedOut.strip().split('\n') if l.strip()!='']
    return lines1 == lines2

if __name__ == "__main__":
    print("running tests")
    tests = [test0, test1]
    for n in range(100):
        for i,t in enumerate(tests):
            if not t():
                print ('%s Test %d failed %s' % (fg('red'), i, attr('reset')))
            else:
                print ('%s Test %d passed %s' % (fg('green'), i, attr('reset')))
            sleep(0.1)
